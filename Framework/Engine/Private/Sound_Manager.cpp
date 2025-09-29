#include "Sound_Manager.h"

CSound_Manager::CSound_Manager()
{
    m_Channels.fill(nullptr);
	m_Volumes.fill(1.f);
}

HRESULT CSound_Manager::Initialize()
{
	FMOD::System_Create(&m_pSystem);
	if (FMOD_OK != m_pSystem->init(32, FMOD_INIT_NORMAL, nullptr))
		return E_FAIL;
	LoadSoundFile();

	return S_OK;
}

HRESULT CSound_Manager::Update(_float fTimeDelta)
{
	Update_Fade(fTimeDelta);

	return S_OK;
}

void CSound_Manager::Update_Fade(_float fTimeDelta)
{
	for (auto it = m_FadeSwaps.begin(); it != m_FadeSwaps.end(); )
	{
		it->fCurrentTime += fTimeDelta;
		_float fRatio = min(it->fCurrentTime / it->fSwapTime, 1.f);

		switch (it->eState)
		{
		case FADESTATE::FADEOUT:
		{
			_float fVolume = it->fStartVolume * (1.f - fRatio);
			if (m_Channels[it->iChannel])
				m_Channels[it->iChannel]->setVolume(fVolume * m_fMasterVolume * m_Volumes[it->iChannel]);

			if (fRatio >= 1.f)
			{
				if (m_Channels[it->iChannel])
					m_Channels[it->iChannel]->stop();

				Play_Sound_Loop(it->strNewSoundTag, it->iChannel, 0.f);

				it->eState = FADESTATE::FADEIN;
				it->fCurrentTime = 0.f;
			}
			break;
		}

		case FADESTATE::FADEIN:
		{
			_float fVolume = it->fNewVolume * fRatio;
			if (m_Channels[it->iChannel])
				m_Channels[it->iChannel]->setVolume(fVolume * m_fMasterVolume * m_Volumes[it->iChannel]);

			if (fRatio >= 1.f)
			{
				it = m_FadeSwaps.erase(it);
				continue;
			}
			break;
		}
		}

		++it;
	}
}

void CSound_Manager::Play_Sound(const _wstring& strSoundTag, _uint iChannelID, _float fVolume)
{
	auto iter = m_Sounds.find(strSoundTag);
	if (iter == m_Sounds.end())
		return;

	m_pSystem->playSound(iter->second, nullptr, false, &m_Channels[iChannelID]);
	m_Channels[iChannelID]->setVolume(fVolume * m_fMasterVolume * m_Volumes[iChannelID]);

	m_pSystem->update();
}

void CSound_Manager::Play_Sound_Loop(const _wstring& strSoundTag, _uint iChannelID, _float fVolume)
{
	auto iter = m_Sounds.find(strSoundTag);

	if (iter == m_Sounds.end())
		return;

	m_pSystem->playSound(iter->second, nullptr, false, &m_Channels[iChannelID]);
	m_Channels[iChannelID]->setMode(FMOD_LOOP_NORMAL);
	m_Channels[iChannelID]->setVolume(fVolume * m_fMasterVolume * m_Volumes[iChannelID]);

	m_pSystem->update();
}

void CSound_Manager::Stop_Sound(_uint iChannelID)
{
	if (m_Channels[iChannelID])
	{
		m_Channels[iChannelID]->stop();
	}
}

void CSound_Manager::Stop_Sound_All()
{
	for (int i = 0; i < SOUND_END; ++i)
	{
		if (nullptr != m_Channels[i])
		{
			m_Channels[i]->stop();
		}
	}
}

_bool CSound_Manager::IsPlaying(_uint iChannelID)
{
	_bool isPlaying = { false };

	m_Channels[iChannelID]->isPlaying(&isPlaying);

	return isPlaying;
}

void CSound_Manager::Set_MasterVolume(_float fVolume)
{
	for (_uint i = 0; i < SOUND_END; i++)
	{
		_float fAdjustedVolume = 0.f;
		m_Channels[i]->getVolume(&fAdjustedVolume);

		_float fRawVolume = fAdjustedVolume / m_fMasterVolume;
		m_Channels[i]->setVolume(fRawVolume * fVolume);
		m_fMasterVolume = fVolume;
	}
	m_pSystem->update();
}

