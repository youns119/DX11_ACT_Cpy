#include "pch.h"
#include "Petroch_State_BlockWalk.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"
#include "Animation.h"


CPetroch_State_BlockWalk::CPetroch_State_BlockWalk()
{
}

HRESULT CPetroch_State_BlockWalk::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "BlockWalk";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CPetroch_State_BlockWalk::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::IDLE, false, 0.2f);
	m_iAnimIndex = (_uint)PETROCH_ANIM_INDEX::IDLE;
	super::Enter(pObj);
}

void CPetroch_State_BlockWalk::Update(_float fTimeDelta, void* pObj)
{
	if (m_iAnimIndex == (_uint)PETROCH_ANIM_INDEX::IDLE && !m_pModel->Get_IsPlaying((_uint)PETROCH_ANIM_INDEX::IDLE)) {
		static_cast<CBoss_Petroch*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}


void CPetroch_State_BlockWalk::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CPetroch_State_BlockWalk* CPetroch_State_BlockWalk::Create(void* pArg)
{
	CPetroch_State_BlockWalk* pInstance = new CPetroch_State_BlockWalk();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CPetroch_State_BlockWalk");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CPetroch_State_BlockWalk::Free()
{
	__super::Free();
}
