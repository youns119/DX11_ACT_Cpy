#include "pch.h"
#include "Event_HammerDashAttackStart.h"

#include "GameInstance.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"
#include "Transform.h"
#include "Movement.h"

CEvent_HammerDashAttackStart::CEvent_HammerDashAttackStart()
{
}

HRESULT CEvent_HammerDashAttackStart::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_HammerDashAttackStart::Execute()
{
	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

	m_pPlayer->Player_CameraShake_Rotation(0.02f, 5.f, 0.2f);
	m_pMovement->Add_Force(vLook, 10.f);

	m_pPlayer->Play_PlayerSound(L"Hammer_DashAttack");
	m_pPlayer->Play_PlayerSound(L"ChargeAttack_Bubble");
	m_pPlayer->Play_PlayerSound(L"Hammer_DashAttack_Voice");

	return S_OK;
}

CEvent_HammerDashAttackStart* CEvent_HammerDashAttackStart::Create(CGameObject* _pGameObject)
{
	CEvent_HammerDashAttackStart* pInstance = new CEvent_HammerDashAttackStart;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_HammerDashAttackStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_HammerDashAttackStart::Free()
{
	super::Free();
}