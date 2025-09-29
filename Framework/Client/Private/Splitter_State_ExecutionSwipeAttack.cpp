#include "pch.h"
#include "Splitter_State_ExecutionSwipeAttack.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Enemy_Splitter.h"


CSplitter_State_ExecutionSwipeAttack::CSplitter_State_ExecutionSwipeAttack()
{
}

HRESULT CSplitter_State_ExecutionSwipeAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Execution Swipe Attack";
#endif

	m_iPriorityLevel = 2;
	m_fMaxCoolTime = 60.5f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CSplitter_State_ExecutionSwipeAttack::Enter(void* pObj)
{
	m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::SWIPE_ATTACK, false, 0.3f);
	static_cast<CEnemy_Splitter*>(pObj)->Set_IsGrab(true);

	CGameInstance::GetInstance()->Play_Sound(L"Executioner_OHGrunt1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.7f);
	CGameInstance::GetInstance()->Play_Sound(L"Executioner Grab Swoosh.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE2, 1.f);

	__super::Enter(pObj);
}

void CSplitter_State_ExecutionSwipeAttack::Update(_float fTimeDelta, void* pObj)
{
	if ((m_iAnimIndex == 0 && m_pModel->Get_CurrAnimation()->Get_Blendable())) {
		CEnemy_Splitter* temp = static_cast<CEnemy_Splitter*>(pObj);
		temp->Set_IsGrab(false);
		if (temp->Is_Execution()) {
			m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::EXECUTION, false, 0.3f);
			CGameInstance::GetInstance()->Play_Sound(L"Executioner Grab ReadyUp 2.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK1, 0.7f);
			m_iAnimIndex++;
		}
		else if((m_iAnimIndex == 0 && !m_pModel->Get_IsPlaying((_uint)SPLITTER_ANIM_INDEX::SWIPE_ATTACK)))
			static_cast<CEnemy_Splitter*>(pObj)->Select_Action();
	}
	if (m_iAnimIndex == 1 && !m_pModel->Get_IsPlaying((_uint)SPLITTER_ANIM_INDEX::EXECUTION)) {
		CEnemy_Splitter* temp = static_cast<CEnemy_Splitter*>(pObj);
		temp->Set_Execution(false);
		temp->Select_Action();
	}
}

void CSplitter_State_ExecutionSwipeAttack::Exit(void* pObj)
{
	CEnemy_Splitter* temp = static_cast<CEnemy_Splitter*>(pObj);
	temp->Set_IsGrab(false);
	temp->Set_Execution(false);
	m_iAnimIndex = 0;
	__super::Exit(pObj);
}

CSplitter_State_ExecutionSwipeAttack* CSplitter_State_ExecutionSwipeAttack::Create(void* pObj)
{
	CSplitter_State_ExecutionSwipeAttack* pInstance = new CSplitter_State_ExecutionSwipeAttack();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_ExecutionSwipeAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSplitter_State_ExecutionSwipeAttack::Free()
{
	__super::Free();
}