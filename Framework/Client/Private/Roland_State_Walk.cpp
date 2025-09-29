#include "pch.h"
#include "Roland_State_Walk.h"
#include "GameInstance.h"
#include "Boss_Roland.h"

//Enigne
#include "Animation.h"
#include "Transform.h"
#include "Movement.h"

CRoland_State_Walk::CRoland_State_Walk()
{
}

HRESULT CRoland_State_Walk::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Walk";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 10.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")));

	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));
	return S_OK;
}

void CRoland_State_Walk::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::WALK_FORWARD, false, 0.2f);
	static_cast<CBoss_Roland*>(pObj)->Set_IsChase(true, 0.5f);
	super::Enter(pObj);
}

void CRoland_State_Walk::Update(_float fTimeDelta, void* pObj)
{				
	_vector vLookTarget = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	m_pMovement->TurnToDirection(vLookTarget, fTimeDelta * 0.5f);
	m_pTransform->Go_Straight(fTimeDelta * 0.8f);

	_vector vDis = vLookTarget - m_pTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = XMVectorGetX(XMVector3Length(vDis));

	if (fDis > 15) {
		static_cast<CBoss_Roland*>(pObj)->Select_Action();
	}
	if (fDis < 6) {
		static_cast<CBoss_Roland*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_Walk::Exit(void* pObj)
{	
	static_cast<CBoss_Roland*>(pObj)->Set_IsChase(false);
	super::Exit(pObj);
}

CRoland_State_Walk* CRoland_State_Walk::Create(void* pArg)
{
	CRoland_State_Walk* pInstance = new CRoland_State_Walk();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_Walk");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_Walk::Free()
{
	__super::Free();
}
