#include "pch.h"
#include "Event_BackStepAttack_Sound.h"

#include "GameInstance.h"
#include "Player.h"

CEvent_BackStepAttack_Sound::CEvent_BackStepAttack_Sound()
{
}

HRESULT CEvent_BackStepAttack_Sound::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_BackStepAttack_Sound::Execute()
{
	m_pPlayer->Play_PlayerSound(L"BackStepAttack");

	return S_OK;
}

CEvent_BackStepAttack_Sound* CEvent_BackStepAttack_Sound::Create(CGameObject* _pGameObject)
{
	CEvent_BackStepAttack_Sound* pInstance = new CEvent_BackStepAttack_Sound;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_BackStepAttack_Sound");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_BackStepAttack_Sound::Free()
{
	super::Free();
}