#include "pch.h"
#include "Enemy_Event_Sound.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"

//Engine
#include "UI_Manager.h"

CEnemy_Event_Sound::CEnemy_Event_Sound(const _wstring _strName, _float _fVolume, _int _iChannel, _wstring _strQuote, _float _fDuration)
	: m_strName(_strName)
	, m_iChannel(_iChannel)
	, m_fVolume(_fVolume)
	, m_fDuration(_fDuration)
	, m_strQuote(_strQuote)
{
}

HRESULT CEnemy_Event_Sound::Execute()
{
	CGameInstance::GetInstance()->Play_Sound(m_strName, m_iChannel, m_fVolume);

	if (m_fDuration == 0)
		return S_OK;

	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
	pUIManager->Show_Caption(m_strQuote.c_str(), m_fDuration);

	return S_OK;
}

CEnemy_Event_Sound* CEnemy_Event_Sound::Create(CGameObject* _pGameObject, const _wstring _strName, _float _fVolume,  _uint _iChannel, _wstring _strQuote, _float _fDuration)
{
	CEnemy_Event_Sound* pInstance = new CEnemy_Event_Sound(_strName, _fVolume, _iChannel, _strQuote, _fDuration);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEnemy_Event_Sound");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Event_Sound::Free()
{
	__super::Free();
}
