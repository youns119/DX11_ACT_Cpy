#include "pch.h"
#include "PlayerState_NormalAttack.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_NormalAttack::CPlayerState_NormalAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_NormalAttack::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex[static_cast<_uint>(NORMALATTACK::NORMALATTACK_1)] = m_pModel->Get_AnimationIndex("Atk-ComboV4-1");
	m_iAnimIndex[static_cast<_uint>(NORMALATTACK::NORMALATTACK_2)] = m_pModel->Get_AnimationIndex("Atk-ComboV4-2");
	m_iAnimIndex[static_cast<_uint>(NORMALATTACK::NORMALATTACK_3)] = m_pModel->Get_AnimationIndex("Atk-ComboV4-3");
	m_iAnimIndex[static_cast<_uint>(NORMALATTACK::NORMALATTACK_4)] = m_pModel->Get_AnimationIndex("Atk-ComboV4-4");

	m_iAnimIndex[static_cast<_uint>(NORMALATTACK::HAMMERATTACK_1)] = m_pModel->Get_AnimationIndex("HammerAttack");
	m_iAnimIndex[static_cast<_uint>(NORMALATTACK::HAMMERATTACK_2)] = m_pModel->Get_AnimationIndex("HammerAttack2");

	return S_OK;
}

void CPlayerState_NormalAttack::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_NORMALATTACK);

	// 기본 움직임 조작 불가능
	// 속도 조절 불가능
	// ComboCount 증가
	m_pPlayerStat->bMoveInputEnable = false;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount++;
	m_pPlayerStat->bCanHammerAnim = false;
	m_pPlayerStat->eDamageType = IDamaged::DAMAGE::DAMAGE_NORMAL;

	if (!m_pPlayerStat->bHasHammer)
	{
		m_pPlayerStat->fDamageFactor = 1.f;

		m_eNormalAttack = (NORMALATTACK)m_pPlayerStat->iComboCount;
		m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eNormalAttack) - 1], false, 0.1f);
	}
	else
	{
		m_pPlayerStat->bSuperArmor = true;
		m_pPlayerStat->fDamageFactor = 2.f;

		m_eNormalAttack = (NORMALATTACK)m_pPlayerStat->iComboCount;
		m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eNormalAttack) + 2], false, 0.1f);
	}

	// 플레이어 AutoLock 활성화
	m_pPlayer->Set_AutoLock(true);
}

void CPlayerState_NormalAttack::Update(_float fTimeDelta)
{
	// 현재 애니메이션 블렌드 시간에 진입하면 Idle로 상태 전환
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_NormalAttack::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_NORMALATTACK);

	// 플레이어 Weapon Collider 초기화
	m_pPlayer->Player_WeaponCollider(false);

	// 기본 움직임 조작 초기화
	// 속도 조절 변수 초기화
	// bTransition 변수 false로 초기화
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->fAnimSpeed = 1.f;
	m_pPlayerStat->bCanHammerAnim = true;

	// ComboCount 최대치 넘어가면 0으로 초기화
	if (!m_pPlayerStat->bHasHammer)
	{
		if (m_pPlayerStat->iComboCount >= m_pPlayerStat->iComboMax)
			m_pPlayerStat->iComboCount = 0;
	}
	else
	{
		m_pPlayerStat->bSuperArmor = false;

		if (m_pPlayerStat->iComboCount >= m_pPlayerStat->iHammerComboMax)
			m_pPlayerStat->iComboCount = 0;
	}

	m_pMovement->Set_Decel(m_pPlayerStat->fDecel);

	// 플레이어 AutoLock 비활성화
	m_pPlayer->Set_AutoLock(false);
}

CPlayerState_NormalAttack* CPlayerState_NormalAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_NormalAttack* pInstance = new CPlayerState_NormalAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_NormalAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_NormalAttack::Free()
{
	super::Free();
}