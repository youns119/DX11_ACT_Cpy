#include "pch.h"
#include "PlayerState_Grapple_Shell.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"
#include "VIBuffer_Rope.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Shell_World.h"
#include "IGrapple.h"

CPlayerState_Grapple_Shell::CPlayerState_Grapple_Shell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Grapple_Shell::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("GrabSpear");

	return S_OK;
}

void CPlayerState_Grapple_Shell::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_GRAPPLE_SHELL);

	// 입력 불가능
	// 갈고리 액션 활성화
	// 콤보 카운트 초기화
	m_pPlayerStat->bInputEnable = false;
	m_pPlayerStat->bGrappling = true;
	m_pPlayerStat->iComboCount = 0;
	m_pPlayerStat->bCanHammerAnim = false;

	m_pMovement->Activate(false);

	m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.15f);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength() * 0.8f);

	m_pPlayer->Play_PlayerSound_Loop(L"Grapple_Shell");
}

void CPlayerState_Grapple_Shell::Update(_float fTimeDelta)
{
	if (m_pModel->Get_CurrAnimation()->Get_Blendable())
		m_pModel->Get_CurrAnimation()->Set_Active(false);

	CGameObject* pRope = static_cast<CGameObject*>(m_pPlayer->Find_PartObject({ L"Part_Rope" }));
	IGrapple* pGrapple = dynamic_cast<IGrapple*>(m_pPlayer->Get_GrappleTarget());
	if (pGrapple == nullptr)
		return;

	_vec3 vPos = pGrapple->Get_GrapplePos();

	CVIBuffer_Rope* pRopeBuffer = static_cast<CVIBuffer_Rope*>(pRope->Find_Component(L"Com_VIBuffer"));
	pRopeBuffer->Set_Info(vPos._vector());
}

void CPlayerState_Grapple_Shell::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_GRAPPLE_SHELL);

	// 입력 가능
	// 갈고리 액션 비활성화
	m_pPlayerStat->bInputEnable = true;
	m_pPlayerStat->bGrappling = false;
	m_pPlayerStat->bCanHammerAnim = true;

	m_pMovement->FlyMode(false);
	m_pMovement->Reset_Velocity();
	m_pMovement->Set_ForceFriction(m_pPlayerStat->fForceFriction);

	m_pMovement->Activate(true);

	m_pModel->Get_CurrAnimation()->Set_Active(true);

	m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength());
}

CPlayerState_Grapple_Shell* CPlayerState_Grapple_Shell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Grapple_Shell* pInstance = new CPlayerState_Grapple_Shell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Grapple_Shell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Grapple_Shell::Free()
{
	super::Free();
}