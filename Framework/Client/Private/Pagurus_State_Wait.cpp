#include "pch.h"
#include "Pagurus_State_Wait.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Boss_Pagurus.h"


CPagurus_State_Wait::CPagurus_State_Wait()
{
}

HRESULT CPagurus_State_Wait::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Execution Attack";
#endif

	m_iPriorityLevel = 2;
	m_fMaxCoolTime = 0.5f;

	m_pPagurus = static_cast<CBoss_Pagurus*>(pObj);
	m_pModel = static_cast<CModel*>(m_pPagurus->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CPagurus_State_Wait::Enter(void* pObj)
{
	m_pModel->SetUp_Animation((_uint)PAGURUS_ANIM_INDEX::IDLE, false, 0.3f);
	m_iAnimIndex = (_uint)PAGURUS_ANIM_INDEX::IDLE;
	
	__super::Enter(pObj);
}

void CPagurus_State_Wait::Update(_float fTimeDelta, void* pObj)
{
	if (m_iAnimIndex == (_uint)PAGURUS_ANIM_INDEX::IDLE && !m_pModel->Get_IsPlaying((_uint)PAGURUS_ANIM_INDEX::IDLE)) {
		m_pPagurus->Select_Action();
	}
}

void CPagurus_State_Wait::Exit(void* pObj)
{	
	__super::Exit(pObj);
}

CPagurus_State_Wait* CPagurus_State_Wait::Create(void* pObj)
{
	CPagurus_State_Wait* pInstance = new CPagurus_State_Wait();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CPagurus_State_Wait");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPagurus_State_Wait::Free()
{
	__super::Free();
}