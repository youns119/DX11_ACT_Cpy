#include "pch.h"
#include "Petroch_State_Pounce.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"
#include "Animation.h"


CPetroch_State_Pounce::CPetroch_State_Pounce()
{
}

HRESULT CPetroch_State_Pounce::Initialize(void* pObj)
{
	m_strStateName = "Pounce";

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 5.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CPetroch_State_Pounce::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::POUNCE, false, 0.2f);
	m_iAnimIndex = (_uint)PETROCH_ANIM_INDEX::POUNCE;
	super::Enter(pObj);
}

void CPetroch_State_Pounce::Update(_float fTimeDelta, void* pObj)
{
	if (m_iAnimIndex == (_uint)PETROCH_ANIM_INDEX::POUNCE && !m_pModel->Get_IsPlaying((_uint)PETROCH_ANIM_INDEX::POUNCE)) {
		static_cast<CBoss_Petroch*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}


void CPetroch_State_Pounce::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CPetroch_State_Pounce* CPetroch_State_Pounce::Create(void* pArg)
{
	CPetroch_State_Pounce* pInstance = new CPetroch_State_Pounce();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CPetroch_State_Pounce");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CPetroch_State_Pounce::Free()
{
	__super::Free();
}
