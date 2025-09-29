#include "pch.h"
#include "Pagurus_State_Execution.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Boss_Pagurus.h"


CPagurus_State_Execution::CPagurus_State_Execution()
{
}

HRESULT CPagurus_State_Execution::Initialize(void* pObj)
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

void CPagurus_State_Execution::Enter(void* pObj)
{
	m_pModel->SetUp_Animation((_uint)PAGURUS_ANIM_INDEX::FORKGRAB, false, 0.3f);
	m_iAnimIndex = (_uint)PAGURUS_ANIM_INDEX::FORKGRAB;	

	m_pPagurus->Set_IsGrab(true);

	__super::Enter(pObj);
}

void CPagurus_State_Execution::Update(_float fTimeDelta, void* pObj)
{
	if ((m_iAnimIndex == (_uint)PAGURUS_ANIM_INDEX::FORKGRAB && m_pModel->Get_CurrAnimation()->Get_Blendable())) {
		m_pPagurus->Set_IsGrab(false);
		if (m_pPagurus->Is_Execution()) {
			m_pModel->SetUp_Animation((_uint)PAGURUS_ANIM_INDEX::FORKGRABSUCCESS, false, 0.2f);
			m_iAnimIndex = (_uint)PAGURUS_ANIM_INDEX::FORKGRABSUCCESS;
		}
		else if (!m_pPagurus->Is_Execution()) {
			m_pPagurus->Select_Action();
		}
	}
	else if (m_iAnimIndex == (_uint)PAGURUS_ANIM_INDEX::FORKGRABSUCCESS && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		m_pModel->SetUp_Animation((_uint)PAGURUS_ANIM_INDEX::EAT, false, 0.2f);
		m_iAnimIndex = (_uint)PAGURUS_ANIM_INDEX::EAT;
		CGameInstance::GetInstance()->Play_Sound(L"Pagurus_Eat.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, .5f);
	}
	else if (m_iAnimIndex == (_uint)PAGURUS_ANIM_INDEX::EAT && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		CGameInstance::GetInstance()->FadeSwap_Sound(L"Field_BGM.wav", (_uint)SOUND_CHANNEL::BGM, 0.1f, .5f);	
	}
}

void CPagurus_State_Execution::Exit(void* pObj)
{	
	m_pPagurus->Set_IsGrab(false);
	m_pPagurus->Set_Execution(false);
	m_iAnimIndex = 0;
	__super::Exit(pObj);
}

CPagurus_State_Execution* CPagurus_State_Execution::Create(void* pObj)
{
	CPagurus_State_Execution* pInstance = new CPagurus_State_Execution();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CPagurus_State_Execution");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPagurus_State_Execution::Free()
{
	__super::Free();
}