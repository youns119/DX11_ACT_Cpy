#include "pch.h"
#include "Petroch_State_Block.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"
#include "Animation.h"


CPetroch_State_Block::CPetroch_State_Block()
{
}

HRESULT CPetroch_State_Block::Initialize(void* pObj)
{
	m_strStateName = "Block";

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pPetroch = static_cast<CBoss_Petroch*>(pObj);
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));		
	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));
	return S_OK;
}

void CPetroch_State_Block::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation(BLOCK_ENTER, false, 0.2f);
	m_iAnimIndex = BLOCK_ENTER;
	m_fBlockDuration = CUtils::Compute_Random(1.f, 5.f);
	super::Enter(pObj);
}

void CPetroch_State_Block::Update(_float fTimeDelta, void* pObj)
{
	m_pPetroch->Set_Block(true);
	_vector vLookTarget = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	m_pMovement->TurnToDirection(vLookTarget, fTimeDelta);

	if (m_iAnimIndex == BLOCK_ENTER && !m_pModel->Get_IsPlaying(BLOCK_ENTER)) {
		m_pPetroch->Set_IsChase(false);
		m_pPetroch->Set_IsSideMove(false);

		Select_Action();
	}
	else if (m_iAnimIndex == BLOCK_EXIT && !m_pModel->Get_IsPlaying(BLOCK_EXIT)) {
		m_pPetroch->Select_Action();
	}
	
	Decrease_Duration(fTimeDelta);

	super::Update(fTimeDelta, pObj);
}

void CPetroch_State_Block::Exit(void* pObj)
{	
	m_fCurBlockDuration = 0;	
	super::Exit(pObj);
}

void CPetroch_State_Block::Decrease_Duration(_float fTimeDelta)
{
	if (m_fCurBlockDuration >= m_fBlockDuration) 
		return;
	
	m_fCurActDuration += fTimeDelta;
	m_fCurBlockDuration += fTimeDelta;

	if (m_fCurBlockDuration >= m_fBlockDuration) {
		m_pModel->SetUp_Animation(BLOCK_EXIT, false, 0.2f);
		m_iAnimIndex = BLOCK_EXIT;		
	}
	else if (m_fCurActDuration >= m_fActDuration) {
		Select_Action();
		m_fCurActDuration = 0;
	}
}

void CPetroch_State_Block::Select_Action()
{
	_int iRand = CUtils::Compute_Random(0, BLOCK_END);
	switch (iRand)
	{
	case 0:
		m_pModel->SetUp_Animation(BLOCK_IDLE, true, 0.2f);
		m_iAnimIndex = BLOCK_IDLE;
		m_pPetroch->Set_IsChase(false);
		m_pPetroch->Set_IsSideMove(false);
		break;
	case 1:
		m_pModel->SetUp_Animation(BLOCK_FORWARD, true, 0.2f);
		m_iAnimIndex = BLOCK_FORWARD;
		m_pPetroch->Set_IsChase(true, 0.1f);
		m_pPetroch->Set_IsSideMove(false);
		break;
	case 2:
		m_pModel->SetUp_Animation(BLOCK_LEFT, true, 0.2f);
		m_iAnimIndex = BLOCK_LEFT;		
		m_pPetroch->Set_IsChase(false);
		m_pPetroch->Set_IsSideMove(true, 0.1f);
		break;
	case 3:
		m_pModel->SetUp_Animation(BLOCK_RIGHT, true, 0.2f);
		m_iAnimIndex = BLOCK_RIGHT;
		m_pPetroch->Set_IsChase(false);
		m_pPetroch->Set_IsSideMove(true, -0.1f);
		break;
	default:
		break;
	}
	
	m_fActDuration = CUtils::Compute_Random(1.f, 3.f);
}

CPetroch_State_Block* CPetroch_State_Block::Create(void* pArg)
{
	CPetroch_State_Block* pInstance = new CPetroch_State_Block();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CPetroch_State_Block");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CPetroch_State_Block::Free()
{
	__super::Free();
}
