#include "pch.h"
#include "Normie_State_Idle.h"
#include "GameInstance.h"
#include "Enemy_Normie.h"


CNormie_State_Idle::CNormie_State_Idle()
{
}

HRESULT CNormie_State_Idle::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Aggroed";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Normie*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CNormie_State_Idle::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)NORMIE_ANIM_INDEX::IDLESAND, false);	
	__super::Enter(pObj);
}

void CNormie_State_Idle::Update(_float fTimeDelta, void* pObj)
{	
	if (!m_pModel->Get_IsPlaying((_uint)NORMIE_ANIM_INDEX::IDLESAND)) {
		static_cast<CEnemy_Normie*>(pObj)->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CNormie_State_Idle::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CNormie_State_Idle* CNormie_State_Idle::Create(void* pArg)
{
	CNormie_State_Idle* pInstance = new CNormie_State_Idle();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNormie_State_Idle::Free()
{
	__super::Free();	
}