#include "pch.h"
#include "Cuttlefish_State_UnaggroedWait.h"
#include "GameInstance.h"
#include "Enemy_Cuttlefish.h"


CCuttlefish_State_UnaggroedWait::CCuttlefish_State_UnaggroedWait()
{
}

HRESULT CCuttlefish_State_UnaggroedWait::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Unaggroed";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.1f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Cuttlefish*>(pObj)->Find_Component(TEXT("Com_Model")));	

	return S_OK;
}

void CCuttlefish_State_UnaggroedWait::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::IDLE_WATITING, false);
	__super::Enter(pObj);
}

void CCuttlefish_State_UnaggroedWait::Update(_float fTimeDelta, void* pObj)
{			
	if (!m_pModel->Get_IsPlaying((_uint)CUTTLEFISH_ANIM_INDEX::IDLE_WATITING)) {
		static_cast<CEnemy_Cuttlefish*>(pObj)->Select_Action();
	}
	__super::Update(fTimeDelta, pObj);
}

void CCuttlefish_State_UnaggroedWait::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CCuttlefish_State_UnaggroedWait* CCuttlefish_State_UnaggroedWait::Create(void* pArg)
{
	CCuttlefish_State_UnaggroedWait* pInstance = new CCuttlefish_State_UnaggroedWait();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_State_UnaggroedWait");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCuttlefish_State_UnaggroedWait::Free()
{
	__super::Free();
}
