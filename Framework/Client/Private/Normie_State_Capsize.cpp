#include "pch.h"
#include "Normie_State_Capsize.h"
#include "GameInstance.h"
#include "Enemy_Normie.h"


CNormie_State_Capsize::CNormie_State_Capsize()
{
}

HRESULT CNormie_State_Capsize::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Capsized";

#endif
	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;
	m_pNormie = static_cast<CEnemy_Normie*>(pObj);
	m_pModel = static_cast<CModel*>(m_pNormie->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CNormie_State_Capsize::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)NORMIE_ANIM_INDEX::FLIP, true, 0.01f);
	m_pNormie->Set_IsChase(false);
	
	__super::Enter(pObj);
}

void CNormie_State_Capsize::Update(_float fTimeDelta, void* pObj)
{	
	if (Decrease_CapsizedDuration(fTimeDelta) && !m_pNormie->IsFinisher()) {
		m_pNormie->Set_Capsized(false);
		m_pNormie->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CNormie_State_Capsize::Exit(void* pObj)
{	
	m_fCapsizeTime = 0;
	m_pNormie->Set_Finished(false);
	__super::Exit(pObj);
}

_bool CNormie_State_Capsize::Decrease_CapsizedDuration(_float fTimeDelta)
{
	if (m_fCapsizeTime > m_fCapsizeDuration)
		return true;
	else
		m_fCapsizeTime += fTimeDelta;

	return false;
}

CNormie_State_Capsize* CNormie_State_Capsize::Create(void* pArg)
{
	CNormie_State_Capsize* pInstance = new CNormie_State_Capsize();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNormie_State_Capsize::Free()
{
	__super::Free();
}
