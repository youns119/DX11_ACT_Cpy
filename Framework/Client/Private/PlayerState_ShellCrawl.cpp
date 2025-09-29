#include "pch.h"
#include "PlayerState_ShellCrawl.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_ShellCrawl::CPlayerState_ShellCrawl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_ShellCrawl::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("BlockingCrawl3");

	return S_OK;
}

void CPlayerState_ShellCrawl::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_SHELLCRAWL);

	// �⺻ ������ ���� ����
	// ���� Ȱ��ȭ
	// �ӵ� ���� �Ұ���
	// �޺� ī��Ʈ �ʱ�ȭ
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bGuard = true;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pMovement->Set_Accel(m_pPlayerStat->fShellAccel);
	m_pMovement->Set_Decel(m_pPlayerStat->fShellDecel);
	m_pMovement->Set_MaxSpeed(m_pPlayerStat->fShellMaxSpeed);

	m_pModel->SetUp_Animation(m_iAnimIndex, true, 0.1f);
}

void CPlayerState_ShellCrawl::Update(_float fTimeDelta)
{
	// ���� ������ ��� Airbone���� ���� ��ȯ
	if (m_pPlayerStat->bInAir)
		m_pOwner->ChangeState(L"Airbone");

	// �ӵ� 0�̸� ShellIdle ���·� ��ȯ
	_float fSpeed = m_pMovement->Get_HorizontalSpeed();
	if (fSpeed == 0.f)
		m_pOwner->ChangeState(L"ShellIdle");

	m_fFootStepSoundAcc += fTimeDelta;
	if (m_fFootStepSoundAcc >= m_fFootStepSoundDist)
	{
		m_fFootStepSoundAcc = 0.f;
		m_pPlayer->Play_PlayerSound(L"FootStep");
	}
}

void CPlayerState_ShellCrawl::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_SHELLCRAWL);

	// ���� �ʱ�ȭ
	// �ӵ� ���� ���� �ʱ�ȭ
	m_pPlayerStat->bGuard = false;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pMovement->Set_Accel(m_pPlayerStat->fAccel);
	m_pMovement->Set_Decel(m_pPlayerStat->fDecel);
	m_pMovement->Set_MaxSpeed(m_pPlayerStat->fMaxSpeed);
}

CPlayerState_ShellCrawl* CPlayerState_ShellCrawl::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_ShellCrawl* pInstance = new CPlayerState_ShellCrawl(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_ShellCrawl");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ShellCrawl::Free()
{
	super::Free();
}