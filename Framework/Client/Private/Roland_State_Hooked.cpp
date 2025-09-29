#include "pch.h"
#include "Roland_State_Hooked.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"



CRoland_State_Hooked::CRoland_State_Hooked()
{
}

HRESULT CRoland_State_Hooked::Initialize(void* pObj)
{
	m_pRoland = static_cast<CBoss_Roland*>(pObj);
	m_pModel = static_cast<CModel*>(m_pRoland->Find_Component(TEXT("Com_Model")));

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	m_pTransform = static_cast<CTransform*>(m_pRoland->Find_Component(TEXT("Com_Transform")));

	return S_OK;
}

void CRoland_State_Hooked::Enter(void* pObj)
{		
	m_pRoland->Set_Hooked(true);	
	m_pRoland->Set_IsChase(false);
	m_pRoland->Set_IsSideMove(false);
	m_pRoland->Disable_AllWeaponCollider();
	m_pRoland->Disable_AllWrenches();
	m_pRoland->Disable_RollEffects();

	m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::HOOKED, true, 0.2f);

	_vector temp = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector sour = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	sour = XMVectorSetY(sour, XMVectorGetY(temp));
	m_pTransform->LookAt(sour);

	super::Enter(pObj);
}

void CRoland_State_Hooked::Update(_float fTimeDelta, void* pObj)
{					
	if (!m_pRoland->IsHooked()) {
		static_cast<CBoss_Roland*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_Hooked::Exit(void* pObj)
{		
	super::Exit(pObj);
}

CRoland_State_Hooked* CRoland_State_Hooked::Create(void* pArg)
{
	CRoland_State_Hooked* pInstance = new CRoland_State_Hooked();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_Hooked");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_Hooked::Free()
{
	__super::Free();
}
