#include "pch.h"
#include "PlayerState_Parry.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_Parry::CPlayerState_Parry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Parry::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("UnblockNakedParry");

	return S_OK;
}

void CPlayerState_Parry::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_PARRY);

	// �⺻ ������ ���� ����
	// �ӵ� ���� �Ұ���
	// �޺� ī��Ʈ �ʱ�ȭ
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);

	m_pPlayer->Play_PlayerSound(L"Parry");
}

void CPlayerState_Parry::Update(_float fTimeDelta)
{
	// �� �ִϸ��̼��� ���� ���� ���� �����ϸ� 
	// ���� ������ ��� Airbone���� ���� ��ȯ
	// ������ �ƴ� ��� Idle�� ���� ��ȯ
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->bInAir)
			m_pOwner->ChangeState(L"Airbone");
		else if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_Parry::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_PARRY);

	// �ӵ� ���� ���� �ʱ�ȭ
	// Parry ���� �ʱ�ȭ
	// Invincible ���� �ʱ�ȭ
	// CanParryAttack ���� �ʱ�ȭ
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bParry = false;
	m_pPlayerStat->bInvincible = false;
	m_pPlayerStat->bCanParryAttack = false;
}

CPlayerState_Parry* CPlayerState_Parry::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Parry* pInstance = new CPlayerState_Parry(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Parry");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Parry::Free()
{
	super::Free();
}