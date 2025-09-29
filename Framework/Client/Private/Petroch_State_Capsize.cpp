#include "pch.h"
#include "Petroch_State_Capsize.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"
#include "Animation.h"


CPetroch_State_Capsize::CPetroch_State_Capsize()
{
}

HRESULT CPetroch_State_Capsize::Initialize(void* pObj)
{
	m_strStateName = "Capsize";

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CPetroch_State_Capsize::Enter(void* pObj)
{	
	m_pPetroch = static_cast<CBoss_Petroch*>(pObj);
	static_cast<CCollider*>(m_pPetroch->Find_PartObject({ TEXT("Part_Shell") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(m_pPetroch->Find_PartObject({ TEXT("Part_LeftClaw") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);

	m_pPetroch->Set_IsChase(false);
	m_pPetroch->Set_Block(false);
	m_pPetroch->Set_IsSideMove(false);

	m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::FLIPPED_ENTER, false, 0.8f);
	m_iAnimIndex = (_uint)PETROCH_ANIM_INDEX::FLIPPED_ENTER;
	
	super::Enter(pObj);
}

void CPetroch_State_Capsize::Update(_float fTimeDelta, void* pObj)
{
	if (m_iAnimIndex == (_uint)PETROCH_ANIM_INDEX::FLIPPED_ENTER && !m_pModel->Get_IsPlaying((_uint)PETROCH_ANIM_INDEX::FLIPPED_ENTER)) {
		m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::FLIPPED, true, 0.2f);
		m_iAnimIndex = (_uint)PETROCH_ANIM_INDEX::FLIPPED;

		CGameInstance::GetInstance()->Play_Sound(L"PetrochVO_Flipped2.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.8f);
	}
	if (m_iAnimIndex == (_uint)PETROCH_ANIM_INDEX::FLIPPED && Decrease_CapsizedDuration(fTimeDelta) && !m_pPetroch->IsFinisher()) {
		m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::FLIPPED_EXIT, false, 0.2f);
		m_iAnimIndex = (_uint)PETROCH_ANIM_INDEX::FLIPPED_EXIT;
		m_pPetroch->Set_Capsized(false);
	}
	if (m_iAnimIndex == (_uint)PETROCH_ANIM_INDEX::FLIPPED_EXIT && !m_pModel->Get_IsPlaying((_uint)PETROCH_ANIM_INDEX::FLIPPED_EXIT)) {
		static_cast<CBoss_Petroch*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}


void CPetroch_State_Capsize::Exit(void* pObj)
{	
	m_pPetroch->Set_Finished(false);
	m_fCapsizeTime = 0;
	super::Exit(pObj);
}

_bool CPetroch_State_Capsize::Decrease_CapsizedDuration(_float fTimeDelta)
{
	if (m_fCapsizeTime > m_fCapsizeDuration)
		return true;
	else
		m_fCapsizeTime += fTimeDelta;

	return false;
}

CPetroch_State_Capsize* CPetroch_State_Capsize::Create(void* pArg)
{
	CPetroch_State_Capsize* pInstance = new CPetroch_State_Capsize();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CPetroch_State_Capsize");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CPetroch_State_Capsize::Free()
{
	__super::Free();
}
