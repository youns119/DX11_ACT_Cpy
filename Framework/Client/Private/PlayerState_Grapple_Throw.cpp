#include "pch.h"
#include "PlayerState_Grapple_Throw.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_Grapple_Throw::CPlayerState_Grapple_Throw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Grapple_Throw::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("SpearThrow");

	return S_OK;
}

void CPlayerState_Grapple_Throw::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_GRAPPLE_THROW);

	// �⺻ ������ ���� �Ұ���
	// �ӵ� ���� �Ұ���
	// ���� �׼� Ȱ��ȭ
	// �޺� ī��Ʈ �ʱ�ȭ
	m_pPlayerStat->bMoveInputEnable = false;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->bGrappling = true;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);
}

void CPlayerState_Grapple_Throw::Update(_float fTimeDelta)
{
	// �� �ִϸ��̼��� ��ȯ ���� ���� �����ϸ� 
	// ���� ������ ��� Airbone���� ���� ��ȯ
	// ������ �ƴ� ��� Idle�� ���� ��ȯ
	// ���� Ÿ���� �����ϸ� �ٸ� ó��
	CGameObject* pGameObject = m_pPlayer->Get_GrappleTarget();
	_bool bGrappable = m_pPlayer->Get_PlayerStat()->bGrapplable;
	if (pGameObject && bGrappable)
	{
		
	}
	else if(m_pPlayerStat->bTransition)
	{
		if (m_pPlayerStat->bInAir)
			m_pOwner->ChangeState(L"Airbone");
		else if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_Grapple_Throw::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_GRAPPLE_THROW);

	// �⺻ ������ ���� ����
	// �ӵ� ���� ���� �ʱ�ȭ
	// ���� �׼� �ʱ�ȭ
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bGrappling = false;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pMovement->Activate(true);

	m_pPlayer->Grapple_Aim();
}

CPlayerState_Grapple_Throw* CPlayerState_Grapple_Throw::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Grapple_Throw* pInstance = new CPlayerState_Grapple_Throw(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Grapple_Throw");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Grapple_Throw::Free()
{
	super::Free();
}