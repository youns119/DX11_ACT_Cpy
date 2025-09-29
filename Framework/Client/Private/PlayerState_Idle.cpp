#include "pch.h"
#include "PlayerState_Idle.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_Idle::CPlayerState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Idle::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("Idle");
	m_fRunMaxSpeedThreshold = m_pPlayerStat->fMaxSpeed * m_pPlayerStat->fRunMaxSpeedRatio * m_pPlayerStat->fRunThresholdRatio;

	return S_OK;
}

void CPlayerState_Idle::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_IDLE);

	// 콤보 카운트 초기화
	m_pPlayerStat->iComboCount = 0;

	if (m_pPlayerStat->bFeared)
	{
		if (m_pPlayerStat->bHasShell)
			m_pOwner->ChangeState(L"ShellEnter");
		else
		{
			if(!m_pPlayerState->test(PLAYERSTATE_GUARD))
				m_pPlayer->Get_UpperStateMachine()->ChangeState(L"Guard");
		}
	}

	_float fSpeed = m_pMovement->Get_VerticalSpeed();
	if (fSpeed > 0.f)
		m_pOwner->ChangeState(L"Walk");
	else m_pModel->SetUp_Animation(m_iAnimIndex, true, 0.2f);
}

void CPlayerState_Idle::Update(_float fTimeDelta)
{
	_float fSpeed = m_pMovement->Get_HorizontalSpeed();

	// 땅에 닿지 않으면 낙하 상태로 전환
	if (m_pPlayerStat->bInAir)
	{
		m_pOwner->ChangeState(L"Airbone");
		return;
	}

	// 속도 0이 아니면 bRunning의 상태에 따라 Walk / Run 으로 상태 전환
	if (fSpeed != 0.f)
	{
		_float fMaxSpeed = m_pMovement->Get_MaxSpeed();

		if (fMaxSpeed >= m_fRunMaxSpeedThreshold)
			m_pOwner->ChangeState(L"Run");
		else if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Walk");
	}
}

void CPlayerState_Idle::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_IDLE);
}

CPlayerState_Idle* CPlayerState_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* m_PlayerState)
{
	CPlayerState_Idle* pInstance = new CPlayerState_Idle(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, m_PlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Idle::Free()
{
	super::Free();
}