#include "pch.h"
#include "PlayerState_SuperAttacked.h"

#include "GameInstance.h"
#include "Model.h"
#include "Movement.h"
#include "Animation.h"
#include "StateMachine.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Weapon_Player.h"
#include "Effect_Textured_Rope.h"

CPlayerState_SuperAttacked::CPlayerState_SuperAttacked(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_SuperAttacked::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex[static_cast<_uint>(ATTACKED::ATTACKED_Y)] = m_pModel->Get_AnimationIndex("Slapstick.ProneSpinY");
	m_iAnimIndex[static_cast<_uint>(ATTACKED::ATTACKED_X)] = m_pModel->Get_AnimationIndex("Slapstick.PronedSpinX");

	return S_OK;
}

void CPlayerState_SuperAttacked::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_SUPERATTACKED);

	// 속도 조절 불가능
	// 콤보 카운트 초기화
	m_pPlayerStat->bInputEnable = false;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bInvincible = true;
	m_pPlayerStat->bCanHammerAnim = false;

	m_eAttacked = ATTACKED::ATTACKED_Y;

	CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	XMStoreFloat3(&m_vInitialPos, pTransform->Get_State(CTransform::STATE::POSITION));

	m_pMovement->Set_RotateToDirection(false);
	m_pMovement->Set_RotateToDirection_3D(true);
	m_pMovement->Set_ForceFriction(1.f);
	m_pMovement->Set_MaxFallingSpeed(20.f);
	m_pMovement->Set_RotationScale(1.f);

	m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eAttacked)], true, 0.1f);

	CEffect_Textured_Rope* pRope = static_cast<CEffect_Textured_Rope*>(m_pPlayer->Find_PartObject({ (L"Part_Rope") }));
	if(pRope != nullptr)
		pRope->Clear_Info();

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength() * 2.f);

	m_pPlayer->Play_PlayerSound(L"Impact");

	_uint iRand = rand() % 6;
	switch (iRand)
	{
	case 0:
		m_pPlayer->Play_PlayerSound(L"SuperAttacked1");
		break;
	case 1:
		m_pPlayer->Play_PlayerSound(L"SuperAttacked2");
		break;
	case 2:
		m_pPlayer->Play_PlayerSound(L"SuperAttacked3");
		break;
	case 3:
		m_pPlayer->Play_PlayerSound(L"SuperAttacked4");
		break;
	case 4:
		m_pPlayer->Play_PlayerSound(L"SuperAttacked5");
		break;
	case 5:
		m_pPlayer->Play_PlayerSound(L"SuperAttacked6");
		break;
	default:
		break;
	}

	m_pPlayer->Play_PlayerSound(L"SuperAttacked_Fly");
}

void CPlayerState_SuperAttacked::Update(_float fTimeDelta)
{
	// 현재 애니메이션 블렌드 시간 진입하면 Idle로 상태 전환
	// 땅에 닿은 상태가 아니면 Airbone 상태로 전환
	if (m_pPlayerStat->bInAir)
	{
		CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
		_vec3 vPrevPos = m_vInitialPos;
		_vec3 vPos = pTransform->Get_State(CTransform::STATE::POSITION);

		_vec3 vDir = (vPos - vPrevPos).Normalized();
		pTransform->UpAt((vPos + vDir)._vector());

		XMStoreFloat3(&m_vInitialPos, vPos._vector());
	}
	else if (!m_pPlayerStat->bInAir && m_eAttacked == ATTACKED::ATTACKED_Y)
	{
		CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
		_vec3 vRotation = pTransform->Get_Rotation();
		pTransform->Rotate(0.f, vRotation.y, 0.f);

		m_pMovement->Set_RotateToDirection(true);
		m_pMovement->Set_RotateToDirection_3D(false);
		m_pMovement->Set_ForceFriction(m_pPlayerStat->fForceFriction);
		m_pMovement->Set_MaxFallingSpeed(m_pPlayerStat->fMaxFallingSpeed);
		m_pMovement->Set_RotationScale(m_pPlayerStat->fRotationScale);

		m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength());

		if (m_pPlayerStat->fCurrHP <= 0.f)
		{
			m_pPlayerStat->bDeath = true;
			m_pOwner->ChangeState(L"NormalDeath");

			return;
		}
		else
		{
			m_eAttacked = ATTACKED::ATTACKED_X;
			m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eAttacked)], false, 0.1f);
		}

		CGameInstance::GetInstance()->Stop_Sound((_uint)SOUND_CHANNEL::PLAYER_LOOP);
	}
	else if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_eAttacked == ATTACKED::ATTACKED_X)
		{
			CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
			_vec3 vRotation = pTransform->Get_Rotation();
			pTransform->Rotate(0.f, vRotation.y, 0.f);

			m_pMovement->Set_RotateToDirection(true);
			m_pMovement->Set_RotateToDirection_3D(false);
			m_pMovement->Set_ForceFriction(m_pPlayerStat->fForceFriction);
			m_pMovement->Set_MaxFallingSpeed(m_pPlayerStat->fMaxFallingSpeed);
			m_pMovement->Set_RotationScale(m_pPlayerStat->fRotationScale);

			m_pOwner->ChangeState(L"Idle");
		}
	}
}

void CPlayerState_SuperAttacked::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_SUPERATTACKED);

	// 속도 조절 변수 초기화
	// 상태 전환 변수 초기화
	m_pPlayerStat->bInputEnable = true;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->bInvincible = false;
	m_pPlayerStat->bCanHammerAnim = true;

	m_eAttacked = ATTACKED::ATTACKED_END;

	CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	_vec3 vRotation = pTransform->Get_Rotation();
	pTransform->Rotate(0.f, vRotation.y, 0.f);

	m_pMovement->Set_RotateToDirection(true);
	m_pMovement->Set_RotateToDirection_3D(false);
	m_pMovement->Set_ForceFriction(m_pPlayerStat->fForceFriction);
	m_pMovement->Set_MaxFallingSpeed(m_pPlayerStat->fMaxFallingSpeed);
	m_pMovement->Set_RotationScale(m_pPlayerStat->fRotationScale);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength());

	CGameInstance::GetInstance()->Stop_Sound((_uint)SOUND_CHANNEL::PLAYER_LOOP);
}

CPlayerState_SuperAttacked* CPlayerState_SuperAttacked::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_SuperAttacked* pInstance = new CPlayerState_SuperAttacked(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_SuperAttacked");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_SuperAttacked::Free()
{
	super::Free();
}