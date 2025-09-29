#include "pch.h"
#include "Inkerton_State_Ambush.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "Animation.h"

#include "UI_Manager.h"

CInkerton_State_Ambush::CInkerton_State_Ambush()
{
}

HRESULT CInkerton_State_Ambush::Initialize(void* pObj)
{
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")));	

	return S_OK;
}

void CInkerton_State_Ambush::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::SWIM_FORWRD_AMBUSH, false, 0.f);
	m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::SWIM_FORWRD_AMBUSH;

	CGameInstance::GetInstance()->Play_Sound(L"Scuttle.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	CGameInstance::GetInstance()->Play_Sound(L"Trash Day Inkerton Teleport.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE1, 0.5f);

	CGameInstance::GetInstance()->FadeSwap_Sound(L"inkerton false fight.wav", (_uint)SOUND_CHANNEL::BGM, 0.1f, 0.5f);

	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
	pUIManager->Show_Caption(L"이 곳이 만만해 보였나?", 2.f);

	super::Enter(pObj);
}

void CInkerton_State_Ambush::Update(_float fTimeDelta, void* pObj)
{				
	if (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::SWIM_FORWRD_AMBUSH && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::SWIM_FORWRD_AMBUSH) || (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::SWIM_FORWRD_AMBUSH && m_pModel->Get_CurrAnimation()->Get_Blendable())) {
		static_cast<CBoss_Inkerton*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_Ambush::Exit(void* pObj)
{
	super::Exit(pObj);
}

CInkerton_State_Ambush* CInkerton_State_Ambush::Create(void* pArg)
{
	CInkerton_State_Ambush* pInstance = new CInkerton_State_Ambush();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_Ambush");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_Ambush::Free()
{
	__super::Free();
}
