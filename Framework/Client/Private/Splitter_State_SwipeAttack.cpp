#include "pch.h"
#include "Splitter_State_SwipeAttack.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Enemy_Splitter.h"


CSplitter_State_SwipeAttack::CSplitter_State_SwipeAttack()
{
}

HRESULT CSplitter_State_SwipeAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Swipe Attack";
#endif

	m_iPriorityLevel = 2;
	//m_fMaxCoolTime = 1.5f;
	m_fMaxCoolTime = 5.5f;
	//
	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CSplitter_State_SwipeAttack::Enter(void* pObj)
{
	m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::SWIPE_ATTACK, false, 0.3f);
	CGameInstance::GetInstance()->Play_Sound(L"Executioner Grab Swoosh.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, .9f);
	__super::Enter(pObj);
}

void CSplitter_State_SwipeAttack::Update(_float fTimeDelta, void* pObj)
{
	if ((!m_pModel->Get_IsPlaying((_uint)SPLITTER_ANIM_INDEX::SWIPE_ATTACK))) {
		static_cast<CEnemy_Splitter*>(pObj)->Select_Action();
	}
}

void CSplitter_State_SwipeAttack::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CSplitter_State_SwipeAttack* CSplitter_State_SwipeAttack::Create(void* pObj)
{
	CSplitter_State_SwipeAttack* pInstance = new CSplitter_State_SwipeAttack();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_SwipeAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSplitter_State_SwipeAttack::Free()
{
	__super::Free();
}