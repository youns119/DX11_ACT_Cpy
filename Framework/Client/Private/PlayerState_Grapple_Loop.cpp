#include "pch.h"
#include "PlayerState_Grapple_Loop.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_Grapple_Loop::CPlayerState_Grapple_Loop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Grapple_Loop::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex[static_cast<_uint>(LOOP::LOOP_GRAPPLING)] = m_pModel->Get_AnimationIndex("Grappling");
	m_iAnimIndex[static_cast<_uint>(LOOP::LOOP_GRAB)] = m_pModel->Get_AnimationIndex("GrabSpear");
	m_iAnimIndex[static_cast<_uint>(LOOP::LOOP_FALLING)] = m_pModel->Get_AnimationIndex("AirboneFalling");

	return S_OK;
}

void CPlayerState_Grapple_Loop::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_GRAPPLE_LOOP);

	m_pPlayer->Reset_PlayerStat();

	// 입력 불가능
	// 갈고리 액션 활성화
	// 콤보 카운트 초기화
	m_pPlayerStat->bInputEnable = false;
	m_pPlayerStat->bGrappling = true;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	m_eLoop = LOOP::LOOP_GRAPPLING;

	m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eLoop)], false, 0.15f);

	m_pPlayer->Play_PlayerSound(L"Grapple_Loop");
}

void CPlayerState_Grapple_Loop::Update(_float fTimeDelta)
{
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_eLoop == LOOP::LOOP_GRAPPLING)
		{
			m_eLoop = LOOP::LOOP_GRAB;
			m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eLoop)], false, 0.15f);

			m_pPlayer->Grapple_Launch();
		}
		else if (m_eLoop == LOOP::LOOP_GRAB)
		{
			m_eLoop = LOOP::LOOP_FALLING;
			m_pModel->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eLoop)], false, 0.15f);
		}
	}
}

void CPlayerState_Grapple_Loop::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_GRAPPLE_LOOP);

	// 입력 가능
	// 갈고리 액션 비활성화
	m_pPlayerStat->bInputEnable = true;
	m_pPlayerStat->bGrappling = false;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pMovement->FlyMode(false);
	m_pMovement->Reset_Velocity();
	m_pMovement->Set_ForceFriction(m_pPlayerStat->fForceFriction);

	m_eLoop = LOOP::LOOP_END;
}

CPlayerState_Grapple_Loop* CPlayerState_Grapple_Loop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Grapple_Loop* pInstance = new CPlayerState_Grapple_Loop(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Grapple_Loop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Grapple_Loop::Free()
{
	super::Free();
}