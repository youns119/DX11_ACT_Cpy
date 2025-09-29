#include "pch.h"
#include "PlayerState_ShellSkill_RollOut_BackFlip.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "CineCamera_Player.h"
#include "Enemy.h"

CPlayerState_ShellSkill_RollOut_BackFlip::CPlayerState_ShellSkill_RollOut_BackFlip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_ShellSkill_RollOut_BackFlip::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("Backflip_RollOut");

	m_pPlayer->Play_PlayerSound(L"DodgeMedium");

	return S_OK;
}

void CPlayerState_ShellSkill_RollOut_BackFlip::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_SHELLSKILL_ROLLOUT_BACKFLIP);

	// 입력 불가능
	// 콤보 카운트 초기화
	m_pPlayerStat->bInputEnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;
	m_pPlayerStat->bInvincible = true;

	m_pMovement->FlyMode(false);
	m_pMovement->Reset_Velocity();
	m_pMovement->Set_ForceFriction(m_pPlayerStat->fForceFriction);

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.f);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength());
}

void CPlayerState_ShellSkill_RollOut_BackFlip::Update(_float fTimeDelta)
{
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->bInAir)
			m_pOwner->ChangeState(L"Airbone");
		else if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_ShellSkill_RollOut_BackFlip::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_SHELLSKILL_ROLLOUT_BACKFLIP);

	// 입력 가능
	// 갈고리 액션 비활성화
	m_pPlayerStat->bInputEnable = true;
	m_pPlayerStat->bGrappling = false;
	m_pPlayerStat->bCanHammerAnim = true;
	m_pPlayerStat->bInvincible = false;

	m_pMovement->Set_RotateToDirection(true);
}

CPlayerState_ShellSkill_RollOut_BackFlip* CPlayerState_ShellSkill_RollOut_BackFlip::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_ShellSkill_RollOut_BackFlip* pInstance = new CPlayerState_ShellSkill_RollOut_BackFlip(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_ShellSkill_RollOut_BackFlip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ShellSkill_RollOut_BackFlip::Free()
{
	super::Free();
}