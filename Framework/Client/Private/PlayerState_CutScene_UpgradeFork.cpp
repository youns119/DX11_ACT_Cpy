#include "pch.h"
#include "PlayerState_CutScene_UpgradeFork.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Movement.h"
#include "Animation.h"
#include "StateMachine.h"

#include "Player.h"
#include "CineCamera_Player.h"

CPlayerState_CutScene_UpgradeFork::CPlayerState_CutScene_UpgradeFork(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_CutScene_UpgradeFork::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("CutsceneUpgradeFork");

	return S_OK;
}

void CPlayerState_CutScene_UpgradeFork::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_CUTSCENE_UPGRADEFORK);

	m_pPlayer->Reset_PlayerStat();

	// 입력 불가능
	m_pPlayerStat->bInputEnable = false;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.2f);
}

void CPlayerState_CutScene_UpgradeFork::Update(_float fTimeDelta)
{
	// 현재 애니메이션 블렌드 시간 진입하면 Idle로 상태 전환
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
		m_pOwner->ChangeState(L"Idle");
}

void CPlayerState_CutScene_UpgradeFork::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_CUTSCENE_UPGRADEFORK);

	// 입력 가능
	m_pPlayerStat->bInputEnable = true;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pPlayer->Get_PlayerCineCamera()->Stop_Cinematic();
}

CPlayerState_CutScene_UpgradeFork* CPlayerState_CutScene_UpgradeFork::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_CutScene_UpgradeFork* pInstance = new CPlayerState_CutScene_UpgradeFork(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_CutScene_UpgradeFork");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_CutScene_UpgradeFork::Free()
{
	super::Free();
}