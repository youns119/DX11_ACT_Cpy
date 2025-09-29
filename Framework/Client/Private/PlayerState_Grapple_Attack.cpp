#include "pch.h"
#include "PlayerState_Grapple_Attack.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "CineCamera_Player.h"

CPlayerState_Grapple_Attack::CPlayerState_Grapple_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Grapple_Attack::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("WaveBreaker-Punch");

	return S_OK;
}

void CPlayerState_Grapple_Attack::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_GRAPPLE_ATTACK);

	// 입력 불가능
	// 갈고리 액션 활성화
	// 콤보 카운트 초기화
	m_pPlayerStat->bInputEnable = false;
	m_pPlayerStat->bGrappling = true;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;
	m_pPlayerStat->bInvincible = true;
	m_pPlayerStat->fDamageFactor = 5.f;
	m_pPlayerStat->eDamageType = IDamaged::DAMAGE::DAMAGE_HEAVY;

	m_pMovement->Reset_Velocity();

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.15f);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength() * 0.65f);
	m_pPlayer->Get_PlayerCineCamera()->Play_Cinematic(L"GrappleAttack");
}

void CPlayerState_Grapple_Attack::Update(_float fTimeDelta)
{
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
		m_pOwner->ChangeState(L"Grapple_AttackEnd");
}

void CPlayerState_Grapple_Attack::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_GRAPPLE_ATTACK);

	// 입력 가능
	// 갈고리 액션 비활성화
	m_pPlayerStat->bInputEnable = true;
	m_pPlayerStat->bGrappling = false;
	m_pPlayerStat->bCanHammerAnim = true;
	m_pPlayerStat->bInvincible = false;

	m_pMovement->FlyMode(false);
	m_pMovement->Reset_Velocity();
	m_pMovement->Set_ForceFriction(m_pPlayerStat->fForceFriction);

	m_pPlayer->Player_HandCollider(false);
}

CPlayerState_Grapple_Attack* CPlayerState_Grapple_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Grapple_Attack* pInstance = new CPlayerState_Grapple_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Grapple_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Grapple_Attack::Free()
{
	super::Free();
}