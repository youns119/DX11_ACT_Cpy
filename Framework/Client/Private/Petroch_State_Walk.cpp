#include "pch.h"
#include "Petroch_State_Walk.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"
#include "Animation.h"


CPetroch_State_Walk::CPetroch_State_Walk()
{
}

HRESULT CPetroch_State_Walk::Initialize(void* pObj)
{
	m_strStateName = "Walk";

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 5.f;

	m_pPetroch = static_cast<CBoss_Petroch*>(pObj);
	m_pTransform = static_cast<CTransform*>(m_pPetroch->Find_Component(TEXT("Com_Transform")));
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CPetroch_State_Walk::Enter(void* pObj)
{
	_uint iRanNum = (_uint)CUtils::Compute_Random(1.f, 5.f);

	m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::WALK_FRONT, false, 0.2f);
	m_iAnimIndex = (_uint)PETROCH_ANIM_INDEX::WALK_FRONT;
			
	m_fWalkDuration = CUtils::Compute_Random(1.f, 5.f);

	super::Enter(pObj);
}

void CPetroch_State_Walk::Update(_float fTimeDelta, void* pObj)
{
	Decrease_Duration(fTimeDelta);

	super::Update(fTimeDelta, pObj);
}

void CPetroch_State_Walk::Exit(void* pObj)
{	
	m_fCurWalkDuration = 0;
	m_pPetroch->Set_IsChase(false);
	m_pPetroch->Set_IsSideMove(false);
	super::Exit(pObj);
}

void CPetroch_State_Walk::Decrease_Duration(_float fTimeDelta)
{
	if (m_fCurWalkDuration >= m_fWalkDuration)
		return;

	m_fCurActDuration += fTimeDelta;
	m_fCurWalkDuration += fTimeDelta;

	if (m_fCurWalkDuration >= m_fWalkDuration) {
		m_pPetroch->Set_IsChase(false);
		m_pPetroch->Set_IsSideMove(false);

		m_pPetroch->Select_Action();
	}
	else if (m_fCurActDuration >= m_fActDuration) {
		m_pPetroch->Set_IsChase(false);
		m_pPetroch->Set_IsSideMove(false);

		Select_Action();
		m_fCurActDuration = 0;
	}
}

void CPetroch_State_Walk::Select_Action()
{
	_int iRand = CUtils::Compute_Random(0, WALK_END);
	switch (iRand)
	{
	case 2:
		m_pModel->SetUp_Animation(WALK_LEFT, true, 0.2f);
		m_iAnimIndex = WALK_LEFT;
		m_pPetroch->Set_IsChase(true, -0.2f);
		m_pPetroch->Set_IsSideMove(false);
		break;
	case 3:
		m_pModel->SetUp_Animation(WALK_RIGHT, true, 0.2f);
		m_iAnimIndex = WALK_RIGHT;
		m_pPetroch->Set_IsChase(false);
		m_pPetroch->Set_IsSideMove(true, 0.2f);
		break;
	default:
		_vector vTemp = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vSour = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
		_float fDest = XMVectorGetX(XMVector4Length(vTemp - vSour));
		if (fDest > 15.f) {
			m_pModel->SetUp_Animation(WALK_FRONT, true, 0.2f);
			m_iAnimIndex = WALK_FRONT;
			m_pPetroch->Set_IsChase(true, 0.3f);
			m_pPetroch->Set_IsSideMove(false);
		}
		else {
			m_pModel->SetUp_Animation(WALK_BACK, true, 0.2f);
			m_iAnimIndex = WALK_BACK;
			m_pPetroch->Set_IsChase(true, -0.2f);
			m_pPetroch->Set_IsSideMove(false);
		}
		break;
	}

	m_fActDuration = CUtils::Compute_Random(1.f, 4.f);
}

CPetroch_State_Walk* CPetroch_State_Walk::Create(void* pArg)
{
	CPetroch_State_Walk* pInstance = new CPetroch_State_Walk();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CPetroch_State_Walk");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CPetroch_State_Walk::Free()
{
	__super::Free();
}
