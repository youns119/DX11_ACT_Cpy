#include "pch.h"
#include "PlayerState_Urchin.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "Weapon_Player.h"
#include "CineCamera_Player.h"

CPlayerState_Urchin::CPlayerState_Urchin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Urchin::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex[static_cast<_uint>(URCHIN::URCHIN_BACKFLIP)] = m_pModel->Get_AnimationIndex("Backflip_Urchin");
	m_iAnimIndex[static_cast<_uint>(URCHIN::URCHIN_THROW)] = m_pModel->Get_AnimationIndex("UmamiAbility-ThrowUrchin");
	m_iAnimIndex[static_cast<_uint>(URCHIN::URCHIN_DODGE)] = m_pModel->Get_AnimationIndex("Urchin_Dodge");

	return S_OK;
}

void CPlayerState_Urchin::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_URCHIN);

	// 조작 불가능
	m_pPlayerStat->bInputEnable = false;
	m_pPlayerStat->bCanHammerAnim = false;
	m_pPlayerStat->bInvincible = true;
	m_pPlayerStat->eDamageType = IDamaged::DAMAGE::DAMAGE_NORMAL;

	m_pPlayerStat->fDamageFactor = 5.f;

	m_eUrchin = URCHIN::URCHIN_BACKFLIP;

	m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eUrchin)], false, 0.1f);

	CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	_vec3 vLook = pTransform->Get_State(CTransform::STATE::LOOK) * -1.f;
	vLook.y += 3.f;

	m_pMovement->Activate(true);
	m_pMovement->Set_RotateToDirection(false);
	m_pMovement->Add_Force(vLook._vector(), 45.f);

	m_pPlayer->Play_PlayerSound(L"Jump_Voice");
	m_pPlayer->Play_PlayerSound(L"DodgeMedium");
	m_pPlayer->Play_PlayerSound(L"DodgeMedium_Bubble");
}

void CPlayerState_Urchin::Update(_float fTimeDelta)
{
	if (m_pPlayerStat->bUrchinThrow && m_eUrchin == URCHIN::URCHIN_THROW)
		m_pModel->Get_CurrAnimation()->Set_Active(true);

	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_eUrchin == URCHIN::URCHIN_BACKFLIP)
		{
			m_eUrchin = URCHIN::URCHIN_THROW;
			m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eUrchin)], false, 0.f);
		}
		else if (m_eUrchin == URCHIN::URCHIN_THROW)
		{
			m_pMovement->Activate(true);
			m_pMovement->Set_RotateToDirection(false);

			m_eUrchin = URCHIN::URCHIN_DODGE;
			m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eUrchin)], false, 0.1f);
		}
		else if (m_eUrchin == URCHIN::URCHIN_DODGE)
		{
			m_pPlayerStat->bInputEnable = true;
			m_pMovement->Reset_Velocity();
			m_pPlayer->Get_PlayerCineCamera()->Stop_Cinematic();

			if (m_pPlayerStat->bInAir)
				m_pOwner->ChangeState(L"Airbone");
			else m_pOwner->ChangeState(L"Idle");
		}
	}
}

void CPlayerState_Urchin::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_URCHIN);

	// 조작 초기화
	m_pPlayerStat->bInputEnable = true;
	m_pPlayerStat->bCanHammerAnim = true;
	m_pPlayerStat->bInvincible = false;

	m_pModel->Get_CurrAnimation()->Set_Active(true);
	m_pPlayerStat->bUrchinThrow = false;
	m_pPlayerStat->bUrchinLaunch = false;

	m_pMovement->Activate(true);
	m_pMovement->Set_RotateToDirection(true);

	if (m_pPlayer->Get_PlayerCineCamera()->IsCurrCam())
		m_pPlayer->Get_PlayerCineCamera()->Stop_Cinematic();
}

CPlayerState_Urchin* CPlayerState_Urchin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Urchin* pInstance = new CPlayerState_Urchin(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Urchin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Urchin::Free()
{
	super::Free();
}