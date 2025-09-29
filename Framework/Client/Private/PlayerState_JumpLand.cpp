#include "pch.h"
#include "PlayerState_JumpLand.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_JumpLand::CPlayerState_JumpLand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_JumpLand::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("JumpLand");

	return S_OK;
}

void CPlayerState_JumpLand::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_JUMPLAND);

	// 콤보 카운트 초기화
	// 착지했으니 Swimming 값들 초기화
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->fSwimmingTimeAcc = 0.f;

	if (CGameInstance::GetInstance()->Get_CurrentLevelIndex() != LEVEL_BEACH)
	{
		m_pPlayerStat->bSwimmable = true;
		m_pPlayerStat->bCanAirAction = true;
	}

	m_pPlayer->Play_PlayerSound(L"FootStepLand");

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);
}

void CPlayerState_JumpLand::Update(_float fTimeDelta)
{
	// 현재 애니메이션 블렌드 시간 진입하면 Idle 상태로 전환
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_JumpLand::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_JUMPLAND);

	m_pMovement->Set_RotateToDirection(true);
}

CPlayerState_JumpLand* CPlayerState_JumpLand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_JumpLand* pInstance = new CPlayerState_JumpLand(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_JumpLand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_JumpLand::Free()
{
	super::Free();
}