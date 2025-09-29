#include "pch.h"
#include "Roland_State_Dodge.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"


CRoland_State_Dodge::CRoland_State_Dodge()
{
}

HRESULT CRoland_State_Dodge::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Dodge";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CRoland_State_Dodge::Enter(void* pObj)
{
	_uint iRanNum = (_uint)CUtils::Compute_Random(1.f, 10.f);
	if (iRanNum > 5)
		m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::DODGE_LEFT, false, 0.2f);
	else
		m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::DODGE_RIGHT, false, 0.2f);

	super::Enter(pObj);
}

void CRoland_State_Dodge::Update(_float fTimeDelta, void* pObj)
{
	if (!m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::DODGE_LEFT) && !m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::DODGE_RIGHT)) {
		static_cast<CBoss_Roland*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}


void CRoland_State_Dodge::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CRoland_State_Dodge* CRoland_State_Dodge::Create(void* pArg)
{
	CRoland_State_Dodge* pInstance = new CRoland_State_Dodge();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_Dodge");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_Dodge::Free()
{
	__super::Free();
}
