#include "pch.h"
#include "Normie_State_Hooked.h"
#include "GameInstance.h"
#include "Enemy_Normie.h"


CNormie_State_Hooked::CNormie_State_Hooked()
{
}

HRESULT CNormie_State_Hooked::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Aggroed";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Normie*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CNormie_State_Hooked::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)NORMIE_ANIM_INDEX::HOOKED, false);
	//CGameInstance::GetInstance()->Play_Sound(L"Sardine Aggro 1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	__super::Enter(pObj);
}

void CNormie_State_Hooked::Update(_float fTimeDelta, void* pObj)
{	
	if (!m_pModel->Get_IsPlaying((_uint)NORMIE_ANIM_INDEX::HOOKED)) {
		static_cast<CEnemy_Normie*>(pObj)->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CNormie_State_Hooked::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CNormie_State_Hooked* CNormie_State_Hooked::Create(void* pArg)
{
	CNormie_State_Hooked* pInstance = new CNormie_State_Hooked();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNormie_State_Hooked::Free()
{
	__super::Free();	
}