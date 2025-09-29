#include "pch.h"
#include "Event_Skill_RollOut_BackFlip.h"

#include "GameInstance.h"
#include "Movement.h"

#include "Player.h"
#include "CineCamera_Player.h"

CEvent_Skill_RollOut_BackFlip::CEvent_Skill_RollOut_BackFlip()
{
}

HRESULT CEvent_Skill_RollOut_BackFlip::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	m_pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	return S_OK;
}

HRESULT CEvent_Skill_RollOut_BackFlip::Execute()
{
	m_pMovement->FlyMode(false);
	m_pMovement->Reset_Velocity();
	m_pMovement->Set_ForceFriction(m_pPlayer->Get_PlayerStat()->fForceFriction);

	_vec3 vLook = m_pTransform->Get_State(CTransform::STATE::LOOK) * -1.f;
	_vec3 vDir = vLook * 7.f + XMVectorSet(0.f, 1.f, 0.f, 0.f) * 13.f;

	m_pMovement->Set_RotateToDirection(false);
	m_pMovement->Add_Force(vDir._vector(), 35.f);

	m_pPlayer->Play_PlayerSound(L"NormalAttack_Bubble");

	return S_OK;
}

CEvent_Skill_RollOut_BackFlip* CEvent_Skill_RollOut_BackFlip::Create(CGameObject* _pGameObject)
{
	CEvent_Skill_RollOut_BackFlip* pInstance = new CEvent_Skill_RollOut_BackFlip;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_Skill_RollOut_BackFlip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Skill_RollOut_BackFlip::Free()
{
	super::Free();
}