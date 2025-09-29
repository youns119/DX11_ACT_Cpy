#include "pch.h"
#include "PlayerState_Guard.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_Guard::CPlayerState_Guard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Guard::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_eGuard = GUARD::GUARD_PRE;

	m_iAnimIndex[static_cast<_uint>(GUARD::GUARD_PRE)] = m_pModel->Get_AnimationIndex("NakedBlock");
	m_iAnimIndex[static_cast<_uint>(GUARD::GUARD)] = m_pModel->Get_AnimationIndex("NakedBlockIdle");

	return S_OK;
}

void CPlayerState_Guard::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_GUARD);

	// �ӵ� ���� �Ұ���
	// Guard ���� true
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->bGuard = true;

	// ó���� ������ Guard_Pre ���
	m_pModel->SetUp_SlotAnimation(ANIMSLOT::SLOT_UPPER, m_iAnimIndex[static_cast<_uint>(GUARD::GUARD_PRE)], false, 0.1f);
}

void CPlayerState_Guard::Update(_float fTimeDelta)
{
	// ���� �ִϸ��̼��� ���� ���� ���� ���Խ� �� �ִϸ��̼� ����
	if (m_eGuard == GUARD::GUARD_PRE &&
		m_pModel->Get_SlotAnimation(ANIMSLOT::SLOT_UPPER)->Get_Blendable())
	{
		m_eGuard = GUARD::GUARD;
		m_pModel->SetUp_SlotAnimation(ANIMSLOT::SLOT_UPPER, m_iAnimIndex[static_cast<_uint>(GUARD::GUARD)], true, 0.1f);
	}
}

void CPlayerState_Guard::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_GUARD);

	// �ӵ� ���� ���� �ʱ�ȭ
	// Guard ���� �ʱ�ȭ
	m_pPlayerStat->bRunnable = true;
	m_eGuard = GUARD::GUARD_PRE;
	m_pPlayerStat->bGuard = false;
}

CPlayerState_Guard* CPlayerState_Guard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Guard* pInstance = new CPlayerState_Guard(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Guard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Guard::Free()
{
	super::Free();
}