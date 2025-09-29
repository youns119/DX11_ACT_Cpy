#include "pch.h"
#include "Petroch_State_Death.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"
#include "Animation.h"

#include "Petroch_Shell.h"

CPetroch_State_Death::CPetroch_State_Death()
{
}

HRESULT CPetroch_State_Death::Initialize(void* pObj)
{
	m_strStateName = "Death";

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CPetroch_State_Death::Enter(void* pObj)
{	
	CBoss_Petroch* pPetroch = static_cast<CBoss_Petroch*>(pObj);
	static_cast<CPetroch_Shell*>(pPetroch->Find_PartObject({ TEXT("Part_Shell") }))->Set_Dissolve(true);
	static_cast<CCollider*>(pPetroch->Find_PartObject({ TEXT("Part_Shell") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pPetroch->Find_PartObject({ TEXT("Part_LeftClaw") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);	

	pPetroch->Set_IsChase(false);
	pPetroch->Set_IsSideMove(false);
	
	pPetroch->Death();

	m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::DIE, false, 0.2f);
	m_iAnimIndex = (_uint)PETROCH_ANIM_INDEX::DIE;

	CGameInstance::GetInstance()->Play_Sound(L"PetrochVO_Die2.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 1.f);	
	CGameInstance::GetInstance()->FadeSwap_Sound(L"CrabTomb_BGM.wav", (_uint)SOUND_CHANNEL::BGM, 0.1f, 0.5f);
	super::Enter(pObj);
}

void CPetroch_State_Death::Update(_float fTimeDelta, void* pObj)
{
	if (m_iAnimIndex == (_uint)PETROCH_ANIM_INDEX::IDLE && !m_pModel->Get_IsPlaying((_uint)PETROCH_ANIM_INDEX::IDLE)) {
		static_cast<CBoss_Petroch*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}


void CPetroch_State_Death::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CPetroch_State_Death* CPetroch_State_Death::Create(void* pArg)
{
	CPetroch_State_Death* pInstance = new CPetroch_State_Death();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CPetroch_State_Death");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CPetroch_State_Death::Free()
{
	__super::Free();
}
