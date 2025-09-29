#include "pch.h"
#include "Bowman_State_Wait.h"
#include "GameInstance.h"
#include "Enemy_Bowman.h"


CBowman_State_Wait::CBowman_State_Wait()
{
}

HRESULT CBowman_State_Wait::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Wait";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Bowman*>(pObj)->Find_Component(TEXT("Com_Model")));	

	return S_OK;
}

void CBowman_State_Wait::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)BOWMAN_ANIM_INDEX::IDLE, false, 0.2f);
	__super::Enter(pObj);
}

void CBowman_State_Wait::Update(_float fTimeDelta, void* pObj)
{			
	if (!m_pModel->Get_IsPlaying((_uint)BOWMAN_ANIM_INDEX::IDLE)) {
		static_cast<CEnemy_Bowman*>(pObj)->Select_Action();
	}
	__super::Update(fTimeDelta, pObj);
}

void CBowman_State_Wait::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CBowman_State_Wait* CBowman_State_Wait::Create(void* pArg)
{
	CBowman_State_Wait* pInstance = new CBowman_State_Wait();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Walk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBowman_State_Wait::Free()
{
	__super::Free();
}
