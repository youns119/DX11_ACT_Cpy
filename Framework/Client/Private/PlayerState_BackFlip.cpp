#include "pch.h"
#include "PlayerState_BackFlip.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_BackFlip::CPlayerState_BackFlip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_BackFlip::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("Backflip_Shortcut");

	return S_OK;
}

void CPlayerState_BackFlip::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_BACKFLIP);

	// 속도 조절 불가능
	// 콤보 카운트 초기화
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);
}

void CPlayerState_BackFlip::Update(_float fTimeDelta)
{
	// 현재 애니메이션 블렌드 시간 진입하면 Rising 상태로 전환
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
		m_pOwner->ChangeState(L"Rising");
}

void CPlayerState_BackFlip::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_BACKFLIP);

	// 속도 조절 변수 초기화
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bCanHammerAnim = true;
}

CPlayerState_BackFlip* CPlayerState_BackFlip::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_BackFlip* pInstance = new CPlayerState_BackFlip(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_BackFlip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_BackFlip::Free()
{
	super::Free();
}