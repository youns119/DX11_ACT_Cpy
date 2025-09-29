#include "pch.h"
#include "Gumpounder_State_Stagger.h"
#include "GameInstance.h"
#include "Enemy_Gumpounder.h"


CGumpounder_State_Stagger::CGumpounder_State_Stagger()
{
}

HRESULT CGumpounder_State_Stagger::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Aggroed";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Gumpounder*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CGumpounder_State_Stagger::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)GUMPOUNDER_ANIM_INDEX::HURT, false);
	//CGameInstance::GetInstance()->Play_Sound(L"Sardine Aggro 1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	__super::Enter(pObj);
}

void CGumpounder_State_Stagger::Update(_float fTimeDelta, void* pObj)
{	
	if (!m_pModel->Get_IsPlaying((_uint)GUMPOUNDER_ANIM_INDEX::HURT)) {
		static_cast<CEnemy_Gumpounder*>(pObj)->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CGumpounder_State_Stagger::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CGumpounder_State_Stagger* CGumpounder_State_Stagger::Create(void* pArg)
{
	CGumpounder_State_Stagger* pInstance = new CGumpounder_State_Stagger();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGumpounder_State_Stagger::Free()
{
	__super::Free();	
}