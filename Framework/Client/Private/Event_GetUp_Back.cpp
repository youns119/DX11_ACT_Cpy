#include "pch.h"
#include "Event_GetUp_Back.h"

#include "GameInstance.h"

#include "Player.h"

CEvent_GetUp_Back::CEvent_GetUp_Back()
{
}

HRESULT CEvent_GetUp_Back::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_GetUp_Back::Execute()
{
	m_pPlayer->Get_PlayerStat()->bInvincible = false;

	return S_OK;
}

CEvent_GetUp_Back* CEvent_GetUp_Back::Create(CGameObject* _pGameObject)
{
	CEvent_GetUp_Back* pInstance = new CEvent_GetUp_Back;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_GetUp_Back");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_GetUp_Back::Free()
{
	super::Free();
}