#include "pch.h"
#include "PlayerState_SuperUltraCheatMode.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "SpringArm_Player.h"

CPlayerState_SuperUltraCheatMode::CPlayerState_SuperUltraCheatMode(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_SuperUltraCheatMode::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex[static_cast<_uint>(CHEAT::CHEAT1)] = m_pModel->Get_AnimationIndex("krillzotsky");
	m_iAnimIndex[static_cast<_uint>(CHEAT::CHEAT2)] = m_pModel->Get_AnimationIndex("Climbing-forward");
	m_iAnimIndex[static_cast<_uint>(CHEAT::CHEAT3)] = m_pModel->Get_AnimationIndex("CutsceneReallyStrongFan");
	m_iAnimIndex[static_cast<_uint>(CHEAT::CHEAT4)] = m_pModel->Get_AnimationIndex("DanceyDance");
	m_iAnimIndex[static_cast<_uint>(CHEAT::CHEAT5)] = m_pModel->Get_AnimationIndex("Emote_Flair-Loop");
	m_iAnimIndex[static_cast<_uint>(CHEAT::CHEAT6)] = m_pModel->Get_AnimationIndex("Grapple-Shortcut-Loop");
	m_iAnimIndex[static_cast<_uint>(CHEAT::CHEAT7)] = m_pModel->Get_AnimationIndex("GroundSit-Loop");
	m_iAnimIndex[static_cast<_uint>(CHEAT::CHEAT8)] = m_pModel->Get_AnimationIndex("SitLoop");
	m_iAnimIndex[static_cast<_uint>(CHEAT::CHEAT9)] = m_pModel->Get_AnimationIndex("Slapstick.ProneSpinY");

	return S_OK;
}

void CPlayerState_SuperUltraCheatMode::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_CHEAT);
	
	// 플레이어 스탯 초기화
	m_pPlayer->Reset_PlayerStat();
	
	// Cheat 상태 활성화
	// Invincible 상태 활설화
	m_pPlayerStat->bCheat = true;
	m_pPlayerStat->bInvincible = true;

	m_pMovement->FlyMode(true);
	m_pPlayer->Get_PlayerSpringArm()->Set_ActiveLag(false);
	m_pMovement->Set_Decel(m_pPlayerStat->fMaxSpeed * m_pPlayerStat->fCheatSpeedRatio * 0.3f);
	m_pMovement->Set_MaxSpeed(m_pPlayerStat->fMaxSpeed * m_pPlayerStat->fCheatSpeedRatio);

	CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	_float3 vRotation = pTransform->Get_Rotation();
	pTransform->Rotate(0.f, vRotation.y, 0.f);

	_uint iRand = rand() % 9;
	m_pModel->SetUp_Animation(m_iAnimIndex[iRand], true, 0.1f);
}

void CPlayerState_SuperUltraCheatMode::Update(_float fTimeDelta)
{
}

void CPlayerState_SuperUltraCheatMode::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_CHEAT);

	// 플레이어 스탯 초기화
	m_pPlayerStat->bCheat = false;
	m_pPlayer->Reset_PlayerStat();

	m_pMovement->FlyMode(false);
	m_pPlayer->Get_PlayerSpringArm()->Set_ActiveLag(true);

	CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	_float3 vRotation = pTransform->Get_Rotation();
	pTransform->Rotate(0.f, vRotation.y, 0.f);
}

CPlayerState_SuperUltraCheatMode* CPlayerState_SuperUltraCheatMode::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_SuperUltraCheatMode* pInstance = new CPlayerState_SuperUltraCheatMode(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_SuperUltraCheatMode");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_SuperUltraCheatMode::Free()
{
	super::Free();
}