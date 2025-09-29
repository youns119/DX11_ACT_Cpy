#include "pch.h"
#include "Roland_State_Wait.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"


CRoland_State_Wait::CRoland_State_Wait()
{
}

HRESULT CRoland_State_Wait::Initialize(void* pObj)
{
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CRoland_State_Wait::Enter(void* pObj)
{	
	m_bIsAggroed = static_cast<CEnemy*>(pObj)->IsAggroed();
	m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::IDLE, false, 0.2f);

	super::Enter(pObj);
}

void CRoland_State_Wait::Update(_float fTimeDelta, void* pObj)
{				
	if (m_bIsAggroed)
		static_cast<CBoss_Roland*>(pObj)->Set_IsChase(true, 0);	

	if (!m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::IDLE) || m_iAnimIndex == 0 && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		static_cast<CBoss_Roland*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_Wait::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CRoland_State_Wait* CRoland_State_Wait::Create(void* pArg)
{
	CRoland_State_Wait* pInstance = new CRoland_State_Wait();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_Wait");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_Wait::Free()
{
	__super::Free();
}
