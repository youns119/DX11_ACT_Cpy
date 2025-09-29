#include "pch.h"
#include "Roland_State_Unroll.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"


CRoland_State_Unroll::CRoland_State_Unroll()
{
}

HRESULT CRoland_State_Unroll::Initialize(void* pObj)
{
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CRoland_State_Unroll::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::UNROLL, false, 0.2f);

	super::Enter(pObj);
}

void CRoland_State_Unroll::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::UNROLL) || m_iAnimIndex == 0 && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		static_cast<CBoss_Roland*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_Unroll::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CRoland_State_Unroll* CRoland_State_Unroll::Create(void* pArg)
{
	CRoland_State_Unroll* pInstance = new CRoland_State_Unroll();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_Unroll");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_Unroll::Free()
{
	__super::Free();
}
