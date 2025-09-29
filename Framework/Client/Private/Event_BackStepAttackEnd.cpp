#include "pch.h"
#include "Event_BackStepAttackEnd.h"

#include "GameInstance.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"
#include "CineCamera_Player.h"
#include "Movement.h"

CEvent_BackStepAttackEnd::CEvent_BackStepAttackEnd()
{
}

HRESULT CEvent_BackStepAttackEnd::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_BackStepAttackEnd::Execute()
{
	CGameInstance::GetInstance()->Call_Lag(0.05f, 1.f);

	m_pMovement->Reset_Velocity();

	m_pPlayer->Get_PlayerCineCamera()->RotationShake(0.05f, 30.f, 0.8f);

	m_pPlayer->Get_PlayerStat()->bDashStart = false;
	m_pPlayer->Get_PlayerStat()->bInvincible = false;

	m_pPlayer->Play_PlayerSound(L"Parry_Attack");
	m_pPlayer->Play_PlayerSound(L"NormalAttack_Bubble");
	m_pPlayer->Play_PlayerSound(L"BackStepAttack_Voice2");

	return S_OK;
}

CEvent_BackStepAttackEnd* CEvent_BackStepAttackEnd::Create(CGameObject* _pGameObject)
{
	CEvent_BackStepAttackEnd* pInstance = new CEvent_BackStepAttackEnd;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_BackStepAttackEnd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_BackStepAttackEnd::Free()
{
	super::Free();
}