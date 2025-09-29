#include "pch.h"
#include "Roland_State_SingleThrustAttack.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"


CRoland_State_SingleThrustAttack::CRoland_State_SingleThrustAttack()
{
}

HRESULT CRoland_State_SingleThrustAttack::Initialize(void* pObj)
{

#ifdef _DEBUG
	m_strStateName = "Single Thrust Attack";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));
	return S_OK;
}

void CRoland_State_SingleThrustAttack::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::SINGLE_THRUST, false, 0.2f);

	super::Enter(pObj);
}

void CRoland_State_SingleThrustAttack::Update(_float fTimeDelta, void* pObj)
{					
	if (!m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::SINGLE_THRUST) || m_iAnimIndex == 0 && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		static_cast<CBoss_Roland*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_SingleThrustAttack::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CRoland_State_SingleThrustAttack* CRoland_State_SingleThrustAttack::Create(void* pArg)
{
	CRoland_State_SingleThrustAttack* pInstance = new CRoland_State_SingleThrustAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_SingleThrustAttack");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_SingleThrustAttack::Free()
{
	__super::Free();
}
