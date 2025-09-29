#include "pch.h"
#include "Roland_State_Stagger.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"



CRoland_State_Stagger::CRoland_State_Stagger()
{
}

HRESULT CRoland_State_Stagger::Initialize(void* pObj)
{
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CRoland_State_Stagger::Enter(void* pObj)
{		
	CBoss_Roland* pRoland = static_cast<CBoss_Roland*>(pObj);
	pRoland->Set_IsChase(false);
	pRoland->Set_IsSideMove(false);
	pRoland->Disable_AllWeaponCollider();
	pRoland->Disable_AllWrenches();	


	CGameInstance::GetInstance()->Play_Sound(L"Roland_Banter_Efforts_2 - Trimmed.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 1.f);
	m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::ITERRUPTED, false, 0.2f);
	super::Enter(pObj);
}

void CRoland_State_Stagger::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::ITERRUPTED) || m_iAnimIndex == 0 && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		static_cast<CBoss_Roland*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_Stagger::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CRoland_State_Stagger* CRoland_State_Stagger::Create(void* pArg)
{
	CRoland_State_Stagger* pInstance = new CRoland_State_Stagger();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_Stagger");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_Stagger::Free()
{
	__super::Free();
}
