#include "pch.h"
#include "PlayerState_Grapple_AttackStart.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_Grapple_AttackStart::CPlayerState_Grapple_AttackStart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Grapple_AttackStart::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("WaveBreaker-Grab");

	return S_OK;
}

void CPlayerState_Grapple_AttackStart::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_GRAPPLE_ATTACKSTART);

	// 입력 불가능
	// 갈고리 액션 활성화
	// 콤보 카운트 초기화
	m_pPlayerStat->bInputEnable = false;
	m_pPlayerStat->bGrappling = true;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.15f);

	m_pPlayer->Play_PlayerSound(L"Hammer_ChargeAttack_Voice");
	m_pPlayer->Play_PlayerSound(L"Grapple_AttackStart");
}

void CPlayerState_Grapple_AttackStart::Update(_float fTimeDelta)
{
}

void CPlayerState_Grapple_AttackStart::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_GRAPPLE_ATTACKSTART);

	// 입력 가능
	// 갈고리 액션 비활성화
	m_pPlayerStat->bInputEnable = true;
	m_pPlayerStat->bGrappling = false;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pMovement->Activate(true);
}

CPlayerState_Grapple_AttackStart* CPlayerState_Grapple_AttackStart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Grapple_AttackStart* pInstance = new CPlayerState_Grapple_AttackStart(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Grapple_AttackStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Grapple_AttackStart::Free()
{
	super::Free();
}