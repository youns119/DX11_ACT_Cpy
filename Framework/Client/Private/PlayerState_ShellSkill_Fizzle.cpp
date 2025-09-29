#include "pch.h"
#include "PlayerState_ShellSkill_Fizzle.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "SpringArm_Player.h"

CPlayerState_ShellSkill_Fizzle::CPlayerState_ShellSkill_Fizzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_ShellSkill_Fizzle::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("ShellAbility-BuffSelfPowerful");

	return S_OK;
}

void CPlayerState_ShellSkill_Fizzle::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_SHELLSKILL_FIZZLE);

	// 기본 움직임 조작 불가능
	// 속도 조절 불가능
	// 콤보 카운트 초기화
	m_pPlayerStat->bMoveInputEnable = false;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pPlayer->Get_UpperStateMachine()->ChangeState(L"None");

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength() * 0.8f);
}

void CPlayerState_ShellSkill_Fizzle::Update(_float fTimeDelta)
{
	// 현재 애니메이션 블렌드 시간 진입하면 Idle로 상태 전환
	// 땅에 닿은 상태가 아니면 Airbone 상태로 전환
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->bInAir)
			m_pOwner->ChangeState(L"Airbone");
		else if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_ShellSkill_Fizzle::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_SHELLSKILL_FIZZLE);

	// 기본 움직임 조작 초기화
	// 속도 조절 변수 초기화
	// 상태 전환 변수 초기화
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pMovement->Set_ForceFriction(m_pPlayerStat->fForceFriction);
	m_pMovement->Set_RotateToDirection(true);
	m_pMovement->Reset_Velocity();

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength());
}

CPlayerState_ShellSkill_Fizzle* CPlayerState_ShellSkill_Fizzle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_ShellSkill_Fizzle* pInstance = new CPlayerState_ShellSkill_Fizzle(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_ShellSkill_Fizzle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ShellSkill_Fizzle::Free()
{
	super::Free();
}