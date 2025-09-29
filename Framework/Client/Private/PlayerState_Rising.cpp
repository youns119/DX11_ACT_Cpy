#include "pch.h"
#include "PlayerState_Rising.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_Rising::CPlayerState_Rising(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Rising::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("AirboneRising");

	return S_OK;
}

void CPlayerState_Rising::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_RISING);

	// 속도 조절 불가능
	// 수영 가능 활성화
	// 콤보 카운트 초기화
	// 수영 시간 초기화
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->bSwimmable = true;
	m_pPlayerStat->bCanAirAction = true;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->fSwimmingTimeAcc = 0.f;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.2f);
}

void CPlayerState_Rising::Update(_float fTimeDelta)
{
	// 블렌드 가능 상태로 진입하면 Airbone 상태로 전환
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		m_pOwner->ChangeState(L"Airbone");
		return;
	}

	// 땅에 닿으면 JumpLand 상태로 전환
	if (!m_pPlayerStat->bInAir)
		m_pOwner->ChangeState(L"JumpLand");
}

void CPlayerState_Rising::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_RISING);

	// 속도 조절 변수 초기화
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bCanHammerAnim = true;
}

CPlayerState_Rising* CPlayerState_Rising::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Rising* pInstance = new CPlayerState_Rising(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Rising");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Rising::Free()
{
	super::Free();
}