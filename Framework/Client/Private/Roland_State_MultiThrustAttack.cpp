#include "pch.h"
#include "Roland_State_MultiThrustAttack.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"


CRoland_State_MultiThrustAttack::CRoland_State_MultiThrustAttack()
{
}

HRESULT CRoland_State_MultiThrustAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Multi-Thrust Attack";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CRoland_State_MultiThrustAttack::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::THRUSTS_MULTI, false, 0.2f);

	super::Enter(pObj);
}

void CRoland_State_MultiThrustAttack::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::THRUSTS_MULTI) || m_iAnimIndex == 0 && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		static_cast<CBoss_Roland*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_MultiThrustAttack::Exit(void* pObj)
{	
	m_iAnimIndex = 0;
	super::Exit(pObj);
}

CRoland_State_MultiThrustAttack* CRoland_State_MultiThrustAttack::Create(void* pArg)
{
	CRoland_State_MultiThrustAttack* pInstance = new CRoland_State_MultiThrustAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_MultiThrustAttack");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_MultiThrustAttack::Free()
{
	__super::Free();
}
