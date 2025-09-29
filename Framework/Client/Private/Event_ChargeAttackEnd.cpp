#include "pch.h"
#include "Event_ChargeAttackEnd.h"

#include "Player.h"
#include "Movement.h"

CEvent_ChargeAttackEnd::CEvent_ChargeAttackEnd()
{
}

HRESULT CEvent_ChargeAttackEnd::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_ChargeAttackEnd::Execute()
{
	m_pMovement->Reset_Velocity();

	m_pPlayer->Set_AutoLock(false);

	return S_OK;
}

CEvent_ChargeAttackEnd* CEvent_ChargeAttackEnd::Create(CGameObject* _pGameObject)
{
	CEvent_ChargeAttackEnd* pInstance = new CEvent_ChargeAttackEnd;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_ChargeAttackEnd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_ChargeAttackEnd::Free()
{
	super::Free();
}