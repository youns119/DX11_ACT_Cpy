#include "pch.h"
#include "Event_GetUp_Front.h"

#include "GameInstance.h"

#include "Player.h"

CEvent_GetUp_Front::CEvent_GetUp_Front()
{
}

HRESULT CEvent_GetUp_Front::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_GetUp_Front::Execute()
{
	m_pPlayer->Get_PlayerStat()->bInvincible = false;

	return S_OK;
}

CEvent_GetUp_Front* CEvent_GetUp_Front::Create(CGameObject* _pGameObject)
{
	CEvent_GetUp_Front* pInstance = new CEvent_GetUp_Front;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_GetUp_Front");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_GetUp_Front::Free()
{
	super::Free();
}