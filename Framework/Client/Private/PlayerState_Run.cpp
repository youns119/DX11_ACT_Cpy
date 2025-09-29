#include "pch.h"
#include "PlayerState_Run.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_Run::CPlayerState_Run(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Run::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("Run");
	m_fRunMaxSpeedThreshold = m_pPlayerStat->fMaxSpeed * m_pPlayerStat->fRunMaxSpeedRatio * m_pPlayerStat->fRunThresholdRatio;

	return S_OK;
}

void CPlayerState_Run::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_RUN);

	// 콤보 카운트 초기화
	// bRunning 상태 true
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bRunning = true;

	m_pModel->SetUp_Animation(m_iAnimIndex, true, 0.15f);
}

void CPlayerState_Run::Update(_float fTimeDelta)
{
	_float fSpeed = m_pMovement->Get_HorizontalSpeed();

	// 땅에 닿지 않으면 낙하 상태로 전환
	if (m_pPlayerStat->bInAir)
	{
		m_pOwner->ChangeState(L"Airbone");
		return;
	}

	// 속도 0이면 Idle 상태로 전환
	// 속도 0이 아니고 bRunning이 false라면 Walk로 상태 전환
	if (fSpeed == 0.f)
	{
		if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
	else
	{
		_float fMaxSpeed = m_pMovement->Get_MaxSpeed();
		if (fMaxSpeed < m_fRunMaxSpeedThreshold)
		{
			if (m_pPlayerStat->bLockOn)
				m_pOwner->ChangeState(L"LockOn");
			else m_pOwner->ChangeState(L"Walk");
		}
	}

	m_fFootStepSoundAcc += fTimeDelta;
	if (m_fFootStepSoundAcc >= m_fFootStepSoundDist)
	{
		m_fFootStepSoundAcc = 0.f;
		m_pPlayer->Play_PlayerSound(L"FootStep");
	}
}

void CPlayerState_Run::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_RUN);

	// bRunning 상태 false
	m_pPlayerStat->bRunning = false;

	m_fFootStepSoundAcc = 0.f;
}

CPlayerState_Run* CPlayerState_Run::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Run* pInstance = new CPlayerState_Run(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Run");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Run::Free()
{
	super::Free();
}