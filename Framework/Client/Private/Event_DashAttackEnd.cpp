#include "pch.h"
#include "Event_DashAttackEnd.h"

#include "GameInstance.h"
#include "Movement.h"

#include "Player.h"

CEvent_DashAttackEnd::CEvent_DashAttackEnd()
{
}

HRESULT CEvent_DashAttackEnd::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_DashAttackEnd::Execute()
{
	_float fSpeed = m_pMovement->Get_HorizontalSpeed();
	_float3 vVelocity = m_pMovement->Get_HorizontalVelocity();

	m_pMovement->Set_ForceFriction(m_pPlayer->Get_PlayerStat()->fForceFriction * 20.f);
	m_pMovement->Set_Decel(m_pPlayer->Get_PlayerStat()->fDecel * 0.5f);

	m_pPlayer->Get_PlayerStat()->bDashStart = false;
	m_pPlayer->Set_AutoLock(false);

	return S_OK;
}

CEvent_DashAttackEnd* CEvent_DashAttackEnd::Create(CGameObject* _pGameObject)
{
	CEvent_DashAttackEnd* pInstance = new CEvent_DashAttackEnd;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_DashAttackEnd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_DashAttackEnd::Free()
{
	super::Free();
}