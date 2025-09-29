#include "pch.h"
#include "Roland_State_Aggroed.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"


CRoland_State_Aggroed::CRoland_State_Aggroed()
{
}

HRESULT CRoland_State_Aggroed::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Aggroed";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CRoland_State_Aggroed::Enter(void* pObj)
{
	m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::IDLE, false, 0.2f);
	static_cast<CBoss_Roland*>(pObj)->Set_IsChase(true, 0.f);
	super::Enter(pObj);
}

void CRoland_State_Aggroed::Update(_float fTimeDelta, void* pObj)
{
	if (!m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::IDLE)) {
		CBoss_Roland* pInkerton = static_cast<CBoss_Roland*>(pObj);
		pInkerton->Set_IsChase(false);
		pInkerton->Set_Aggroed(true);
		pInkerton->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_Aggroed::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CRoland_State_Aggroed* CRoland_State_Aggroed::Create(void* pArg)
{
	CRoland_State_Aggroed* pInstance = new CRoland_State_Aggroed();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_Aggroed");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_Aggroed::Free()
{
	__super::Free();
}
