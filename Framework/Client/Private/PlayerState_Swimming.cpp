#include "pch.h"
#include "PlayerState_Swimming.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_Swimming::CPlayerState_Swimming(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Swimming::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("Swimming");

	m_fSwimmingMaxTime = m_pPlayer->Get_PlayerStat()->fSwimmingMaxTime;
	m_fSwimmingFallingSpeed = m_pPlayer->Get_PlayerStat()->fSwimmingFallingSpeed;

	return S_OK;
}

void CPlayerState_Swimming::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_SWIMMING);

	// �ӵ� ���� �Ұ���
	// �޺� ī��Ʈ �ʱ�ȭ
	// Swimming ���� true
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bSwimming = true;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pPlayer->Play_PlayerSound_Loop(L"Swim_Bubble");

	m_pModel->SetUp_Animation(m_iAnimIndex, true);
}

void CPlayerState_Swimming::Update(_float fTimeDelta)
{
	// ���� ������ JumpLand ���·� ��ȯ
	if (!m_pPlayerStat->bInAir)
	{
		m_pOwner->ChangeState(L"JumpLand");
		return;
	}

	// Swimming ������ �� ���� �ð� üũ
	// ���� �ð� ���� ��� Airbone ���·� ��ȯ
	// bSwimmable�� false��
	m_pPlayerStat->fSwimmingTimeAcc += fTimeDelta;
	if (m_pPlayerStat->fSwimmingTimeAcc > m_fSwimmingMaxTime)
	{
		m_pPlayerStat->bSwimmable = false;
		m_pPlayerStat->bCanAirAction = false;
		m_pOwner->ChangeState(L"Airbone");

		return;
	}
}

void CPlayerState_Swimming::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_SWIMMING);

	// Swimming ���� �ʱ�ȭ
	// �ӵ� ���� ���� �ʱ�ȭ
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bSwimming = false;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pMovement->Set_MaxFallingSpeed(m_pPlayerStat->fMaxFallingSpeed);

	CGameInstance::GetInstance()->Stop_Sound((_uint)SOUND_CHANNEL::PLAYER_AMBIENCE);
}

CPlayerState_Swimming* CPlayerState_Swimming::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Swimming* pInstance = new CPlayerState_Swimming(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Swimming");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Swimming::Free()
{
	super::Free();
}