#include "pch.h"
#include "PlayerState_Airbone.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_Airbone::CPlayerState_Airbone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Airbone::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("AirboneFalling");

	return S_OK;
}

void CPlayerState_Airbone::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_AIRBONE);

	// 속도 조절 불가능
	// 콤보 카운트 초기화
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pModel->SetUp_Animation(m_iAnimIndex, true, 0.2f);
}

void CPlayerState_Airbone::Update(_float fTimeDelta)
{
	// 땅에 닿으면 JumpLand 상태로 전환
	if (!m_pPlayerStat->bInAir)
		m_pOwner->ChangeState(L"JumpLand");
}

void CPlayerState_Airbone::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_AIRBONE);

	// 속도 조절 변수 초기화
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bCanHammerAnim = true;

	// Call_Effects();
}

void CPlayerState_Airbone::Call_Effects()
{
	CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	_vector vPos{ pTransform->Get_State(CTransform::STATE::POSITION) }, vLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPSANDBURST, vPos, vLook, false, nullptr);
}

CPlayerState_Airbone* CPlayerState_Airbone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Airbone* pInstance = new CPlayerState_Airbone(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Airbone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Airbone::Free()
{
	super::Free();
}