#include "pch.h"
#include "PlayerState_DashAttack.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Skill_DataBase.h"

CPlayerState_DashAttack::CPlayerState_DashAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_DashAttack::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex[static_cast<_uint>(DASHATTACK::DASHATTACK_1)] = m_pModel->Get_AnimationIndex("DashAttack");
	m_iAnimIndex[static_cast<_uint>(DASHATTACK::HAMMERDASHATTACK_1)] = m_pModel->Get_AnimationIndex("HammerDashAttack_002");

	return S_OK;
}

void CPlayerState_DashAttack::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_DASHATTACK);

	// �⺻ ������ ���� �Ұ���
	// �ӵ� ���� �Ұ���
	// ComboCount 1 ����
	// DashStart ���� true
	// �ظ� ��ȯ ����
	m_pPlayerStat->bMoveInputEnable = false;
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount++;
	m_pPlayerStat->bDashStart = true;
	m_pPlayerStat->bCanHammerAnim = false;
	m_pPlayerStat->eDamageType = IDamaged::DAMAGE::DAMAGE_HEAVY;

	if (CSkill_DataBase::Get().Get_ActionSkill(ACTIONSKILL::ACTIONSKILL_HAMMER).bUnLocked)
		m_pPlayerStat->bCanBeHammer = true;

	if (!m_pPlayerStat->bHasHammer)
	{
		m_pPlayerStat->fDamageFactor = 1.5f;

		m_eDashAttack = DASHATTACK::DASHATTACK_1;
		m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eDashAttack)], false, 0.1f);

		m_pMovement->Set_Decel(0.f);
	}
	else
	{
		m_pPlayerStat->bSuperArmor = true;
		m_pPlayerStat->fDamageFactor = 2.5f;

		m_eDashAttack = DASHATTACK::HAMMERDASHATTACK_1;
		m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eDashAttack)], false, 0.1f);

		m_pMovement->Set_Decel(m_pPlayerStat->fDecel * 0.5f);
	}

	m_pPlayer->Get_UpperStateMachine()->ChangeState(L"None");

	// �÷��̾� AutoLock Ȱ��ȭ
	m_pPlayer->Set_AutoLock(true);

	if (!m_pPlayerStat->bHasHammer)
	{
		m_pPlayer->Play_PlayerSound(L"DashAttack");
		m_pPlayer->Play_PlayerSound(L"ChargeAttack_Bubble");
		m_pPlayer->Play_PlayerSound(L"DashAttack_Voice");
	}

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength() * 0.85f);
}

void CPlayerState_DashAttack::Update(_float fTimeDelta)
{
	if (m_pPlayerStat->bDashStart && 
		!m_pPlayerStat->bHasHammer)
	{
		_vector vLook = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::LOOK);
		m_pMovement->Add_HorizontalVelocity(vLook, fTimeDelta);
	}

	// ���� �ִϸ��̼� ���� �ð��� �����ϸ� Idle�� ���� ��ȯ
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_DashAttack::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_DASHATTACK);

	// �÷��̾� Weapon Collider �ʱ�ȭ
	m_pPlayer->Player_WeaponCollider(false);

	// �⺻ ������ ���� �ʱ�ȭ
	// �ӵ� ���� ���� �ʱ�ȭ
	// bTransition ���� false�� �ʱ�ȭ
	// DashStart ���� false�� �ʱ�ȭ
	// �ظ� ��ȯ �Ұ���
	m_pPlayerStat->bMoveInputEnable = true;
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->bDashStart = false;
	m_pPlayerStat->bCanBeHammer = false;
	m_pPlayerStat->bCanHammerAnim = true;
	m_pPlayerStat->bSuperArmor = false;

	m_pMovement->Set_Decel(m_pPlayerStat->fDecel);
	m_pMovement->Set_ForceFriction(m_pPlayerStat->fForceFriction);

	// �÷��̾� AutoLock ��Ȱ��ȭ
	m_pPlayer->Set_AutoLock(false);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength());
}

CPlayerState_DashAttack* CPlayerState_DashAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_DashAttack* pInstance = new CPlayerState_DashAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_DashAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_DashAttack::Free()
{
	super::Free();
}