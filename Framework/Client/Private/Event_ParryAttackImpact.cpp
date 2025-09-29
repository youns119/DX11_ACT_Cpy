#include "pch.h"
#include "Event_ParryAttackImpact.h"

#include "GameInstance.h"
#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"
#include "Movement.h"

CEvent_ParryAttackImpact::CEvent_ParryAttackImpact()
{
}

HRESULT CEvent_ParryAttackImpact::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_ParryAttackImpact::Execute()
{
	CGameInstance::GetInstance()->Call_Lag(0.1f, 0.8f);

	m_pPlayer->Play_PlayerSound(L"Parry_Attack");
	m_pPlayer->Play_PlayerSound(L"NormalAttack_Bubble");
	m_pPlayer->Play_PlayerSound(L"Hammer_ChargeAttack_Voice2");

	return S_OK;
}

CEvent_ParryAttackImpact* CEvent_ParryAttackImpact::Create(CGameObject* _pGameObject)
{
	CEvent_ParryAttackImpact* pInstance = new CEvent_ParryAttackImpact;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_ParryAttackImpact");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_ParryAttackImpact::Free()
{
	super::Free();
}