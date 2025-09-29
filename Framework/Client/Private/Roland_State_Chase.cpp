#include "pch.h"
#include "Roland_State_Chase.h"
#include "GameInstance.h"
#include "Boss_Roland.h"

//Engine
#include "Animation.h"
#include "Transform.h"
#include "Movement.h"


CRoland_State_Chase::CRoland_State_Chase()
{
}

HRESULT CRoland_State_Chase::Initialize(void* pObj)
{
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;

#ifdef _DEBUG
	m_strStateName = "Chase";
#endif
	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")));

}

void CRoland_State_Chase::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::WALK_FORWARD, true, 0.2f);

	super::Enter(pObj);
}

void CRoland_State_Chase::Update(_float fTimeDelta, void* pObj)
{				
	_vector vLookTarget = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	m_pMovement->TurnToDirection(vLookTarget, fTimeDelta * 0.5f);
	m_pTransform->Go_Straight(fTimeDelta * 0.6f);

	_vector vDis = vLookTarget - m_pTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = XMVectorGetX(XMVector3Length(vDis));

	if (fDis > 25) {
		static_cast<CBoss_Roland*>(pObj)->Select_Action();
	}
	if (fDis < 8) {
		static_cast<CBoss_Roland*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_Chase::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CRoland_State_Chase* CRoland_State_Chase::Create(void* pArg)
{
	CRoland_State_Chase* pInstance = new CRoland_State_Chase();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_Chase");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_Chase::Free()
{
	__super::Free();
}
