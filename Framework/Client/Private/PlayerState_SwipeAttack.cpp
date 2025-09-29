#include "pch.h"
#include "PlayerState_SwipeAttack.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "SpringArm_Player.h"

CPlayerState_SwipeAttack::CPlayerState_SwipeAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_SwipeAttack::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex[static_cast<_uint>(SWIPEATTACK::SWIPEATTACK_1)] = m_pModel->Get_AnimationIndex("AirAttack");
	m_iAnimIndex[static_cast<_uint>(SWIPEATTACK::HAMMERSWIPEATTACK_1)] = m_pModel->Get_AnimationIndex("HammerAirAttack");

	return S_OK;
}

void CPlayerState_SwipeAttack::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_SWIPEATTACK);

	// 기본 움직임 조작 불가능
	// 속도 조절 불가능
	m_pPlayerStat->bMoveInputEnable = false;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->bCanHammerAnim = false;
	m_pPlayerStat->bCharging = false;
	m_pPlayerStat->iComboCount++;
	m_pPlayerStat->eDamageType = IDamaged::DAMAGE::DAMAGE_NORMAL;
	m_pPlayerStat->bSwimmable = false;

	if (!m_pPlayerStat->bHasHammer)
	{
		m_pPlayerStat->fDamageFactor = 1.5f;

		m_eSwipeAttack = SWIPEATTACK::SWIPEATTACK_1;
		m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eSwipeAttack)], false, 0.1f);
	}
	else
	{
		m_pPlayerStat->bSuperArmor = true;
		m_pPlayerStat->fDamageFactor = 2.5f;

		m_eSwipeAttack = SWIPEATTACK::HAMMERSWIPEATTACK_1;
		m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eSwipeAttack)], false, 0.1f);
	}

	m_pPlayer->Get_UpperStateMachine()->ChangeState(L"None");

	// 플레이어 AutoLock 활성화
	m_pPlayer->Set_AutoLock(true);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength() * 0.9f);
}

void CPlayerState_SwipeAttack::Update(_float fTimeDelta)
{
	// 블렌드 가능 상태 진입하면
	// 공중 상태일 경우 Airbone 상태로 전환
	// 지면에 닿았을 경우 Idle로 상태 전환
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
		m_pOwner->ChangeState(L"Airbone");
}

void CPlayerState_SwipeAttack::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_SWIPEATTACK);

	// 플레이어 Weapon Collider 초기화
	m_pPlayer->Player_WeaponCollider(false);

	// 기본 움직임 조작 초기화
	// 속도 조절 변수 초기화
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->fAnimSpeed = 1.f;
	m_pPlayerStat->bCanHammerAnim = true;
	m_pPlayerStat->bSuperArmor = false;
	m_pPlayerStat->bCanAirAction = false;

	m_pMovement->Set_Decel(m_pPlayerStat->fDecel);

	// 플레이어 AutoLock 비활성화
	m_pPlayer->Set_AutoLock(false);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength());
}

CPlayerState_SwipeAttack* CPlayerState_SwipeAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_SwipeAttack* pInstance = new CPlayerState_SwipeAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_SwipeAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_SwipeAttack::Free()
{
	super::Free();
}