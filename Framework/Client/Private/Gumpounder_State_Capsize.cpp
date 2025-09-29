#include "pch.h"
#include "Gumpounder_State_Capsize.h"
#include "GameInstance.h"
#include "Enemy_Gumpounder.h"


CGumpounder_State_Capsize::CGumpounder_State_Capsize()
{
}

HRESULT CGumpounder_State_Capsize::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Capsized";

#endif
	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;
	m_pGumpounder = static_cast<CEnemy_Gumpounder*>(pObj);
	m_pModel = static_cast<CModel*>(m_pGumpounder->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CGumpounder_State_Capsize::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)GUMPOUNDER_ANIM_INDEX::TURTLEENTER, true, 0.01f);
	m_pGumpounder->Set_IsChase(false);

	static_cast<CTransform*>(m_pGumpounder->Find_Component(TEXT("Com_Transform")))->Rotate(XMVectorSet(1.f, 1.f, 1.f, 1.f), 90.f);
	__super::Enter(pObj);
}

void CGumpounder_State_Capsize::Update(_float fTimeDelta, void* pObj)
{	
	if (Decrease_CapsizedDuration(fTimeDelta) && !m_pGumpounder->IsFinisher()) {
		m_pGumpounder->Set_Capsized(false);
		m_pGumpounder->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CGumpounder_State_Capsize::Exit(void* pObj)
{
	static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")))->Rotate(XMVectorSet(1.f, 1.f, 1.f, 1.f), 270.f);
	m_fCapsizeTime = 0;
	m_pGumpounder->Set_Finished(false);
	__super::Exit(pObj);
}

_bool CGumpounder_State_Capsize::Decrease_CapsizedDuration(_float fTimeDelta)
{
	if (m_fCapsizeTime > m_fCapsizeDuration)
		return true;
	else
		m_fCapsizeTime += fTimeDelta;

	return false;
}

CGumpounder_State_Capsize* CGumpounder_State_Capsize::Create(void* pArg)
{
	CGumpounder_State_Capsize* pInstance = new CGumpounder_State_Capsize();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGumpounder_State_Capsize::Free()
{
	__super::Free();
}
