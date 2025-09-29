#include "pch.h"
#include "PlayerState_GetUp_Back.h"

#include "GameObject.h"
#include "Model.h"
#include "Movement.h"
#include "Animation.h"
#include "StateMachine.h"

#include "Player.h"
#include "Weapon_Player.h"
#include "GameInstance.h"

CPlayerState_GetUp_Back::CPlayerState_GetUp_Back(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_GetUp_Back::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("GetUp");

	return S_OK;
}

void CPlayerState_GetUp_Back::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_GETUP_BACK);

	// �⺻ ������ ���� �Ұ���
	// �ӵ� ���� �Ұ���
	// ���� Ȱ��ȭ
	// �޺� ī��Ʈ �ʱ�ȭ
	m_pPlayerStat->bMoveInputEnable = false;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->bInvincible = true;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pMovement->Set_RotateToDirection(false);
	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);
}

void CPlayerState_GetUp_Back::Update(_float fTimeDelta)
{
	// ���� �ִϸ��̼� ���� �ð� �����ϸ� Idle�� ���� ��ȯ
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_GetUp_Back::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_GETUP_BACK);

	// �⺻ ������ ���� �ʱ�ȭ
	// �ӵ� ���� ���� �ʱ�ȭ
	// ���� ��Ȱ��ȭ
	// ���� ��ȯ ���� �ʱ�ȭ
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bInvincible = false;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pMovement->Set_RotateToDirection(true);
}
CPlayerState_GetUp_Back* CPlayerState_GetUp_Back::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_GetUp_Back* pInstance = new CPlayerState_GetUp_Back(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_GetUp_Back");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_GetUp_Back::Free()
{
	super::Free();
}