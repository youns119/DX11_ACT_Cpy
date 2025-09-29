#include "pch.h"
#include "PlayerState_ShellIdle.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_ShellIdle::CPlayerState_ShellIdle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_ShellIdle::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("BlockIdle3");

	return S_OK;
}

void CPlayerState_ShellIdle::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_SHELLIDLE);

	// 기본 움직임 조작 가능
	// 가드 활성화
	// 속도 조절 불가능
	// 콤보 카운트 초기화
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bGuard = true;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pModel->SetUp_Animation(m_iAnimIndex, true, 0.1f);
}

void CPlayerState_ShellIdle::Update(_float fTimeDelta)
{
	// 공중 상태일 경우 Airbone으로 상태 전환
	if (m_pPlayerStat->bInAir && (!m_pPlayerStat->bMagnetPulling && !m_pPlayerStat->bMagnetPulled))
		m_pOwner->ChangeState(L"Airbone");

	// 속도 0이 아니고 Running 상태 아니면 ShellCrawl로 전환
	_float fSpeed = m_pMovement->Get_HorizontalSpeed();
	if (fSpeed != 0.f)
		m_pOwner->ChangeState(L"ShellCrawl");
}

void CPlayerState_ShellIdle::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_SHELLIDLE);

	// 가드 초기화
	// 속도 조절 변수 초기화
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bGuard = false;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bCanHammerAnim = true;
}

CPlayerState_ShellIdle* CPlayerState_ShellIdle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_ShellIdle* pInstance = new CPlayerState_ShellIdle(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_ShellIdle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ShellIdle::Free()
{
	super::Free();
}