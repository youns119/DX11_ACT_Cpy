#include "pch.h"
#include "PlayerState_Grabbed.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "SpringArm_Player.h"

CPlayerState_Grabbed::CPlayerState_Grabbed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Grabbed::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("Struggle");

	return S_OK;
}

void CPlayerState_Grabbed::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_GRABBED);

	m_pPlayer->Reset_PlayerStat();

	// Grabbed ���� true
	// InputEnable ���� false
	// LockOn ���� false
	// �޺� ī��Ʈ �ʱ�ȭ
	m_pPlayerStat->bGrabbed = true;
	m_pPlayerStat->bInputEnable = false;
	m_pPlayerStat->bLockOn = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	// ���� �Ǿ����� ��츦 ����� �÷��̾� �������� ���� ����
	m_pPlayer->Get_PlayerSpringArm()->UnLock_LockOn();

	// �ظ� ���Ծִϸ��̼� ��Ȱ��ȭ
	if (m_pPlayerStat->bHasHammer && m_pModel->Is_SlotAnimActive(ANIMSLOT::SLOT_HAMMER))
		m_pModel->Activate_SlotAnim(ANIMSLOT::SLOT_HAMMER, false);

	m_pModel->SetUp_Animation(m_iAnimIndex, true, 0.f);
}

void CPlayerState_Grabbed::Update(_float fTimeDelta)
{

}

void CPlayerState_Grabbed::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_GRABBED);

	// Grabbed ���� false
	// InputEnable ���� true
	m_pPlayerStat->bGrabbed = false;
	m_pPlayerStat->bInputEnable = true;
	m_pPlayerStat->bCanHammerAnim = true;
}

CPlayerState_Grabbed* CPlayerState_Grabbed::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Grabbed* pInstance = new CPlayerState_Grabbed(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Grabbed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Grabbed::Free()
{
	super::Free();
}