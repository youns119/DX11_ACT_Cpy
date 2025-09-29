#include "pch.h"
#include "PlayerState_ShellAttacked.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "Shell_Part.h"
#include "Shell_DataBase.h"

CPlayerState_ShellAttacked::CPlayerState_ShellAttacked(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_ShellAttacked::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("BlockGunShoot");

	return S_OK;
}

void CPlayerState_ShellAttacked::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_SHELLATTACKED);

	// �⺻ ������ ���� �Ұ���
	// ���� Ȱ��ȭ
	// �ӵ� ���� �Ұ���
	// �޺� ī��Ʈ �ʱ�ȭ
	m_pPlayerStat->bMoveInputEnable = false;
	m_pPlayerStat->bGuard = true;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);

	// ���� �ӵ� 0.1��� ����
	m_pMovement->Set_HorizontalVelocity(_vec3(m_pMovement->Get_HorizontalVelocity())._vector() * 0.5f);

	SHELL eType = m_pPlayer->Get_PlayerShellStat_Const()->eShell;
	switch ((_uint)eType)
	{
	case (_uint)SHELL::SHELL_HOME :
		m_pPlayer->Play_PlayerSound(L"Shell_Attacked_Home");
		break;
	case (_uint)SHELL::SHELL_SODACAN :
		m_pPlayer->Play_PlayerSound(L"Shell_Attacked_SodaCan");
		break;
	case (_uint)SHELL::SHELL_MIRRORBALL:
		m_pPlayer->Play_PlayerSound(L"Shell_Attacked_MirrorBall");
		break;
	case (_uint)SHELL::SHELL_DUCK:
		m_pPlayer->Play_PlayerSound(L"Shell_Attacked_Duck");
		break;
	case (_uint)SHELL::SHELL_TENNISBALL:
		m_pPlayer->Play_PlayerSound(L"Shell_Attacked_TennisBall");
		break;
	default :
		break;
	}
}

void CPlayerState_ShellAttacked::Update(_float fTimeDelta)
{
	// ���� �ִϸ��̼� ���� �ð� �����ϸ� Idle�� ���� ��ȯ
	// ���� ���� ���°� �ƴϸ� Airbone ���·� ��ȯ
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->bInAir)
			m_pOwner->ChangeState(L"Airbone");
		else m_pOwner->ChangeState(L"ShellIdle");
	}
}

void CPlayerState_ShellAttacked::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_SHELLATTACKED);

	// �⺻ ������ ���� �ʱ�ȭ
	// ���� ��Ȱ��ȭ
	// �ӵ� ���� ���� �ʱ�ȭ
	// ���� ��ȯ ���� �ʱ�ȭ
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bGuard = false;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->bCanHammerAnim = true;
}

CPlayerState_ShellAttacked* CPlayerState_ShellAttacked::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_ShellAttacked* pInstance = new CPlayerState_ShellAttacked(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_ShellAttacked");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ShellAttacked::Free()
{
	super::Free();
}