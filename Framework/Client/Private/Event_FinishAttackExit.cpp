#include "pch.h"
#include "Event_FinishAttackExit.h"

#include "GameInstance.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"
#include "CineCamera_Player.h"

CEvent_FinishAttackExit::CEvent_FinishAttackExit()
{
}

HRESULT CEvent_FinishAttackExit::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_FinishAttackExit::Execute()
{
	CGameInstance::GetInstance()->Call_Lag(0.1f, 1.3f);

	m_pPlayer->Get_PlayerCineCamera()->Resume();

	m_pPlayer->Play_PlayerSound(L"Parry_Attack");
	m_pPlayer->Play_PlayerSound(L"NormalAttack_Bubble");

	return S_OK;
}

CEvent_FinishAttackExit* CEvent_FinishAttackExit::Create(CGameObject* _pGameObject)
{
	CEvent_FinishAttackExit* pInstance = new CEvent_FinishAttackExit;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_FinishAttackExit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_FinishAttackExit::Free()
{
	super::Free();
}