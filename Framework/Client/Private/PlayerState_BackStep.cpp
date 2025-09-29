#include "pch.h"
#include "PlayerState_BackStep.h"

#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_BackStep::CPlayerState_BackStep(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_BackStep::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("DodgeStepback");

	return S_OK;
}

void CPlayerState_BackStep::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_BACKSTEP);

	// 기본 움직임 조작 불가능
	// 속도 조절 불가능
	// 콤보 카운트 초기화
	m_pPlayerStat->bMoveInputEnable = false;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;
	m_pPlayerStat->bSwimmable = false;
	m_pPlayerStat->bBackStep = true;

	m_pMovement->Set_RotateToDirection(false);
	m_pMovement->Set_MaxFallingSpeed(m_pPlayerStat->fMaxFallingSpeed);

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);
}

void CPlayerState_BackStep::Update(_float fTimeDelta)
{
	// 현재 애니메이션 블렌드 시간 진입하면 
	// 공중일 때 Airbone으로 상태 전환
	// 공중 아닐 때 Idle으로 상태 전환
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->bInAir)
			m_pOwner->ChangeState(L"Airbone");
		else if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_BackStep::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_BACKSTEP);

	// 기본 움직임 조작 초기화
	// 속도 조절 변수 초기화
	// Evade 관련 변수 초기화
	// Invincible 상태 초기화
	// bTransition 변수 false로 초기화
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bBackStep = false;
	m_pPlayerStat->bInvincible = false;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->bCanHammerAnim = true;
	m_pPlayerStat->bCanAirAction = false;
	m_pPlayerStat->bCanBackStepAttack = false;

	m_pMovement->Set_GravityScale(m_pPlayerStat->fGravityScale);
	m_pMovement->Set_InAirControlRatio(m_pPlayerStat->fInAirControlRatio);
	m_pMovement->Set_RotateToDirection(true);
	m_pMovement->Set_ForceFriction(m_pPlayerStat->fForceFriction);
}

CPlayerState_BackStep* CPlayerState_BackStep::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_BackStep* pInstance = new CPlayerState_BackStep(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_BackStep");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_BackStep::Free()
{
	super::Free();
}