#include "pch.h"
#include "PlayerState_PlungeAttack.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "SpringArm_Player.h"

CPlayerState_PlungeAttack::CPlayerState_PlungeAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_PlungeAttack::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex[static_cast<_uint>(PLUNGEATTACK::PLUNGEATTACK_OPEN)] = m_pModel->Get_AnimationIndex("PlungeOpen");
	m_iAnimIndex[static_cast<_uint>(PLUNGEATTACK::PLUNGEATTACK_TRAVEL)] = m_pModel->Get_AnimationIndex("PlungeTravel");
	m_iAnimIndex[static_cast<_uint>(PLUNGEATTACK::PLUNGEATTACK_LAND)] = m_pModel->Get_AnimationIndex("PlungeLand");

	return S_OK;
}

void CPlayerState_PlungeAttack::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_PLUNGEATTACK);

	// 기본 움직임 조작 불가능
	// 속도 조절 불가능
	m_pPlayerStat->bMoveInputEnable = false;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->bCanHammerAnim = false;
	m_pPlayerStat->bCharging = false;
	m_pPlayerStat->eDamageType = IDamaged::DAMAGE::DAMAGE_HEAVY;

	m_pPlayerStat->fDamageFactor = 2.5f;

	m_ePlungeAttack = PLUNGEATTACK::PLUNGEATTACK_OPEN;

	m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_ePlungeAttack)], false, 0.1f);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength() * 0.85f);
}

void CPlayerState_PlungeAttack::Update(_float fTimeDelta)
{
	// 블렌드 가능 상태 진입하면
	// 공중 상태일 경우 Airbone 상태로 전환
	// 지면에 닿았을 경우 Idle로 상태 전환
	if (!m_pPlayerStat->bInAir && m_ePlungeAttack != PLUNGEATTACK::PLUNGEATTACK_LAND)
	{
		m_ePlungeAttack = PLUNGEATTACK::PLUNGEATTACK_LAND;
		m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_ePlungeAttack)], false, 0.1f);
	}
	else if(m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_ePlungeAttack == PLUNGEATTACK::PLUNGEATTACK_OPEN)
		{
			m_ePlungeAttack = PLUNGEATTACK::PLUNGEATTACK_TRAVEL;
			m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_ePlungeAttack)], false, 0.f);

			m_pMovement->Set_MaxFallingSpeed(m_pPlayerStat->fMaxFallingSpeed);
			m_pMovement->Reset_Velocity();

			// 플레이어 AutoLock 비활성화
			m_pPlayer->Set_AutoLock(false);
		}
		else if (m_ePlungeAttack == PLUNGEATTACK::PLUNGEATTACK_LAND)
		{
			if (m_pPlayerStat->bLockOn)
				m_pOwner->ChangeState(L"LockOn");
			else m_pOwner->ChangeState(L"Idle");
		}
	}
}

void CPlayerState_PlungeAttack::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_PLUNGEATTACK);

	// 플레이어 Weapon Collider 초기화
	m_pPlayer->Player_WeaponCollider(false);

	// 기본 움직임 조작 초기화
	// 속도 조절 변수 초기화
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bCanHammerAnim = true;
	m_pPlayerStat->bTransition = false;

	m_pMovement->Activate(true);
	m_pMovement->Reset_Velocity();
	m_pMovement->Set_MaxFallingSpeed(m_pPlayerStat->fMaxFallingSpeed);

	m_pPlayer->Set_AutoLock(false);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength());
}

CPlayerState_PlungeAttack* CPlayerState_PlungeAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_PlungeAttack* pInstance = new CPlayerState_PlungeAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_PlungeAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_PlungeAttack::Free()
{
	super::Free();
}