void CSound_Manager::Set_ChannelVolume(_uint iChannelID, _float fVolume)
{
	if (m_Channels[iChannelID])
	{
		_float fAdjustedVolume = 0.f;
		_float fCurrentVolume = m_Volumes[iChannelID];
		m_Channels[iChannelID]->getVolume(&fAdjustedVolume);

		_float fRawVolume = fAdjustedVolume / fCurrentVolume;
		m_Channels[iChannelID]->setVolume(fRawVolume * fVolume);
		m_Volumes[iChannelID] = fVolume;
	}
	m_pSystem->update();
}

void CSound_Manager::Set_RawChannelVolume(_uint iChannelID, _float fVolume)
{
	if (m_Channels[iChannelID])
	{
		m_Channels[iChannelID]->setVolume(fVolume * m_Volumes[iChannelID] * m_fMasterVolume);
	}
	m_pSystem->update();
}

void CSound_Manager::Set_ChannelVolume_With_SpatialDesc(_uint iChannelID, _vec3 vPosition, _vec2 vDistRange, _vec2 vVolumeRange)
{
	_float fDistance = (vPosition - m_vListenerPosition).Length();

	_float fRatio = (fDistance - vDistRange.x) / (vDistRange.y - vDistRange.x);
	fRatio = min(1.f, max(0.f, fRatio)); /* 가까우면 볼륨 1. 멀면 볼륨 0. */
	_float vVolume = 1.f - fRatio;
	vVolume = CUtils::Lerp_Float(vVolumeRange.x, vVolumeRange.y, vVolume);

	Set_RawChannelVolume(iChannelID, vVolume);
}

void CSound_Manager::FadeSwap_Sound(const _wstring& strNewSoundTag, _uint iChannelID, _float fSwapTime, _float fVolume)
{
	if (!m_Channels[iChannelID])
		return;

	FadeSwapInfo info;
	info.strNewSoundTag = strNewSoundTag;
	info.iChannel = iChannelID;
	info.fNewVolume = fVolume;
	info.fSwapTime = fSwapTime;

	// 현재 볼륨 가져오기
	m_Channels[iChannelID]->getVolume(&info.fStartVolume);

	m_FadeSwaps.push_back(info);
}

void CSound_Manager::LoadSoundFile(const _string& strPath)
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	_string strFile = strPath + "*.*";
	intptr_t handle = _findfirst(strFile.c_str(), &fd);

	if (handle == -1)
		return;

	_int iResult = 0;

	_char szCurPath[MAX_PATH] = "";	 // 상대 경로
	strcpy_s(szCurPath, strPath.c_str());
	_char szFullPath[MAX_PATH] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../Sound/" + "Success.wav"
		strcat_s(szFullPath, fd.name);
		// "../Sound/Success.wav"

		FMOD::Sound* pSound = nullptr;

		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_DEFAULT, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			size_t iLength = strlen(fd.name) + 1;

			_tchar szSoundKey[MAX_PATH]{};
			ZeroMemory(szSoundKey, sizeof(_tchar) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, fd.name, -1, szSoundKey, static_cast<int>(iLength));

			m_Sounds.emplace(szSoundKey, pSound);
		}
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	m_pSystem->update();

	_findclose(handle);
}

void CSound_Manager::Set_ListenerPositoin(const _vec3& vPosition)
{
	m_vListenerPosition = vPosition;
}

void CSound_Manager::Clear()
{
	Stop_Sound_All();
}

void CSound_Manager::LoadSoundFile()
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst("../../Client/Bin/Resources/Sound/*.*", &fd);

	if (handle == -1)
		return;

	_int iResult = 0;

	_char szCurPath[MAX_PATH] = "../../Client/Bin/Resources/Sound/";	 // 상대 경로
	_char szFullPath[MAX_PATH] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../Sound/" + "Success.wav"
		strcat_s(szFullPath, fd.name);
		// "../Sound/Success.wav"

		FMOD::Sound* pSound = nullptr;

		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_DEFAULT, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			size_t iLength = strlen(fd.name) + 1;

			_tchar szSoundKey[MAX_PATH] {};
			ZeroMemory(szSoundKey, sizeof(_tchar) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, fd.name, -1, szSoundKey, static_cast<int>(iLength));

			m_Sounds.emplace(szSoundKey, pSound);
		}
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	m_pSystem->update();

	_findclose(handle);
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Create : CSound_Manager");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CSound_Manager::Free()
{
    super::Free();

	Clear();

	for (auto& Pair : m_Sounds)
	{
		Pair.second->release();
		//Safe_Release(Pair.second);
	}
	m_Sounds.clear();

	m_pSystem->close();
	m_pSystem->release();
}
