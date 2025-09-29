#include "pch.h"
#include "Inkerton_State_DodgeBack.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"


CInkerton_State_DodgeBack::CInkerton_State_DodgeBack()
{
}

HRESULT CInkerton_State_DodgeBack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Charge Attack";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 20.f;
	
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")));

	return S_OK;
}


void CInkerton_State_DodgeBack::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::DODGE_BACK, false, 0.2f);
	CGameInstance::GetInstance()->Play_Sound(L"WaterWhoosh_Light_4.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE3, 0.5f);
	super::Enter(pObj);
}

void CInkerton_State_DodgeBack::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::DODGE_BACK)) {
		static_cast<CBoss_Inkerton*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_DodgeBack::Exit(void* pObj)
{
	super::Exit(pObj);
}

CInkerton_State_DodgeBack* CInkerton_State_DodgeBack::Create(void* pArg)
{
	CInkerton_State_DodgeBack* pInstance = new CInkerton_State_DodgeBack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_DodgeBack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_DodgeBack::Free()
{
	__super::Free();
}
