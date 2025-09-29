#include "pch.h"
#include "PlayerState_JumpLaunch.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_JumpLaunch::CPlayerState_JumpLaunch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_JumpLaunch::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("JumpLaunch");

	return S_OK;
}

void CPlayerState_JumpLaunch::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_JUMPLAUNCH);

	// 속도 조절 불가능
	// 콤보 카운트 초기화
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	if (CGameInstance::GetInstance()->Get_CurrentLevelIndex() != LEVEL_BEACH)
	{
		m_pPlayerStat->bSwimmable = true;
		m_pPlayerStat->bCanAirAction = true;

		m_pPlayer->Play_PlayerSound(L"Jump_Bubble");
	}

	m_pPlayer->Play_PlayerSound(L"Jump_Voice");

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);
}

void CPlayerState_JumpLaunch::Update(_float fTimeDelta)
{
	// 현재 애니메이션 블렌드 시간 진입하면 Airbone 상태로 전환
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
		m_pOwner->ChangeState(L"Airbone");
}

void CPlayerState_JumpLaunch::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_JUMPLAUNCH);

	// 속도 조절 변수 초기화
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bCanHammerAnim = true;
}

CPlayerState_JumpLaunch* CPlayerState_JumpLaunch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_JumpLaunch* pInstance = new CPlayerState_JumpLaunch(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_JumpLaunch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_JumpLaunch::Free()
{
	super::Free();
}