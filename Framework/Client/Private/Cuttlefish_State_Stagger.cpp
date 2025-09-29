#include "pch.h"
#include "Cuttlefish_State_Stagger.h"
#include "GameInstance.h"
#include "Enemy_Cuttlefish.h"


CCuttlefish_State_Stagger::CCuttlefish_State_Stagger()
{
}

HRESULT CCuttlefish_State_Stagger::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Stagger";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 10.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Cuttlefish*>(pObj)->Find_Component(TEXT("Com_Model")));
	return S_OK;
}

void CCuttlefish_State_Stagger::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::HURT, false);
	__super::Enter(pObj);

	CGameInstance::GetInstance()->Play_Sound(L"Cuttle_Hit2.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
}

void CCuttlefish_State_Stagger::Update(_float fTimeDelta, void* pObj)
{			
	if (!m_pModel->Get_IsPlaying((_uint)CUTTLEFISH_ANIM_INDEX::HURT)) {
		static_cast<CEnemy_Cuttlefish*>(pObj)->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CCuttlefish_State_Stagger::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CCuttlefish_State_Stagger* CCuttlefish_State_Stagger::Create(void* pArg)
{
	CCuttlefish_State_Stagger* pInstance = new CCuttlefish_State_Stagger();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_State_Stagger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCuttlefish_State_Stagger::Free()
{
	__super::Free();
}
