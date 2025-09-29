#include "pch.h"
#include "PlayerState_ShellEnter.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_ShellEnter::CPlayerState_ShellEnter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_ShellEnter::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("EnterShell3");

	return S_OK;
}

void CPlayerState_ShellEnter::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_SHELLENTER);

	// 기본 움직임 조작 가능
	// 속도 조절 불가능
	// 콤보 카운트 초기화
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);

	m_pPlayer->Play_PlayerSound(L"Shell_Enter");
}

void CPlayerState_ShellEnter::Update(_float fTimeDelta)
{
	// 공중 상태일 경우 Airbone으로 상태 전환
	if (m_pPlayerStat->bInAir)
		m_pOwner->ChangeState(L"Airbone");

	// 본 애니메이션이 블렌드 가능 상태 진입하면 Idle로 상태 전환
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
		m_pOwner->ChangeState(L"ShellIdle");
}

void CPlayerState_ShellEnter::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_SHELLENTER);

	// 속도 조절 변수 초기화
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bCanHammerAnim = true;
}

CPlayerState_ShellEnter* CPlayerState_ShellEnter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_ShellEnter* pInstance = new CPlayerState_ShellEnter(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_ShellEnter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ShellEnter::Free()
{
	super::Free();
}