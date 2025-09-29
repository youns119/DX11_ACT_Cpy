#include "pch.h"
#include "PlayerState_ParryAttack.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "CineCamera_Player.h"

CPlayerState_ParryAttack::CPlayerState_ParryAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_ParryAttack::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("AttackUppercut");

	return S_OK;
}

void CPlayerState_ParryAttack::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_PARRYATTACK);

	// 기본 움직임 조작 불가능
	// 속도 조절 불가능
	// CanParryAttack 변수 초기화
	m_pPlayerStat->bMoveInputEnable = false;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->bCanParryAttack = false;
	m_pPlayerStat->bCanHammerAnim = false;
	m_pPlayerStat->bInvincible = true;
	m_pPlayerStat->eDamageType = IDamaged::DAMAGE::DAMAGE_HEAVY;

	m_pPlayerStat->fDamageFactor = 4.f;

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);

	// 플레이어 AutoLock 활성화
	m_pPlayer->Set_AutoLock(true);

	if (m_pPlayer->Get_AutoLockTarget() != nullptr)
	{
		CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
		_vec3 vTargetPos = static_cast<CTransform*>(m_pPlayer->Get_AutoLockTarget()->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);
		_vec3 vPos = pTransform->Get_State(CTransform::STATE::POSITION);
		_vec3 vDir = (vTargetPos - vPos).Normalized();

		pTransform->LookAt((vPos + vDir)._vector());
	}

	//m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength() * 0.75f);

	m_pPlayer->Get_PlayerCineCamera()->Play_Cinematic(L"ParryAttack");

	m_pPlayer->Play_PlayerSound(L"BackStep_Success");
	m_pPlayer->Play_PlayerSound(L"Hammer_ChargeAttack_Voice");
}

void CPlayerState_ParryAttack::Update(_float fTimeDelta)
{
	// 현재 애니메이션 블렌드 시간에 진입하면 Idle로 상태 전환
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_ParryAttack::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_PARRYATTACK);

	// 플레이어 Hand Collider 초기화
	m_pPlayer->Player_HandCollider(false);

	// 기본 움직임 조작 초기화
	// 속도 조절 변수 초기화
	// bTransition 변수 false로 초기화
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->bCanHammerAnim = true;
	m_pPlayerStat->bInvincible = false;

	// 플레이어 AutoLock 비활성화
	m_pPlayer->Set_AutoLock(false);

	if (m_pPlayer->Get_PlayerCineCamera()->IsCurrCam())
		m_pPlayer->Get_PlayerCineCamera()->Stop_Cinematic();

	//m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength());
}

CPlayerState_ParryAttack* CPlayerState_ParryAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_ParryAttack* pInstance = new CPlayerState_ParryAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_ParryAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ParryAttack::Free()
{
	super::Free();
}