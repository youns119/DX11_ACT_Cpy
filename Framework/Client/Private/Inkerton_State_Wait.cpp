#include "pch.h"
#include "Inkerton_State_Wait.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "Animation.h"


CInkerton_State_Wait::CInkerton_State_Wait()
{
}

HRESULT CInkerton_State_Wait::Initialize(void* pObj)
{
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CInkerton_State_Wait::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::IDLE, false, 0.2f);

	CGameInstance::GetInstance()->Play_Sound_Loop(L"Inkerton_Idle_Loop_Keys", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE1, 0.3f);
	super::Enter(pObj);
}

void CInkerton_State_Wait::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::IDLE)) {
		static_cast<CBoss_Inkerton*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_Wait::Exit(void* pObj)
{	
	CGameInstance::GetInstance()->Stop_Sound((_uint)SOUND_CHANNEL::ENEMY_AMBIENCE1);
	super::Exit(pObj);
}

CInkerton_State_Wait* CInkerton_State_Wait::Create(void* pArg)
{
	CInkerton_State_Wait* pInstance = new CInkerton_State_Wait();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_Wait");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CInkerton_State_Wait::Free()
{
	__super::Free();
}
