#include "pch.h"
#include "PlayerState_ChargeAttack.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Skill_DataBase.h"

CPlayerState_ChargeAttack::CPlayerState_ChargeAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_ChargeAttack::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex[static_cast<_uint>(CHARGEATTACK::CHARGEATTACK_1)] = m_pModel->Get_AnimationIndex("AtkChargeThrust");
	m_iAnimIndex[static_cast<_uint>(CHARGEATTACK::HAMMERCHARGEATTACK_1)] = m_pModel->Get_AnimationIndex("AtkChargeThrustHammer");

	return S_OK;
}

void CPlayerState_ChargeAttack::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_CHARGEATTACK);

	// 기본 움직임 조작 불가능
	// 속도 조절 불가능
	// 해머 전환 가능
	// ComboCount 1 증가
	m_pPlayerStat->bMoveInputEnable = false;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->bCharging = true;
	m_pPlayerStat->iComboCount++;
	m_pPlayerStat->bCanHammerAnim = false;
	m_pPlayerStat->eDamageType = IDamaged::DAMAGE::DAMAGE_HEAVY;

	if(CSkill_DataBase::Get().Get_ActionSkill(ACTIONSKILL::ACTIONSKILL_HAMMER).bUnLocked)
		m_pPlayerStat->bCanBeHammer = true;

	if (!m_pPlayerStat->bHasHammer)
	{
		m_pPlayerStat->fDamageFactor = 2.f;

		m_eChargeAttack = CHARGEATTACK::CHARGEATTACK_1;
		m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eChargeAttack)], false, 0.1f);
	}
	else
	{
		m_pPlayerStat->bSuperArmor = true;
		m_pPlayerStat->fDamageFactor = 3.f;

		m_eChargeAttack = CHARGEATTACK::HAMMERCHARGEATTACK_1;
		m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eChargeAttack)], false, 0.1f);
	}

	m_pPlayer->Get_UpperStateMachine()->ChangeState(L"None");

	// 플레이어 AutoLock 활성화
	m_pPlayer->Set_AutoLock(true);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength() * 0.85f);
}

void CPlayerState_ChargeAttack::Update(_float fTimeDelta)
{
	// 현재 애니메이션 블렌드 시간에 진입하면 Idle로 상태 전환
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_ChargeAttack::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_CHARGEATTACK);

	// 플레이어 Weapon Collider 초기화
	m_pPlayer->Player_WeaponCollider(false);

	// 기본 움직임 조작 초기화
	// 속도 조절 변수 초기화
	// 해머 전환 불가능
	// bTransition 변수 false로 초기화
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bCharging = false;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->bCanBeHammer = false;
	m_pPlayerStat->bCanHammerAnim = true;
	m_pPlayerStat->bSuperArmor = false;

	// 플레이어 AutoLock 비활성화
	m_pPlayer->Set_AutoLock(false);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength());
}

CPlayerState_ChargeAttack* CPlayerState_ChargeAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_ChargeAttack* pInstance = new CPlayerState_ChargeAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_ChargeAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ChargeAttack::Free()
{
	super::Free();
}