#pragma once

#include "Base.h"
#include "DoubleBuffer.h"

BEGIN(Engine)
class CSound_Manager final : public CBase
{
	using super = CBase;

	enum  FADESTATE
	{
		FADEOUT,
		FADEIN
	};

	struct FadeSwapInfo
	{
		wstring strNewSoundTag;
		_uint iChannel;
		_float fNewVolume;

		_float fCurrentTime = 0.f;
		_float fSwapTime = 0.f;
		_float fStartVolume = 1.f;

		FADESTATE eState = FADESTATE::FADEOUT;
	};

private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Update(_float fTimeDelta);
public:
	void Play_Sound(const _wstring& strSoundTag, _uint iChannelID, _float fVolume);
	void Play_Sound_Loop(const _wstring& strSoundTag, _uint iChannelID, _float fVolume);
	void Stop_Sound(_uint iChannelID);
	void Stop_Sound_All();
	_bool IsPlaying(_uint iChannelID);

	void Set_MasterVolume(_float fVolume);
	void Set_ChannelVolume(_uint iChannelID, _float fVolume); /* m_Volume 을 조절 */
	void Set_RawChannelVolume(_uint iChannelID, _float fVolume); /* 실제 오디오 파일에서 여기로 넣은 값을 조절( 원래 넣던거 ) */
	void Set_ChannelVolume_With_SpatialDesc(_uint iChannelID, _vec3 vPosition, _vec2 vDistRange, _vec2 vVolumeRange);

	void FadeSwap_Sound(const _wstring& strNewSoundTag, _uint iChannelID, _float fSwapTime, _float fVolume);

	void LoadSoundFile(const _string& strPath);                                                    
	
	void Set_ListenerPositoin(const _vec3& vPosition);

	void Clear();

private:
	map<_wstring, FMOD::Sound*> m_Sounds;
	std::array<FMOD::Channel*, SOUND_END> m_Channels {};
	FMOD::System* m_pSystem { nullptr };

	vector<FadeSwapInfo> m_FadeSwaps;

	array<_float, SOUND_END>	m_Volumes;
	_float						m_fMasterVolume{ 1.f };

	_vec3					m_vListenerPosition {};

private:
	void LoadSoundFile();

	void Update_Fade(_float fTimeDelta);

public:
	static CSound_Manager* Create();
	virtual void Free();
};
END