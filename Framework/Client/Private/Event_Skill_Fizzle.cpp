#include "pch.h"
#include "Event_Skill_Fizzle.h"

#include "GameInstance.h"
#include "Movement.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"
#include "Shell_Part.h"

CEvent_Skill_Fizzle::CEvent_Skill_Fizzle()
{
}

HRESULT CEvent_Skill_Fizzle::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);
	m_pPlayerCamera = static_cast<CCamera_Player*>(m_pPlayer->Get_PlayerSpringArm()->Get_PlayerCamera());

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	return S_OK;
}

HRESULT CEvent_Skill_Fizzle::Execute()
{
	CShell_Part* pShell = m_pPlayer->Get_PlayerShell();
	pShell->Use_Skill();

	m_pMovement->Set_RotateToDirection(false);

	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	m_pMovement->Set_ForceFriction(m_pPlayerStat->fForceFriction * 2.f);
	m_pMovement->Add_Force(vLook * -1.f, 15.f);

	CGameInstance::GetInstance()->Call_Lag(0.1f, 0.1f);

	m_pPlayerCamera->PositionShake(0.08f, 20.f, 0.5f);
	m_pPlayerCamera->RotationShake(0.05f, 20.f, 0.5f);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength() * 1.5f);

	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vUp = m_pTransform->Get_State(CTransform::STATE::UP);
	vLook = _mat(m_pPlayerCamera->Get_CombinedMatrix()).look._vector();

	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FIZZLE_FLASH, vPos, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FIZZLE_SANDBURST, vPos, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vUp - vLook * 4.f, -vLook, false, nullptr);

	m_pPlayer->Play_PlayerSound(L"Shell_Fizzle");

	return S_OK;
}

CEvent_Skill_Fizzle* CEvent_Skill_Fizzle::Create(CGameObject* _pGameObject)
{
	CEvent_Skill_Fizzle* pInstance = new CEvent_Skill_Fizzle;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_Skill_Fizzle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Skill_Fizzle::Free()
{
	super::Free();
}