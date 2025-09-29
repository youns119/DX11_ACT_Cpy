#include "pch.h"
#include "PlayerState_HeavyAttacked_Back.h"

#include "GameInstance.h"

#include "GameObject.h"
#include "Model.h"
#include "Movement.h"
#include "Animation.h"
#include "StateMachine.h"

#include "Player.h"
#include "Weapon_Player.h"

CPlayerState_HeavyAttacked_Back::CPlayerState_HeavyAttacked_Back(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super{ pDevice, pContext }
{
}

HRESULT CPlayerState_HeavyAttacked_Back::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex[static_cast<_uint>(HEAVYATTACKED_BACK::HEAVYATTACKED_BACK_START)] = m_pModel->Get_AnimationIndex("Prone");
	m_iAnimIndex[static_cast<_uint>(HEAVYATTACKED_BACK::HEAVYATTACKED_BACK_LOOP)] = m_pModel->Get_AnimationIndex("ProneFallingLoop");

	return S_OK;
}

void CPlayerState_HeavyAttacked_Back::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_HEAVYATTACKED_BACK);

	// 기본 움직임 조작 불가능
	// 속도 조절 불가능
	// 무적 활성화
	// 콤보 카운트 초기화
	m_pPlayerStat->bMoveInputEnable = false;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->bInvincible = true;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	m_eAttacked = HEAVYATTACKED_BACK::HEAVYATTACKED_BACK_START;

	m_pMovement->Set_RotateToDirection(false);

	m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eAttacked)], false, 0.1f);

	// 플레이어 무기 위치 변경
	m_pPlayer->Get_PlayerWeapon()->MoveToSocket(CWeapon_Player::SOCKET::SOCKET_CLAW_END);

	m_pPlayer->Play_PlayerSound(L"Impact");

	_uint iRand = rand() % 4;
	switch (iRand)
	{
	case 0:
		m_pPlayer->Play_PlayerSound(L"HeavyAttacked1");
		break;
	case 1:
		m_pPlayer->Play_PlayerSound(L"HeavyAttacked2");
		break;
	case 2:
		m_pPlayer->Play_PlayerSound(L"HeavyAttacked3");
		break;
	case 3:
		m_pPlayer->Play_PlayerSound(L"HeavyAttacked4");
		break;
	default:
		break;
	}
}

void CPlayerState_HeavyAttacked_Back::Update(_float fTimeDelta)
{
	// 현재 애니메이션 블렌드 시간 진입하면 
	// bDeath가 true면 HeavyDeath_Front 상태로 전환
	// 공중이고 Start인 경우 Loop 애니메이션으로 전환
	// 아니면 GetUp 상태로 전환
	if (!m_pPlayerStat->bInAir && m_eAttacked == HEAVYATTACKED_BACK::HEAVYATTACKED_BACK_LOOP)
	{
		if (m_pPlayerStat->fCurrHP <= 0.f)
		{
			m_pOwner->ChangeState(L"HeavyDeath_Back");
			m_pPlayerStat->bInvincible = false;
			m_pPlayerStat->bDeath = true;
		}
		else
			m_pOwner->ChangeState(L"GetUp_Back");
	}
	else if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->fCurrHP <= 0.f)
		{
			m_pOwner->ChangeState(L"HeavyDeath_Back");
			m_pPlayerStat->bInvincible = false;
			m_pPlayerStat->bDeath = true;
		}
		else if (m_pPlayerStat->bInAir)
		{
			if (m_eAttacked == HEAVYATTACKED_BACK::HEAVYATTACKED_BACK_START)
			{
				m_eAttacked = HEAVYATTACKED_BACK::HEAVYATTACKED_BACK_LOOP;
				m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eAttacked)], false, 0.1f);
			}
		}
		else
		{
			CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
			_vec3 vPos = pTransform->Get_State(CTransform::STATE::POSITION);
			_vec3 vLook = pTransform->Get_State(CTransform::STATE::LOOK) * -1.f;

			vLook = vPos + vLook;
			pTransform->LookAt(vLook._vector());

			m_pOwner->ChangeState(L"GetUp_Back");
		}
	}
}

void CPlayerState_HeavyAttacked_Back::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_HEAVYATTACKED_BACK);

	// 기본 움직임 조작 초기화
	// 속도 조절 변수 초기화
	// 무적 비활성화
	// 상태 전환 변수 초기화
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bInvincible = false;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pMovement->Set_RotateToDirection(true);

	m_eAttacked = HEAVYATTACKED_BACK::HEAVYATTACKED_BACK_END;

	// 플레이어 무기 위치 초기화
	m_pPlayer->Get_PlayerWeapon()->MoveToSocket(CWeapon_Player::SOCKET::SOCKET_HAND);
}

CPlayerState_HeavyAttacked_Back* CPlayerState_HeavyAttacked_Back::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_HeavyAttacked_Back* pInstance = new CPlayerState_HeavyAttacked_Back(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_HeavyAttacked_Back");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_HeavyAttacked_Back::Free()
{
	super::Free();
}