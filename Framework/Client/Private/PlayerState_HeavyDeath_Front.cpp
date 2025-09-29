#include "pch.h"
#include "PlayerState_HeavyDeath_Front.h"

#include "GameInstance.h"

#include "GameObject.h"
#include "Model.h"
#include "Movement.h"
#include "Animation.h"
#include "StateMachine.h"

#include "Player.h"
#include "Weapon_Player.h"

CPlayerState_HeavyDeath_Front::CPlayerState_HeavyDeath_Front(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_HeavyDeath_Front::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("DieProneForward");

	return S_OK;
}

void CPlayerState_HeavyDeath_Front::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_HEAVYDEATH_FRONT);

	m_pPlayer->Reset_PlayerStat();

	// 조작 불가능
	m_pPlayerStat->bInputEnable = false;
	m_pPlayerStat->bDeath = true;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);

	m_pMovement->Set_RotateToDirection(false);

	// 속도 0으로 초기화
	m_pMovement->Reset_Velocity();

	_uint iRand = rand() % 2;
	switch (iRand)
	{
	case 0:
		m_pPlayer->Play_PlayerSound(L"HeavyDeath1");
		break;
	case 1:
		m_pPlayer->Play_PlayerSound(L"HeavyDeath2");
		break;
	default:
		break;
	}
}

void CPlayerState_HeavyDeath_Front::Update(_float fTimeDelta)
{
}

void CPlayerState_HeavyDeath_Front::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_HEAVYDEATH_FRONT);

	// 조작 가능
	m_pPlayerStat->bInputEnable = true;
	m_pPlayerStat->bDeath = false;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pMovement->Set_RotateToDirection(true);
}

CPlayerState_HeavyDeath_Front* CPlayerState_HeavyDeath_Front::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_HeavyDeath_Front* pInstance = new CPlayerState_HeavyDeath_Front(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_HeavyDeath_Front");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_HeavyDeath_Front::Free()
{
	super::Free();
}