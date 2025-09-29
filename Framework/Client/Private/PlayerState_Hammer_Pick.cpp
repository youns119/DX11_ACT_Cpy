#include "pch.h"
#include "PlayerState_Hammer_Pick.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "SpringArm_Player.h"

CPlayerState_Hammer_Pick::CPlayerState_Hammer_Pick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Hammer_Pick::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("EquipHammer");

	return S_OK;
}

void CPlayerState_Hammer_Pick::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_HAMMER_PICK);

	m_pPlayer->Reset_PlayerStat();
	m_pMovement->Reset_Velocity();

	// 입력 불가능
	// 해머가능 비활성화
	// 해머 활성화
	// 콤보 카운트 초기화
	m_pPlayerStat->bInputEnable = false;
	m_pPlayerStat->bCanBeHammer = false;
	m_pPlayerStat->bHasHammer = true;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	_vec3 vLook = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::LOOK);

	m_pMovement->Set_Decel(m_pPlayerStat->fDecel);
	m_pMovement->Add_Force(vLook._vector(), 3.f);

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.2f);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength() * 0.8f);
}

void CPlayerState_Hammer_Pick::Update(_float fTimeDelta)
{
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_pPlayerStat->bLockOn)
			m_pOwner->ChangeState(L"LockOn");
		else m_pOwner->ChangeState(L"Idle");
	}
}

void CPlayerState_Hammer_Pick::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_HAMMER_PICK);

	// 입력 가능
	m_pPlayerStat->bInputEnable = true;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pMovement->Set_Decel(m_pPlayerStat->fDecel);

	m_pModel->Activate_SlotAnim(ANIMSLOT::SLOT_HAMMER, true);
	m_pModel->SetUp_SlotAnimation(ANIMSLOT::SLOT_HAMMER, m_pModel->Get_AnimationIndex("HammerIdle"), true, 0.1f);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength());
}

CPlayerState_Hammer_Pick* CPlayerState_Hammer_Pick::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Hammer_Pick* pInstance = new CPlayerState_Hammer_Pick(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Hammer_Pick");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Hammer_Pick::Free()
{
	super::Free();
}