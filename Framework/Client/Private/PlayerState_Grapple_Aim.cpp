#include "pch.h"
#include "PlayerState_Grapple_Aim.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "SpringArm_Player.h"

CPlayerState_Grapple_Aim::CPlayerState_Grapple_Aim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Grapple_Aim::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("SpearAim");

	return S_OK;
}

void CPlayerState_Grapple_Aim::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_GRAPPLE_AIM);

	// 기본 움직임 조작 가능
	// 속도 조절 불가능
	// 갈고리 조준 활성화
	// 콤보 카운트 초기화
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->bGrappleAim = true;
	m_pPlayerStat->bCanHammerAnim = false;
	m_pPlayerStat->iComboCount = 0;

	m_pPlayer->Grapple_Aim();

	m_pModel->SetUp_SlotAnimation(ANIMSLOT::SLOT_UPPER, m_iAnimIndex, false, 0.1f);
}

void CPlayerState_Grapple_Aim::Update(_float fTimeDelta)
{
	if (m_pPlayerStat->bInAir && !CGameInstance::GetInstance()->Get_IsLag())
		m_pPlayer->Player_TimeSlow(true, 0.15f);
	else if (!m_pPlayerStat->bInAir && CGameInstance::GetInstance()->Get_IsLag())
		m_pPlayer->Player_TimeSlow(false);
}

void CPlayerState_Grapple_Aim::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_GRAPPLE_AIM);

	// 속도 조절 변수 초기화
	// 갈고리 조준 비활성화
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bGrappleAim = false;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pPlayer->Grapple_Aim();

	m_pPlayer->Player_TimeSlow(false);
}

CPlayerState_Grapple_Aim* CPlayerState_Grapple_Aim::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Grapple_Aim* pInstance = new CPlayerState_Grapple_Aim(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Grapple_Aim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Grapple_Aim::Free()
{
	super::Free();
}