#include "pch.h"
#include "PlayerState_MantisPunch.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "Weapon_Player.h"
#include "SpringArm_Player.h"
#include "CineCamera_Player.h"

CPlayerState_MantisPunch::CPlayerState_MantisPunch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_MantisPunch::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex[static_cast<_uint>(MANTISPUNCH::MANTISPUNCH_WINDUP)] = m_pModel->Get_AnimationIndex("UmamiAbility_MantisPunchWindup");
	m_iAnimIndex[static_cast<_uint>(MANTISPUNCH::MANTISPUNCH_TRAVEL)] = m_pModel->Get_AnimationIndex("UmamiAbility_MantisPunchTravel");
	m_iAnimIndex[static_cast<_uint>(MANTISPUNCH::MANTISPUNCH_PUNCH)] = m_pModel->Get_AnimationIndex("UmamiAbility_MantisPunchSwing");

	return S_OK;
}

void CPlayerState_MantisPunch::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_MANTISPUNCH);

	// 기본 움직임 조작 불가능
	// 속도 조절 불가능
	// CanParryAttack 변수 초기화
	m_pPlayerStat->bMoveInputEnable = false;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->bCanHammerAnim = false;
	m_pPlayerStat->bInvincible = true;

	m_pPlayerStat->fDamageFactor = 10.f;
	m_pPlayerStat->eDamageType = IDamaged::DAMAGE::DAMAGE_HEAVY;

	m_eMantisPunch = MANTISPUNCH::MANTISPUNCH_WINDUP;

	m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eMantisPunch)], false, 0.1f);

	m_pPlayer->Get_PlayerWeapon()->MoveToSocket(CWeapon_Player::SOCKET::SOCKET_CLAW_END);

	m_pPlayer->Get_PlayerCineCamera()->Play_Cinematic(L"MantisPunch");

	m_pPlayer->Play_PlayerSound(L"MantisPunch_Charge");
	m_pPlayer->Play_PlayerSound(L"MantisPunch_Voice");
}

void CPlayerState_MantisPunch::Update(_float fTimeDelta)
{
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_eMantisPunch == MANTISPUNCH::MANTISPUNCH_WINDUP)
		{
			m_eMantisPunch = MANTISPUNCH::MANTISPUNCH_TRAVEL;
			m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eMantisPunch)], false, 0.1f);

			m_pPlayer->Play_PlayerSound(L"MantisPunch_Teleport");
		}
		else if (m_eMantisPunch == MANTISPUNCH::MANTISPUNCH_TRAVEL)
		{
			m_eMantisPunch = MANTISPUNCH::MANTISPUNCH_PUNCH;
			m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eMantisPunch)], false, 0.1f);
		}
		else
			m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_MantisPunch::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_MANTISPUNCH);

	// 플레이어 Hand Collider 초기화
	m_pPlayer->Player_HandCollider(false);

	// 기본 움직임 조작 초기화
	// 속도 조절 변수 초기화
	// bTransition 변수 false로 초기화
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bCanHammerAnim = true;
	m_pPlayerStat->bInvincible = false;

	m_pMovement->Set_RotateToDirection(true);
	m_pMovement->Reset_Horizontal();

	m_pPlayer->Get_PlayerWeapon()->MoveToSocket(CWeapon_Player::SOCKET::SOCKET_HAND);

	if (m_pPlayer->Get_PlayerCineCamera()->IsCurrCam())
		m_pPlayer->Get_PlayerCineCamera()->Stop_Cinematic();
}

CPlayerState_MantisPunch* CPlayerState_MantisPunch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_MantisPunch* pInstance = new CPlayerState_MantisPunch(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_MantisPunch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_MantisPunch::Free()
{
	super::Free();
}