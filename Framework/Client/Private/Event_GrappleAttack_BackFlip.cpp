#include "pch.h"
#include "Event_GrappleAttack_BackFlip.h"

#include "GameInstance.h"
#include "Movement.h"

#include "Player.h"
#include "CineCamera_Player.h"

CEvent_GrappleAttack_BackFlip::CEvent_GrappleAttack_BackFlip()
{
}

HRESULT CEvent_GrappleAttack_BackFlip::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	m_pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	return S_OK;
}

HRESULT CEvent_GrappleAttack_BackFlip::Execute()
{
	m_pMovement->FlyMode(false);
	m_pMovement->Reset_Velocity();
	m_pMovement->Set_ForceFriction(m_pPlayer->Get_PlayerStat()->fForceFriction);

	_vec3 vLook = m_pTransform->Get_State(CTransform::STATE::LOOK) * -1.f;
	_vec3 vDir = vLook * 7.f + XMVectorSet(0.f, 1.f, 0.f, 0.f) * 13.f;

	m_pMovement->Set_RotateToDirection(false);
	m_pMovement->Add_Force(vDir._vector(), 35.f);

	m_pPlayer->Play_PlayerSound(L"Parry_Attack");
	m_pPlayer->Play_PlayerSound(L"NormalAttack_Bubble");

	m_pPlayer->Get_PlayerCineCamera()->Resume();

	return S_OK;
}

CEvent_GrappleAttack_BackFlip* CEvent_GrappleAttack_BackFlip::Create(CGameObject* _pGameObject)
{
	CEvent_GrappleAttack_BackFlip* pInstance = new CEvent_GrappleAttack_BackFlip;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_GrappleAttack_BackFlip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_GrappleAttack_BackFlip::Free()
{
	super::Free();
}