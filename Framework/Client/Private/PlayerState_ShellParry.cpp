#include "pch.h"
#include "PlayerState_ShellParry.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_ShellParry::CPlayerState_ShellParry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_ShellParry::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("Unblock3");

	return S_OK;
}

void CPlayerState_ShellParry::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_SHELLPARRY);

	// 기본 움직임 조작 가능
	// 속도 조절 불가능
	// 콤보 카운트 초기화
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);

	m_pPlayer->Play_PlayerSound(L"Parry");
}

void CPlayerState_ShellParry::Update(_float fTimeDelta)
{
	// 본 애니메이션이 블렌드 가능 상태 진입하면 
	// 공중 상태일 경우 Airbone으로 상태 전환
	// 공중이 아닐 경우 Idle로 상태 전환
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->bInAir)
			m_pOwner->ChangeState(L"Airbone");
		else if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_ShellParry::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_SHELLPARRY);

	// 속도 조절 변수 초기화
	// Parry 상태 초기화
	// Invincible 상태 초기화
	// CanParryAttack 상태 초기화
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bParry = false;
	m_pPlayerStat->bInvincible = false;
	m_pPlayerStat->bCanParryAttack = false;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pMovement->Set_RotateToDirection(true);
}

CPlayerState_ShellParry* CPlayerState_ShellParry::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_ShellParry* pInstance = new CPlayerState_ShellParry(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_ShellParry");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ShellParry::Free()
{
	super::Free();
}