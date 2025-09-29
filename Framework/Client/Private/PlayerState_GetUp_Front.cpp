#include "pch.h"
#include "PlayerState_GetUp_Front.h"

#include "GameObject.h"
#include "Model.h"
#include "Movement.h"
#include "Animation.h"
#include "StateMachine.h"

#include "Player.h"
#include "Weapon_Player.h"
#include "GameInstance.h"

CPlayerState_GetUp_Front::CPlayerState_GetUp_Front(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_GetUp_Front::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("GetUp-Forward");

	return S_OK;
}

void CPlayerState_GetUp_Front::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_GETUP_FRONT);

	// 기본 움직임 조작 불가능
	// 속도 조절 불가능
	// 무적 활성화
	// 콤보 카운트 초기화
	m_pPlayerStat->bMoveInputEnable = false;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->bInvincible = true;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);
}

void CPlayerState_GetUp_Front::Update(_float fTimeDelta)
{
	// 현재 애니메이션 블렌드 시간 진입하면 Idle로 상태 전환
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_GetUp_Front::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_GETUP_FRONT);

	// 기본 움직임 조작 초기화
	// 속도 조절 변수 초기화
	// 무적 비활성화
	// 상태 전환 변수 초기화
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bInvincible = false;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->bCanHammerAnim = true;
}

CPlayerState_GetUp_Front* CPlayerState_GetUp_Front::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_GetUp_Front* pInstance = new CPlayerState_GetUp_Front(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_GetUp_Front");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_GetUp_Front::Free()
{
	super::Free();
}