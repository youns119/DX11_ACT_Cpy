#include "pch.h"
#include "Normie_State_SmashAttack.h"
#include "GameInstance.h"
#include "Enemy_Normie.h"


CNormie_State_SmashAttack::CNormie_State_SmashAttack()
{
}

HRESULT CNormie_State_SmashAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Aggroed";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Normie*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CNormie_State_SmashAttack::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)NORMIE_ANIM_INDEX::ATKSMASH, false);
	//CGameInstance::GetInstance()->Play_Sound(L"Sardine Aggro 1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	__super::Enter(pObj);
}

void CNormie_State_SmashAttack::Update(_float fTimeDelta, void* pObj)
{	
	if (!m_pModel->Get_IsPlaying((_uint)NORMIE_ANIM_INDEX::ATKSMASH)) {
		static_cast<CEnemy_Normie*>(pObj)->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CNormie_State_SmashAttack::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CNormie_State_SmashAttack* CNormie_State_SmashAttack::Create(void* pArg)
{
	CNormie_State_SmashAttack* pInstance = new CNormie_State_SmashAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNormie_State_SmashAttack::Free()
{
	__super::Free();	
}