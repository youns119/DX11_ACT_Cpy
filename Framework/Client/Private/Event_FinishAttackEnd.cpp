#include "pch.h"
#include "Event_FinishAttackEnd.h"

#include "GameInstance.h"
#include "Movement.h"
#include "Collider.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"
#include "CineCamera_Player.h"

CEvent_FinishAttackEnd::CEvent_FinishAttackEnd()
{
}

HRESULT CEvent_FinishAttackEnd::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_FinishAttackEnd::Execute()
{
	_vec3 vLook = m_pTransform->Get_State(CTransform::STATE::LOOK) * -1.f;
	_vec3 vDir = vLook * 7.f + XMVectorSet(0.f, 1.f, 0.f, 0.f) * 13.f;

	m_pMovement->Activate(true);
	m_pMovement->Set_RotateToDirection(false);
	m_pMovement->Add_Force(vDir._vector(), 40.f);

	m_pPlayer->Get_PlayerCineCamera()->PositionShake(0.08f, 25.f, 0.8f);
	m_pPlayer->Get_PlayerCineCamera()->RotationShake(0.08f, 25.f, 0.8f);

	CGameInstance::GetInstance()->Call_Lag(0.01f, 0.5f);

	m_pPlayer->Play_PlayerSound(L"Hammer_ChargeAttack_Voice2");

	return S_OK;
}

CEvent_FinishAttackEnd* CEvent_FinishAttackEnd::Create(CGameObject* _pGameObject)
{
	CEvent_FinishAttackEnd* pInstance = new CEvent_FinishAttackEnd;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_FinishAttackEnd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_FinishAttackEnd::Free()
{
	super::Free();
}