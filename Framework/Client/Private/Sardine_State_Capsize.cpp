#include "pch.h"
#include "Sardine_State_Capsize.h"
#include "GameInstance.h"
#include "Enemy_Sardine.h"


CSardine_State_Capsize::CSardine_State_Capsize()
{
}

HRESULT CSardine_State_Capsize::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Capsized";

#endif
	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;
	m_pSardine = static_cast<CEnemy_Sardine*>(pObj);
	m_pModel = static_cast<CModel*>(m_pSardine->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CSardine_State_Capsize::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)SARDINE_ANIM_INDEX::HOOKED, true, 0.001f);
	m_pModel->SetUp_Animation((_uint)SARDINE_ANIM_INDEX::SWIM_FORWARD, true, 0.01f);
	m_pSardine->Set_IsChase(false);

	static_cast<CTransform*>(m_pSardine->Find_Component(TEXT("Com_Transform")))->Rotate(XMVectorSet(1.f, 1.f, 1.f, 1.f), 90.f);
	__super::Enter(pObj);
}

void CSardine_State_Capsize::Update(_float fTimeDelta, void* pObj)
{	
	if (Decrease_CapsizedDuration(fTimeDelta) && !m_pSardine->IsFinisher()) {
		m_pSardine->Set_Capsized(false);
		m_pSardine->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CSardine_State_Capsize::Exit(void* pObj)
{
	static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")))->Rotate(XMVectorSet(1.f, 1.f, 1.f, 1.f), 270.f);
	m_fCapsizeTime = 0;
	m_pSardine->Set_Finished(false);
	__super::Exit(pObj);
}

_bool CSardine_State_Capsize::Decrease_CapsizedDuration(_float fTimeDelta)
{
	if (m_fCapsizeTime > m_fCapsizeDuration)
		return true;
	else
		m_fCapsizeTime += fTimeDelta;

	return false;
}

CSardine_State_Capsize* CSardine_State_Capsize::Create(void* pArg)
{
	CSardine_State_Capsize* pInstance = new CSardine_State_Capsize();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSardine_State_Capsize::Free()
{
	__super::Free();
}
