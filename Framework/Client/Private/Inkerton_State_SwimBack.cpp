#include "pch.h"
#include "Inkerton_State_SwimBack.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"


CInkerton_State_SwimBack::CInkerton_State_SwimBack()
{
}

HRESULT CInkerton_State_SwimBack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Swim Back";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 20.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CInkerton_State_SwimBack::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::SWIM_BACK, false, 0.2f);
	CGameInstance::GetInstance()->Play_Sound(L"WaterWhoosh_Light_4.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE3, 0.5f);
	super::Enter(pObj);
}

void CInkerton_State_SwimBack::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::SWIM_BACK)) {
		static_cast<CBoss_Inkerton*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_SwimBack::Exit(void* pObj)
{
	super::Exit(pObj);
}

CInkerton_State_SwimBack* CInkerton_State_SwimBack::Create(void* pArg)
{
	CInkerton_State_SwimBack* pInstance = new CInkerton_State_SwimBack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_SwimBack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_SwimBack::Free()
{
	__super::Free();
}
