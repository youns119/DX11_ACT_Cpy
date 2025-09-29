#include "pch.h"
#include "Gumpounder_State_AttackCross.h"
#include "GameInstance.h"
#include "Enemy_Gumpounder.h"


CGumpounder_State_AttackCross::CGumpounder_State_AttackCross()
{
}

HRESULT CGumpounder_State_AttackCross::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Aggroed";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Gumpounder*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CGumpounder_State_AttackCross::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)GUMPOUNDER_ANIM_INDEX::ATKCROSS, false);
	//CGameInstance::GetInstance()->Play_Sound(L"Sardine Aggro 1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	__super::Enter(pObj);
}

void CGumpounder_State_AttackCross::Update(_float fTimeDelta, void* pObj)
{	
	if (!m_pModel->Get_IsPlaying((_uint)GUMPOUNDER_ANIM_INDEX::ATKCROSS)) {
		static_cast<CEnemy_Gumpounder*>(pObj)->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CGumpounder_State_AttackCross::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CGumpounder_State_AttackCross* CGumpounder_State_AttackCross::Create(void* pArg)
{
	CGumpounder_State_AttackCross* pInstance = new CGumpounder_State_AttackCross();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGumpounder_State_AttackCross::Free()
{
	__super::Free();	
}