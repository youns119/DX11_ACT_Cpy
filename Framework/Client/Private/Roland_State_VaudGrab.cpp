#include "pch.h"
#include "Roland_State_VaudGrab.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"


CRoland_State_VaudGrab::CRoland_State_VaudGrab()
{
}

HRESULT CRoland_State_VaudGrab::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Vaud Grab";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CRoland_State_VaudGrab::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::VAUD_GRAB, false, 0.2f);

	super::Enter(pObj);
}

void CRoland_State_VaudGrab::Update(_float fTimeDelta, void* pObj)
{
	if (!m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::VAUD_GRAB) || m_iAnimIndex == 0 && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		static_cast<CBoss_Roland*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_VaudGrab::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CRoland_State_VaudGrab* CRoland_State_VaudGrab::Create(void* pArg)
{
	CRoland_State_VaudGrab* pInstance = new CRoland_State_VaudGrab();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_VaudGrab");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_VaudGrab::Free()
{
	__super::Free();
}
