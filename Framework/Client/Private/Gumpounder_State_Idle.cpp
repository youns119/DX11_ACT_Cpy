#include "pch.h"
#include "Gumpounder_State_Idle.h"
#include "GameInstance.h"
#include "Enemy_Gumpounder.h"


CGumpounder_State_Idle::CGumpounder_State_Idle()
{
}

HRESULT CGumpounder_State_Idle::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Aggroed";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Gumpounder*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CGumpounder_State_Idle::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)GUMPOUNDER_ANIM_INDEX::IDLE, false);	
	__super::Enter(pObj);
}

void CGumpounder_State_Idle::Update(_float fTimeDelta, void* pObj)          
{	
	if (!m_pModel->Get_IsPlaying((_uint)GUMPOUNDER_ANIM_INDEX::IDLE)) {
		static_cast<CEnemy_Gumpounder*>(pObj)->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CGumpounder_State_Idle::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CGumpounder_State_Idle* CGumpounder_State_Idle::Create(void* pArg)
{
	CGumpounder_State_Idle* pInstance = new CGumpounder_State_Idle();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGumpounder_State_Idle::Free()
{
	__super::Free();	
}