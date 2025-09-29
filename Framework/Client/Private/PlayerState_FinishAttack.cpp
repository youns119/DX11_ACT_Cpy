#include "pch.h"
#include "PlayerState_FinishAttack.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"
#include "CineCamera_Player.h"
#include "Enemy.h"

CPlayerState_FinishAttack::CPlayerState_FinishAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_FinishAttack::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex[static_cast<_uint>(FINISHATTACK::FINISHATTACK_ENTER)] = m_pModel->Get_AnimationIndex("flowstate.Enter");
	m_iAnimIndex[static_cast<_uint>(FINISHATTACK::FINISHATTACK_STAB)] = m_pModel->Get_AnimationIndex("flowstate.Stab");
	m_iAnimIndex[static_cast<_uint>(FINISHATTACK::FINISHATTACK_EXIT)] = m_pModel->Get_AnimationIndex("flowstate.Exit");

	return S_OK;
}

void CPlayerState_FinishAttack::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_FINISHATTACK);

	// 입력 불가능
	// 콤보 카운트 초기화
	m_pPlayerStat->bInputEnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;
	m_pPlayerStat->bInvincible = true;
	m_pPlayerStat->bFinishAttack = true;
	m_pPlayerStat->eDamageType = IDamaged::DAMAGE::DAMAGE_HEAVY;

	m_pPlayerStat->fDamageFactor = 10.f;

	m_eFinishAttack = FINISHATTACK::FINISHATTACK_ENTER;

	CGameObject* pTarget = m_pPlayer->Get_FinishTarget();
	_vec3 vTargetDir = static_cast<CCollider*>(pTarget->Find_Component(L"Com_Collider"))->Get_BoundingCenter();
	_vec3 vMyPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	vTargetDir.y = vMyPos.y;
	m_pTransform->LookAt(vTargetDir._vector());

	CEnemy* pFinish = static_cast<CEnemy*>(m_pPlayer->Get_FinishTarget());
	pFinish->Set_Finisher(true);

	m_pMovement->Activate(false);
	m_pMovement->Set_RotateToDirection(false);

	m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eFinishAttack)], false, 0.1f);

	m_pPlayer->Get_PlayerCineCamera()->Play_Cinematic(L"FinishAttack");

	m_pPlayer->Play_PlayerSound(L"Hammer_ChargeAttack_Voice");
	m_pPlayer->Play_PlayerSound(L"FinishAttack");
	m_pPlayer->Play_PlayerSound(L"FinishAttack_Siren");
}

void CPlayerState_FinishAttack::Update(_float fTimeDelta)
{
	if (!m_pPlayerStat->bInAir && m_pPlayerStat->bFinishAttack)
	{
		m_pMovement->Activate(true);

		m_pPlayerStat->bFinishAttack = false;

		m_pPlayer->Get_PlayerCineCamera()->PositionShake(0.08f, 25.f, 0.8f);
		m_pPlayer->Get_PlayerCineCamera()->RotationShake(0.08f, 25.f, 0.8f);

		CGameInstance::GetInstance()->Call_Lag(0.01f, 0.3f);
	}

	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_eFinishAttack == FINISHATTACK::FINISHATTACK_ENTER)
		{
			m_eFinishAttack = FINISHATTACK::FINISHATTACK_STAB;
			m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eFinishAttack)], false, 0.f);
		}
		else if (m_eFinishAttack == FINISHATTACK::FINISHATTACK_STAB)
		{
			m_eFinishAttack = FINISHATTACK::FINISHATTACK_EXIT;
			m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eFinishAttack)], false, 0.f);

			m_pPlayer->Player_WeaponCollider(false);

			CEnemy* pEnemy = static_cast<CEnemy*>(m_pPlayer->Get_FinishTarget());
			pEnemy->Set_Finisher(false);
			m_pPlayer->Reset_Finish();
		}
		else if (m_eFinishAttack == FINISHATTACK::FINISHATTACK_EXIT)
		{
			m_pPlayerStat->bInvincible = false;
			m_pMovement->Set_RotateToDirection(true);

			if (m_pPlayerStat->bInAir)
				m_pOwner->ChangeState(L"Airbone");
			else if (m_pPlayerStat->bLockOn)
				m_pOwner->ChangeState(L"LockOn");
			else m_pOwner->ChangeState(L"Idle");
		}
	}
}

void CPlayerState_FinishAttack::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_FINISHATTACK);

	m_eFinishAttack = FINISHATTACK::FINISHATTACK_END;

	// 입력 가능
	m_pPlayerStat->bInputEnable = true;
	m_pPlayerStat->bCanHammerAnim = true;
	m_pPlayerStat->bInvincible = false;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->bFinishAttack = false;

	m_pMovement->Activate(true);
	m_pMovement->Set_RotateToDirection(true);

	m_pPlayer->Player_WeaponCollider(false);

	if (m_pPlayer->Get_PlayerCineCamera()->IsCurrCam())
		m_pPlayer->Get_PlayerCineCamera()->Stop_Cinematic();
}

CPlayerState_FinishAttack* CPlayerState_FinishAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_FinishAttack* pInstance = new CPlayerState_FinishAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_FinishAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_FinishAttack::Free()
{
	super::Free();
}