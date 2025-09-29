#include "pch.h"
#include "PlayerState_NormalAttacked.h"

#include "GameInstance.h"
#include "Model.h"
#include "Movement.h"
#include "Animation.h"
#include "StateMachine.h"

#include "Player.h"
#include "Weapon_Player.h"

CPlayerState_NormalAttacked::CPlayerState_NormalAttacked(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_NormalAttacked::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("Hurt");

	return S_OK;
}

void CPlayerState_NormalAttacked::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_NORMALATTACKED);

	// 속도 조절 불가능
	// 콤보 카운트 초기화
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bInvincible = true;
	m_pPlayerStat->bCanHammerAnim = false;

	// 현재 속도 0.1배로 줄임
	m_pMovement->Set_HorizontalVelocity(_vec3(m_pMovement->Get_HorizontalVelocity())._vector() * 0.1f);
	m_pMovement->Set_MaxSpeed(m_pPlayerStat->fMaxSpeed * 0.1f);

	// 플레이어 무기 위치 변경
	m_pPlayer->Get_PlayerWeapon()->MoveToSocket(CWeapon_Player::SOCKET::SOCKET_CLAW);

	m_pPlayer->Play_PlayerSound(L"Impact");

	_uint iRand = rand() % 2;
	switch (iRand)
	{
	case 0 :
		m_pPlayer->Play_PlayerSound(L"NormalAttacked1");
		break;
	case 1:
		m_pPlayer->Play_PlayerSound(L"NormalAttacked2");
		break;
	default :
		break;
	}

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);
}

void CPlayerState_NormalAttacked::Update(_float fTimeDelta)
{
	// 현재 애니메이션 블렌드 시간 진입하면 Idle로 상태 전환
	// 땅에 닿은 상태가 아니면 Airbone 상태로 전환
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->fCurrHP <= 0.f)
		{
			m_pPlayerStat->bDeath = true;
			m_pOwner->ChangeState(L"NormalDeath");
		}
		else
		{
			if (m_pPlayerStat->bInAir)
				m_pOwner->ChangeState(L"Airbone");
			else if (m_pPlayerStat->bLockOn)
				m_pOwner->ChangeState(L"LockOn");
			else m_pOwner->ChangeState(L"Idle");
		}
	}
}

void CPlayerState_NormalAttacked::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_NORMALATTACKED);

	// 속도 조절 변수 초기화
	// 상태 전환 변수 초기화
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bInvincible = false;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pMovement->Set_MaxSpeed(m_pPlayerStat->fMaxSpeed);

	m_pMovement->Set_RotateToDirection(true);

	// 플레이어 무기 위치 초기화
	m_pPlayer->Get_PlayerWeapon()->MoveToSocket(CWeapon_Player::SOCKET::SOCKET_HAND);
}

CPlayerState_NormalAttacked* CPlayerState_NormalAttacked::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_NormalAttacked* pInstance = new CPlayerState_NormalAttacked(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_NormalAttacked");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_NormalAttacked::Free()
{
	super::Free();
}