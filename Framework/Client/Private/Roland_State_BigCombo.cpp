#include "pch.h"
#include "Roland_State_BigCombo.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"

#include "UI_Manager.h"


CRoland_State_BigCombo::CRoland_State_BigCombo()
{
}

HRESULT CRoland_State_BigCombo::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Big Combo Attack";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CRoland_State_BigCombo::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::BIG_COMBO_REVEALWEAPONS, false, 0.2f);

	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	pUIManager->Hide_Caption();

	CGameInstance::GetInstance()->Play_Sound(L"Roland_6wrench.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK1, 1.f);
	CGameInstance::GetInstance()->Play_Sound(L"Roland-BigCombo.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 1.f);
	
	pUIManager->Show_Caption(L"그래. 이게 우리 본능이지! 너도 나도 짐승이야. 살육을 위해 태어난!", 8.f);

	super::Enter(pObj);
}

void CRoland_State_BigCombo::Update(_float fTimeDelta, void* pObj)
{				
	if (m_iAnimIndex == 0 && !m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::BIG_COMBO_REVEALWEAPONS)) {
		m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::BIG_COMBO_ATTACK, false, 0.2f);
		m_iAnimIndex++;		
	}
	else if (m_iAnimIndex == 1 && !m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::BIG_COMBO_ATTACK)) {		
		static_cast<CBoss_Roland*>(pObj)->Select_Action();
	}	

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_BigCombo::Exit(void* pObj)
{	
	m_iAnimIndex = 0;
	super::Exit(pObj);
}

CRoland_State_BigCombo* CRoland_State_BigCombo::Create(void* pArg)
{
	CRoland_State_BigCombo* pInstance = new CRoland_State_BigCombo();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_BigCombo");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_BigCombo::Free()
{
	__super::Free();
}
