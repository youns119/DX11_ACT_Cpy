#include "pch.h"
#include "Petroch_State_Retreat.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"
#include "Animation.h"


CPetroch_State_Retreat::CPetroch_State_Retreat()
{
}

HRESULT CPetroch_State_Retreat::Initialize(void* pObj)
{
	m_strStateName = "Retreat";

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 5.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CPetroch_State_Retreat::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::RETREAT, false, 0.2f);
	m_iAnimIndex = (_uint)PETROCH_ANIM_INDEX::RETREAT;

	CGameInstance::GetInstance()->Play_Sound(L"Heikea Jump Impact.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE2, 0.7f);
	CGameInstance::GetInstance()->Play_Sound(L"PetrochVO_LookOut1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 1.f);

	super::Enter(pObj);
}

void CPetroch_State_Retreat::Update(_float fTimeDelta, void* pObj)
{
	if (m_iAnimIndex == (_uint)PETROCH_ANIM_INDEX::RETREAT && !m_pModel->Get_IsPlaying((_uint)PETROCH_ANIM_INDEX::RETREAT)) {
		static_cast<CBoss_Petroch*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}


void CPetroch_State_Retreat::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CPetroch_State_Retreat* CPetroch_State_Retreat::Create(void* pArg)
{
	CPetroch_State_Retreat* pInstance = new CPetroch_State_Retreat();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CPetroch_State_Retreat");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CPetroch_State_Retreat::Free()
{
	__super::Free();
}
