#include "pch.h"
#include "PlayerState_LockOn.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_LockOn::CPlayerState_LockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_LockOn::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_eLockOn = LOCKON::LOCKON_IDLE;

	m_iAnimIndex[static_cast<_uint>(LOCKON::LOCKON_IDLE)] = m_pModel->Get_AnimationIndex("CrabIdle");
	m_iAnimIndex[static_cast<_uint>(LOCKON::LOCKON_WALK_F)] = m_pModel->Get_AnimationIndex("CrabWalkForward");
	m_iAnimIndex[static_cast<_uint>(LOCKON::LOCKON_WALK_B)] = m_pModel->Get_AnimationIndex("CrabWalkBackward");
	m_iAnimIndex[static_cast<_uint>(LOCKON::LOCKON_WALK_L)] = m_pModel->Get_AnimationIndex("CrabWalkLeft");
	m_iAnimIndex[static_cast<_uint>(LOCKON::LOCKON_WALK_R)] = m_pModel->Get_AnimationIndex("CrabWalkRight");

	m_fRunMaxSpeedThreshold = m_pPlayerStat->fMaxSpeed * m_pPlayerStat->fRunMaxSpeedRatio * m_pPlayerStat->fRunThresholdRatio;

	return S_OK;
}

void CPlayerState_LockOn::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_LOCKON);

	// 콤보 카운트 초기화
	m_pPlayerStat->iComboCount = 0;

	m_pLockOnTransform = m_pPlayer->Get_LockOnTransform();

	// 처음은 무조건 Idle 모션
	m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(LOCKON::LOCKON_IDLE)], true, 0.1f);

	m_pMovement->Set_RotateToDirection(false);
	m_pMovement->Set_MaxSpeed(m_pPlayerStat->fMaxSpeed);
}

void CPlayerState_LockOn::Update(_float fTimeDelta)
{
	// 땅에 닿지 않으면 낙하 상태로 전환
	if (m_pPlayerStat->bInAir)
	{
		m_pOwner->ChangeState(L"Airbone");
		return;
	}

	_float fSpeed = m_pMovement->Get_HorizontalSpeed();

	// 일정 속도 이상이면 Run으로 상태 전환
	_float fMaxSpeed = m_pMovement->Get_MaxSpeed();
	if (fMaxSpeed >= m_fRunMaxSpeedThreshold)
	{
		m_pOwner->ChangeState(L"Run");
		return;
	}

	if (fSpeed == 0.f)
	{
		if (m_eLockOn != LOCKON::LOCKON_IDLE)
		{
			m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(LOCKON::LOCKON_IDLE)], true, 0.1f);
			m_eLockOn = LOCKON::LOCKON_IDLE;
		}
	}
	else
	{
		_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
		_float3 vVelocity = m_pMovement->Get_HorizontalVelocity();
		_vector vVel = XMLoadFloat3(&vVelocity);

		_float fDegree = CUtils::GetBetweenAngle(vLook, vVel);

		if ((fDegree > -45.f && fDegree < 45.f) && m_eLockOn != LOCKON::LOCKON_WALK_F)
		{
			m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(LOCKON::LOCKON_WALK_F)], true, 0.1f);
			m_eLockOn = LOCKON::LOCKON_WALK_F;
		}
		else if ((fDegree < -135.f || fDegree > 135.f) && m_eLockOn != LOCKON::LOCKON_WALK_B)
		{
			m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(LOCKON::LOCKON_WALK_B)], true, 0.1f);
			m_eLockOn = LOCKON::LOCKON_WALK_B;
		}
		else if ((fDegree >= -135.f && fDegree <= -45.f) && m_eLockOn != LOCKON::LOCKON_WALK_L)
		{
			m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(LOCKON::LOCKON_WALK_L)], true, 0.1f);
			m_eLockOn = LOCKON::LOCKON_WALK_L;
		}
		else if ((fDegree >= 45.f && fDegree <= 135.f) && m_eLockOn != LOCKON::LOCKON_WALK_R)
		{
			m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(LOCKON::LOCKON_WALK_R)], true, 0.1f);
			m_eLockOn = LOCKON::LOCKON_WALK_R;
		}
	}

	_vector vLockOnPos = m_pLockOnTransform->Get_State(CTransform::STATE::POSITION);
	m_pMovement->TurnToDirection(vLockOnPos, fTimeDelta);
}

void CPlayerState_LockOn::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_LOCKON);

	m_pMovement->Set_RotateToDirection(true);

	// LockOn 상태 초기화
	m_eLockOn = LOCKON::LOCKON_IDLE;
}

CPlayerState_LockOn* CPlayerState_LockOn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_LockOn* pInstance = new CPlayerState_LockOn(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_LockOn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_LockOn::Free()
{
	super::Free();
}