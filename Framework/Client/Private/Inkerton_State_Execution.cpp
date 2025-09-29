#include "pch.h"
#include "Inkerton_State_Execution.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Boss_Inkerton.h"


CInkerton_State_Execution::CInkerton_State_Execution()
{
}

HRESULT CInkerton_State_Execution::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Execution Attack";
#endif

	m_iPriorityLevel = 2;
	m_fMaxCoolTime = 40.5f;

	m_pInkerton = static_cast<CBoss_Inkerton*>(pObj);
	m_pModel = static_cast<CModel*>(m_pInkerton->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CInkerton_State_Execution::Enter(void* pObj)
{
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::GRAB_SUCCESS, false, 0.3f);
	m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::GRAB_SUCCESS;

	m_pInkerton->Set_IsGrab(true);

	__super::Enter(pObj);
}

void CInkerton_State_Execution::Update(_float fTimeDelta, void* pObj)
{
	if ((m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::GRAB_SUCCESS && m_pModel->Get_CurrAnimation()->Get_Blendable())) {
		m_pInkerton->Set_IsGrab(false);
		if (m_pInkerton->Is_Execution() && (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::GRAB_SUCCESS && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::GRAB_SUCCESS))) {
			m_pInkerton->Select_Action();
		}
		else if (!m_pInkerton->Is_Execution()) {
			m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::GRAB_ATTEMPT, false, 0.1f);
			m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::GRAB_ATTEMPT;
		}
	}
	else if (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::GRAB_ATTEMPT && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::GRAB_ATTEMPT) && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		m_pInkerton->Select_Action();
	}
}

void CInkerton_State_Execution::Exit(void* pObj)
{	
	m_pInkerton->Set_IsGrab(false);
	m_pInkerton->Set_Execution(false);
	m_iAnimIndex = 0;
	__super::Exit(pObj);
}

CInkerton_State_Execution* CInkerton_State_Execution::Create(void* pObj)
{
	CInkerton_State_Execution* pInstance = new CInkerton_State_Execution();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_Execution");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_Execution::Free()
{
	__super::Free();
}