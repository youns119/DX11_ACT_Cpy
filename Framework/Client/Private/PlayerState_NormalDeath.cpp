#include "pch.h"
#include "PlayerState_NormalDeath.h"

#include "GameInstance.h"

#include "GameObject.h"
#include "Model.h"
#include "Movement.h"
#include "Animation.h"
#include "StateMachine.h"

#include "Player.h"
#include "Weapon_Player.h"

CPlayerState_NormalDeath::CPlayerState_NormalDeath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_NormalDeath::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("Death");

	return S_OK;
}

void CPlayerState_NormalDeath::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_NORMALDEATH);

	m_pPlayer->Reset_PlayerStat();

	// 조작 불가능
	m_pPlayerStat->bInputEnable = false;
	m_pPlayerStat->bDeath = true;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);

	// 속도 0으로 초기화
	m_pMovement->Reset_Velocity();

	_uint iRand = rand() % 3;
	switch (iRand)
	{
	case 0 :
		m_pPlayer->Play_PlayerSound(L"NormalDeath1");
		break;
	case 1:
		m_pPlayer->Play_PlayerSound(L"NormalDeath2");
		break;
	case 2:
		m_pPlayer->Play_PlayerSound(L"NormalDeath3");
		break;
	default :
		break;
	}
}

void CPlayerState_NormalDeath::Update(_float fTimeDelta)
{
}

void CPlayerState_NormalDeath::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_NORMALATTACKED);

	// 조작 가능
	m_pPlayerStat->bInputEnable = true;
	m_pPlayerStat->bDeath = false;
	m_pPlayerStat->bCanHammerAnim = true;
}

CPlayerState_NormalDeath* CPlayerState_NormalDeath::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_NormalDeath* pInstance = new CPlayerState_NormalDeath(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_NormalDeath");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_NormalDeath::Free()
{
	super::Free();
}