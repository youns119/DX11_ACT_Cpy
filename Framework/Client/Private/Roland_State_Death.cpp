#include "pch.h"
#include "Roland_State_Death.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"
#include "CutScene_Manager.h"

CRoland_State_Death::CRoland_State_Death()
{
}

HRESULT CRoland_State_Death::Initialize(void* pObj)
{
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CRoland_State_Death::Enter(void* pObj)
{
	CBoss_Roland* pRoland = static_cast<CBoss_Roland*>(pObj);
	pRoland->Disable_AllWeaponCollider();
	pRoland->Disable_AllWrenches();
	pRoland->Disable_RollEffects();

	pRoland->Set_IsChase(false);
	pRoland->Set_IsSideMove(false);	
	//pRoland->Death();	

	CGameInstance::GetInstance()->Stop_Sound((_uint)SOUND_CHANNEL::ENEMY_ATTACK1);

	CCutScene_Manager* pCutSceneManager = static_cast<CCutScene_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_CutSceneManager", 0));
	pCutSceneManager->Begin_CutScene(L"Roland_Death");		

	super::Enter(pObj);
}

void CRoland_State_Death::Update(_float fTimeDelta, void* pObj)
{	
	super::Update(fTimeDelta, pObj);
}

void CRoland_State_Death::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CRoland_State_Death* CRoland_State_Death::Create(void* pArg)
{
	CRoland_State_Death* pInstance = new CRoland_State_Death();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_Death");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_Death::Free()
{
	__super::Free();
}
