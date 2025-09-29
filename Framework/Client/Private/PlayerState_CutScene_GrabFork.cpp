#include "pch.h"
#include "PlayerState_CutScene_GrabFork.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Movement.h"
#include "Animation.h"
#include "StateMachine.h"

#include "Player.h"
#include "CineCamera_Player.h"

CPlayerState_CutScene_GrabFork::CPlayerState_CutScene_GrabFork(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_CutScene_GrabFork::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("CutsceneGrabFork");

	return S_OK;
}

void CPlayerState_CutScene_GrabFork::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_CUTSCENE_GRABFORK);

	m_pPlayer->Reset_PlayerStat();

	// 입력 불가능
	m_pPlayerStat->bInputEnable = false;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.f);
	m_pModel->Set_AnimationFrame_Specific(0.f);
	m_pModel->Get_CurrAnimation()->Set_Active(false);
}

void CPlayerState_CutScene_GrabFork::Update(_float fTimeDelta)
{
	// 현재 애니메이션 블렌드 시간 진입하면 Idle로 상태 전환
	if (m_pPlayer->Get_PlayerCineCamera()->Get_Active() && !m_pModel->Get_CurrAnimation()->Get_Active())
	{
		m_pModel->Get_CurrAnimation()->Set_Active(true);
		// Call_Effects();
	}
	else if (m_pModel->Get_CurrAnimation()->Get_Blendable())
		m_pOwner->ChangeState(L"Idle");
}

void CPlayerState_CutScene_GrabFork::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_CUTSCENE_GRABFORK);

	// 입력 가능
	m_pPlayerStat->bInputEnable = true;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pPlayer->Get_PlayerCineCamera()->Stop_Cinematic();
}

void CPlayerState_CutScene_GrabFork::Call_Effects()
{
	CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	_vector vPos{ pTransform->Get_State(CTransform::STATE::POSITION) }, vLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_SPRT_HEAL_SPREAD, vPos, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HEAL_SPREAD, vPos, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HEAL_CIRCLE, vPos, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HEAL_DIAMOND, vPos, vLook, false, nullptr);
}

CPlayerState_CutScene_GrabFork* CPlayerState_CutScene_GrabFork::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_CutScene_GrabFork* pInstance = new CPlayerState_CutScene_GrabFork(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_CutScene_GrabFork");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_CutScene_GrabFork::Free()
{
	super::Free();
}