#include "pch.h"
#include "Splitter_State_Wait.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"


CSplitter_State_Wait::CSplitter_State_Wait()
{
}

HRESULT CSplitter_State_Wait::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Wait";
#endif

	m_iPriorityLevel = 5;
	m_fMaxCoolTime = 0.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Model")));


	return S_OK;
}	

void CSplitter_State_Wait::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::GUILLOTINE_UP_IDLE, true, 0.3f);
	__super::Enter(pObj);
}

void CSplitter_State_Wait::Update(_float fTimeDelta, void* pObj)
{
	if (!m_pModel->Get_IsPlaying((_uint)SPLITTER_ANIM_INDEX::GUILLOTINE_UP_IDLE)) {
		static_cast<CEnemy_Splitter*>(pObj)->Select_Action();
	}
}

void CSplitter_State_Wait::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CSplitter_State_Wait* CSplitter_State_Wait::Create(void* pObj)
{
	CSplitter_State_Wait* pInstance = new CSplitter_State_Wait();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Wait");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSplitter_State_Wait::Free()
{
	__super::Free();
}
