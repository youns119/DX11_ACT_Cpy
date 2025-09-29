#include "pch.h"
#include "Roland_State_SideWalk.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"


CRoland_State_SideWalk::CRoland_State_SideWalk()
{
}

HRESULT CRoland_State_SideWalk::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Side Walk";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CRoland_State_SideWalk::Enter(void* pObj)
{		
	_uint iRanNum = (_uint)CUtils::Compute_Random(0.f, 10.f);
	if (iRanNum > 5) {
		m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::WALK_LEFT, false, 0.2f);
		static_cast<CBoss_Roland*>(pObj)->Set_IsSideMove(true, -0.5f);
	}		
	else {
		m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::WALK_RIGHT, false, 0.2f);
		static_cast<CBoss_Roland*>(pObj)->Set_IsSideMove(true, 0.5f);
	}
		
	super::Enter(pObj);
}

void CRoland_State_SideWalk::Update(_float fTimeDelta, void* pObj)
{					
	if (!m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::WALK_LEFT) && !m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::WALK_RIGHT)) {
		static_cast<CBoss_Roland*>(pObj)->Set_IsSideMove(false);
		static_cast<CBoss_Roland*>(pObj)->Select_Action();		
	}

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_SideWalk::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CRoland_State_SideWalk* CRoland_State_SideWalk::Create(void* pArg)
{
	CRoland_State_SideWalk* pInstance = new CRoland_State_SideWalk();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_SideWalk");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_SideWalk::Free()
{
	__super::Free();
}
