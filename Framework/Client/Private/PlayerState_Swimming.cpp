#include "pch.h"
#include "PlayerState_Swimming.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_Swimming::CPlayerState_Swimming(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Swimming::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("Swimming");

	m_fSwimmingMaxTime = m_pPlayer->Get_PlayerStat()->fSwimmingMaxTime;
	m_fSwimmingFallingSpeed = m_pPlayer->Get_PlayerStat()->fSwimmingFallingSpeed;

	return S_OK;
}

void CPlayerState_Swimming::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_SWIMMING);

	// 속도 조절 불가능
	// 콤보 카운트 초기화
	// Swimming 변수 true
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bSwimming = true;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pPlayer->Play_PlayerSound_Loop(L"Swim_Bubble");

	m_pModel->SetUp_Animation(m_iAnimIndex, true);
}

void CPlayerState_Swimming::Update(_float fTimeDelta)
{
	// 땅에 닿으면 JumpLand 상태로 전환
	if (!m_pPlayerStat->bInAir)
	{
		m_pOwner->ChangeState(L"JumpLand");
		return;
	}

	// Swimming 상태일 때 제한 시간 체크
	// 제한 시간 넘을 경우 Airbone 상태로 전환
	// bSwimmable은 false로
	m_pPlayerStat->fSwimmingTimeAcc += fTimeDelta;
	if (m_pPlayerStat->fSwimmingTimeAcc > m_fSwimmingMaxTime)
	{
		m_pPlayerStat->bSwimmable = false;
		m_pPlayerStat->bCanAirAction = false;
		m_pOwner->ChangeState(L"Airbone");

		return;
	}
}

void CPlayerState_Swimming::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_SWIMMING);

	// Swimming 변수 초기화
	// 속도 조절 변수 초기화
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bSwimming = false;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pMovement->Set_MaxFallingSpeed(m_pPlayerStat->fMaxFallingSpeed);

	CGameInstance::GetInstance()->Stop_Sound((_uint)SOUND_CHANNEL::PLAYER_AMBIENCE);
}

CPlayerState_Swimming* CPlayerState_Swimming::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Swimming* pInstance = new CPlayerState_Swimming(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Swimming");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Swimming::Free()
{
	super::Free();
}