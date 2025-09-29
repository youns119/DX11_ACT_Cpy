#include "pch.h"
#include "PlayerState_ShellSkill_RollOut.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"
#include "Shell_Part.h"
#include "ShellSkill_RollOut.h"

CPlayerState_ShellSkill_RollOut::CPlayerState_ShellSkill_RollOut(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_ShellSkill_RollOut::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("EnterShellRollout");

	return S_OK;
}

void CPlayerState_ShellSkill_RollOut::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_SHELLSKILL_ROLLOUT);

	// 속도 조절 불가능
	// 콤보 카운트 초기화
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;
	m_pPlayerStat->bRolling = true;
	m_pPlayerStat->bSuperArmor = true;

	m_iImpact = 0;

	m_pMovement->Reset_Velocity();
	m_pMovement->Add_Force(XMVectorSet(0.f, 1.f, 0.f, 0.f), 50.f);
	m_pMovement->Set_MaxFallingSpeed(m_pPlayerStat->fMaxFallingSpeed * 1.5f);
	m_pMovement->Set_GravityScale(m_pPlayerStat->fGravityScale * 1.5f);

	m_pPlayer->Get_PlayerShell()->Use_Skill();

	m_pPlayer->Get_UpperStateMachine()->ChangeState(L"None");

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength() * 0.75f);

	m_pPlayer->Play_PlayerSound(L"Shell_RollOut_Charge");
	m_pPlayer->Play_PlayerSound_Loop(L"Shell_RollOut_Loop");
}

void CPlayerState_ShellSkill_RollOut::Update(_float fTimeDelta)
{
	if (!m_pPlayerStat->bInAir)
	{
		_vec3 vLook = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::LOOK);

		m_pMovement->Set_MaxSpeed(m_pPlayerStat->fMaxSpeed * 5.f);
		m_pMovement->Set_Accel(m_pPlayerStat->fAccel * 3.f);
		m_pMovement->Add_Velocity(vLook._vector(), 100.f);

		if (m_iImpact == 0)
		{
			m_iImpact++;
			m_pPlayer->Apply_RadialBlur(1.5f);

			m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength() * 1.5f);
		}
	}

	if (m_iImpact == 1)
	{
		CGameInstance::GetInstance()->Call_Lag(0.06f, 0.08f);
		m_pPlayer->Player_CameraShake_Position(0.06f, 30.f, 0.8f);

		m_iImpact++;
	}
}

void CPlayerState_ShellSkill_RollOut::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_SHELLSKILL_ROLLOUT);

	// 속도 조절 변수 초기화
	// 상태 전환 변수 초기화
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bTransition = false;
	m_pPlayerStat->bCanHammerAnim = true;
	m_pPlayerStat->bRolling = false;
	m_pPlayerStat->bSuperArmor = false;

	m_iImpact = 0;

	m_pMovement->Set_MaxSpeed(m_pPlayerStat->fMaxSpeed);
	m_pMovement->Set_Accel(m_pPlayerStat->fAccel);
	m_pMovement->Set_MaxFallingSpeed(m_pPlayerStat->fMaxFallingSpeed);
	m_pMovement->Set_GravityScale(m_pPlayerStat->fGravityScale);

	CShellSkill_RollOut* pSkill = static_cast<CShellSkill_RollOut*>(m_pPlayer->Get_PlayerShell()->Get_ShellSkill());
	pSkill->Cancel_Skill();

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength());
}

CPlayerState_ShellSkill_RollOut* CPlayerState_ShellSkill_RollOut::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_ShellSkill_RollOut* pInstance = new CPlayerState_ShellSkill_RollOut(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_ShellSkill_RollOut");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_ShellSkill_RollOut::Free()
{
	super::Free();
}