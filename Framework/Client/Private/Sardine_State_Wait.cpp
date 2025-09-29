#include "pch.h"
#include "Sardine_State_Wait.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Enemy_Sardine.h"


CSardine_State_Wait::CSardine_State_Wait()
{
}

HRESULT CSardine_State_Wait::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Wait";
#endif

	m_iPriorityLevel = 10;
	m_fMaxCoolTime = 0.f;
	return S_OK;
}

void CSardine_State_Wait::Enter(void* pObj)
{		
	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Sardine*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pModel->SetUp_Animation((_uint)SARDINE_ANIM_INDEX::SWIM_FORWARD, false, 0.01f);
	m_pModel->Get_CurrAnimation()->Set_BaseAnimSpeed(0.7f);

	__super::Enter(pObj);
}

void CSardine_State_Wait::Update(_float fTimeDelta, void* pObj)
{		
	//|| m_pModel->Get_CurrAnimation()->Get_Blendable()

	if (!m_pModel->Get_IsPlaying((_uint)SARDINE_ANIM_INDEX::SWIM_FORWARD)) {
		m_pModel->Get_CurrAnimation()->Set_BaseAnimSpeed(1.f);
		static_cast<CEnemy_Sardine*>(pObj)->Select_Action();
	}
	__super::Update(fTimeDelta, pObj);
}

void CSardine_State_Wait::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CSardine_State_Wait* CSardine_State_Wait::Create(void* pArg)
{
	CSardine_State_Wait* pInstance = new CSardine_State_Wait();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSardine_State_Wait::Free()
{
	__super::Free();
}
