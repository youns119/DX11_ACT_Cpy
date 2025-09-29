#include "pch.h"
#include "PlayerState_ShellEquip.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_ShellEquip::CPlayerState_ShellEquip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_ShellEquip::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("EnterShell");

	return S_OK;
}

void CPlayerState_ShellEquip::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_SHELLEQUIP);

	// �⺻ ������ ���� �Ұ���
	// �ӵ� ���� �Ұ���
	// �޺� ī��Ʈ �ʱ�ȭ
	m_pPlayerStat->bMoveInputEnable = false;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);
}

void CPlayerState_ShellEquip::Update(_float fTimeDelta)
{
	// ���� �ִϸ��̼� ���� �ð� �����ϸ� Idle�� ���� ��ȯ
	// ���� ���� ���°� �ƴϸ� Airbone ���·� ��ȯ
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->bInAir)
			m_pOwner->ChangeState(L"Airbone");
		else if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_ShellEquip::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_SHELLEQUIP);

	// �⺻ ������ ���� �ʱ�ȭ
	// �ӵ� ���� ���� �ʱ�ȭ
	// ���� ��ȯ ���� �ʱ�ȭ
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pMovement->FlyMode(false);
	m_pMovement->Reset_Velocity();
	m_pMovement->Set_ForceFriction(m_pPlayerStat->fForceFriction);
}

CPlayerState_ShellEquip* CPlayerState_ShellEquip::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_ShellEquip* pInstance = new CPlayerState_ShellEquip(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_ShellEquip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ShellEquip::Free()
{
	super::Free();
}