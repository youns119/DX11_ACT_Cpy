#include "pch.h"
#include "Petroch_State_Stagger.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"
#include "Animation.h"


CPetroch_State_Stagger::CPetroch_State_Stagger()
{
}

HRESULT CPetroch_State_Stagger::Initialize(void* pObj)
{
	m_strStateName = "Stagger";

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CPetroch_State_Stagger::Enter(void* pObj)
{	
	CBoss_Petroch* pPetroch = static_cast<CBoss_Petroch*>(pObj);
	static_cast<CCollider*>(pPetroch->Find_PartObject({ TEXT("Part_Shell") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pPetroch->Find_PartObject({ TEXT("Part_LeftClaw") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);

	pPetroch->Set_IsChase(false);
	pPetroch->Set_IsSideMove(false);

	m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::HURT, false, 0.2f);
	m_iAnimIndex = (_uint)PETROCH_ANIM_INDEX::HURT;

	CGameInstance::GetInstance()->Play_Sound(L"PetrochVO_Hurt4.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 1.f);

	super::Enter(pObj);
}

void CPetroch_State_Stagger::Update(_float fTimeDelta, void* pObj)
{
	if (m_iAnimIndex == (_uint)PETROCH_ANIM_INDEX::HURT && !m_pModel->Get_IsPlaying((_uint)PETROCH_ANIM_INDEX::HURT)) {
		static_cast<CBoss_Petroch*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}


void CPetroch_State_Stagger::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CPetroch_State_Stagger* CPetroch_State_Stagger::Create(void* pArg)
{
	CPetroch_State_Stagger* pInstance = new CPetroch_State_Stagger();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CPetroch_State_Stagger");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CPetroch_State_Stagger::Free()
{
	__super::Free();
}
