#include "pch.h"
#include "Inkerton_State_SwimFront.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"


CInkerton_State_SwimFront::CInkerton_State_SwimFront()
{
}

HRESULT CInkerton_State_SwimFront::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Swim Front";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 10.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CInkerton_State_SwimFront::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::SWIM_FORWARD, false, 0.2f);
	CGameInstance::GetInstance()->Play_Sound(L"WaterWhoosh_Light_4.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE3, 0.6f);
	CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Banter_7.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.6f);
	super::Enter(pObj);
}

void CInkerton_State_SwimFront::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::SWIM_FORWARD)) {
		static_cast<CBoss_Inkerton*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_SwimFront::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CInkerton_State_SwimFront* CInkerton_State_SwimFront::Create(void* pArg)
{
	CInkerton_State_SwimFront* pInstance = new CInkerton_State_SwimFront();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_SwimFront");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_SwimFront::Free()
{
	__super::Free();
}
