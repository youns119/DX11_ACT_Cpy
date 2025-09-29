#include "pch.h"
#include "PlayerState_Walk.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_Walk::CPlayerState_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Walk::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("Walk");
	m_fRunMaxSpeedThreshold = m_pPlayerStat->fMaxSpeed * m_pPlayerStat->fRunMaxSpeedRatio * m_pPlayerStat->fRunThresholdRatio;

	return S_OK;
}

void CPlayerState_Walk::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_WALK);

	// �޺� ī��Ʈ �ʱ�ȭ
	m_pPlayerStat->iComboCount = 0;

	m_pModel->SetUp_Animation(m_iAnimIndex, true, 0.2f);
}

void CPlayerState_Walk::Update(_float fTimeDelta)
{
	_float fSpeed = m_pMovement->Get_HorizontalSpeed();

	// ���� ���� ������ ���� ���·� ��ȯ
	if (m_pPlayerStat->bInAir)
	{
		m_pOwner->ChangeState(L"Airbone");
		return;
	}

	// �ӵ� 0�̸� Idle ���·� ��ȯ
	// �ӵ� 0�� �ƴϰ� bRunning�� true��� Run���� ���� ��ȯ
	if (fSpeed == 0.f)
	{
		if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");

	}
	else
	{
		_float fMaxSpeed = m_pMovement->Get_MaxSpeed();

		if(fMaxSpeed >= m_fRunMaxSpeedThreshold)
			m_pOwner->ChangeState(L"Run");
	}

	m_fFootStepSoundAcc += fTimeDelta;
	if (m_fFootStepSoundAcc >= m_fFootStepSoundDist)
	{
		m_fFootStepSoundAcc = 0.f;
		m_pPlayer->Play_PlayerSound(L"FootStep");
	}
}

void CPlayerState_Walk::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_WALK);

	m_fFootStepSoundAcc = 0.f;
}

CPlayerState_Walk* CPlayerState_Walk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Walk* pInstance = new CPlayerState_Walk(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Walk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Walk::Free()
{
	super::Free();
}