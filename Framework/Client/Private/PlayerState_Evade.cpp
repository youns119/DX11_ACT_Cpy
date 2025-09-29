#include "pch.h"
#include "PlayerState_Evade.h"

#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_Evade::CPlayerState_Evade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Evade::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex[static_cast<_uint>(EVADE::EVADE_NAKED)] = m_pModel->Get_AnimationIndex("DodgeNaked");
	m_iAnimIndex[static_cast<_uint>(EVADE::EVADE_MEDIUM)] = m_pModel->Get_AnimationIndex("DodgeMedium");
	m_iAnimIndex[static_cast<_uint>(EVADE::EVADE_HEAVY)] = m_pModel->Get_AnimationIndex("DodgeHeavy");

	return S_OK;
}

void CPlayerState_Evade::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_EVADE);


	// 기본 움직임 조작 불가능
	// 속도 조절 불가능
	// 콤보 카운트 초기화
	m_pPlayerStat->bMoveInputEnable = false;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;
	m_pPlayerStat->bSwimmable = false;

	CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	_vec3 vPos = pTransform->Get_State(CTransform::STATE::POSITION);
	_vec3 Dir = m_pMovement->Get_InputDir();
	pTransform->LookAt((vPos + Dir)._vector());

	m_eEvade = EVADE::EVADE_NAKED;
	if(!m_pPlayerStat->bInAir && m_pPlayerStat->bHasShell)
	{
		if (m_pPlayer->Get_PlayerShellStat_Const()->eShellWeight == SHELLWEIGHT::SHELLWEIGHT_S)
		{
			m_eEvade = EVADE::EVADE_NAKED;

			if(CGameInstance::GetInstance()->Get_CurrentLevelIndex() != LEVEL_BEACH)
				m_pPlayer->Play_PlayerSound(L"DodgeNaked_Bubble");
			m_pPlayer->Play_PlayerSound(L"DodgeNaked");
		}
		else if (m_pPlayer->Get_PlayerShellStat_Const()->eShellWeight == SHELLWEIGHT::SHELLWEIGHT_M)
		{
			m_eEvade = EVADE::EVADE_MEDIUM;

			m_pPlayer->Play_PlayerSound(L"DodgeMedium");
			m_pPlayer->Play_PlayerSound(L"DodgeMedium_Bubble");
		}
		else if (m_pPlayer->Get_PlayerShellStat_Const()->eShellWeight == SHELLWEIGHT::SHELLWEIGHT_L)
		{
			m_eEvade = EVADE::EVADE_HEAVY;

			m_pPlayer->Play_PlayerSound(L"DodgeHeavy");
			m_pPlayer->Play_PlayerSound(L"DodgeHeavy_Bubble");
		}
	}
	else
	{
		if (CGameInstance::GetInstance()->Get_CurrentLevelIndex() != LEVEL_BEACH)
			m_pPlayer->Play_PlayerSound(L"DodgeNaked_Bubble");
		m_pPlayer->Play_PlayerSound(L"DodgeNaked");
	}


	m_pMovement->Set_MaxFallingSpeed(m_pPlayerStat->fMaxFallingSpeed);

	m_pPlayer->Play_PlayerSound(L"Dodge_Voice");

	m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eEvade)], false, 0.1f);
}

void CPlayerState_Evade::Update(_float fTimeDelta)
{
	// 현재 애니메이션 블렌드 시간 진입하면 
	// 공중일 때 Airbone으로 상태 전환
	// 공중 아닐 때 Idle으로 상태 전환
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->bInAir)
			m_pOwner->ChangeState(L"Airbone");
		else if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_Evade::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_EVADE);

	// 기본 움직임 조작 초기화
	// 속도 조절 변수 초기화
	// Evade 관련 변수 초기화
	// Invincible 상태 초기화
	// bTransition 변수 false로 초기화
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bEvade = false;
	m_pPlayerStat->bInvincible = false;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->bCanHammerAnim = true;
	m_pPlayerStat->bCanAirAction = false;
	
	m_pMovement->Set_GravityScale(m_pPlayerStat->fGravityScale);
	m_pMovement->Set_InAirControlRatio(m_pPlayerStat->fInAirControlRatio);
}

CPlayerState_Evade* CPlayerState_Evade::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Evade* pInstance = new CPlayerState_Evade(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Evade");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Evade::Free()
{
	super::Free();
}