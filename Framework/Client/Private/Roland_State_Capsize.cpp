#include "pch.h"
#include "Roland_State_Capsize.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"

#include "UI_Manager.h"

CRoland_State_Capsize::CRoland_State_Capsize()
{
}

HRESULT CRoland_State_Capsize::Initialize(void* pObj)
{
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CRoland_State_Capsize::Enter(void* pObj)
{	
	m_pRoland = static_cast<CBoss_Roland*>(pObj);
	m_pRoland->Set_IsChase(false);
	m_pRoland->Set_IsSideMove(false);
	m_pRoland->Disable_AllWeaponCollider();
	m_pRoland->Disable_AllWrenches();
	m_pRoland->Disable_RollEffects();

	m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::STUNNED_UNROLL, false, 0.2f);
	m_iAnimIndex = (_uint)ROLAND_ANIM_INDEX::STUNNED_UNROLL;

	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
	pUIManager->Hide_Caption();

	CGameInstance::GetInstance()->Play_Sound(L"Roland_Banter_Efforts - Trimmed (4).wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 1.f);
	
	pUIManager->Show_Caption(L"ÀÌ·± Á¨Àå", 2.f);

	super::Enter(pObj);
}

void CRoland_State_Capsize::Update(_float fTimeDelta, void* pObj)
{				
	if (m_iAnimIndex == (_uint)ROLAND_ANIM_INDEX::STUNNED_UNROLL && !m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::STUNNED_UNROLL)) {
		m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::STUNNED_LOOP, true, 0.2f);
		m_iAnimIndex = (_uint)ROLAND_ANIM_INDEX::STUNNED_LOOP;
	}
	else if (m_iAnimIndex == (_uint)ROLAND_ANIM_INDEX::STUNNED_LOOP && Decrease_CapsizedDuration(fTimeDelta) && !m_pRoland->IsFinisher()) {
		m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::STUNNED_EXIT, false, 0.2f);
		m_iAnimIndex = (_uint)ROLAND_ANIM_INDEX::STUNNED_EXIT;
	}
	else if (m_iAnimIndex == (_uint)ROLAND_ANIM_INDEX::STUNNED_EXIT && !m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::STUNNED_EXIT)) {
		m_pRoland->Set_Capsized(false);
		m_pRoland->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_Capsize::Exit(void* pObj)
{	
	m_pRoland->Set_Finished(false);
	m_fCapsizeTime = 0;
	super::Exit(pObj);
}

_bool CRoland_State_Capsize::Decrease_CapsizedDuration(_float fTimeDelta)
{
	if (m_fCapsizeTime > m_fCapsizeDuration)
		return true;
	else
		m_fCapsizeTime += fTimeDelta;

	return false;
}


CRoland_State_Capsize* CRoland_State_Capsize::Create(void* pArg)
{
	CRoland_State_Capsize* pInstance = new CRoland_State_Capsize();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_Capsize");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_Capsize::Free()
{
	__super::Free();
}
