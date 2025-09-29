#include "pch.h"
#include "Inkerton_State_DodgeRight.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"


CInkerton_State_DodgeRight::CInkerton_State_DodgeRight()
{
}

HRESULT CInkerton_State_DodgeRight::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Dodge Right";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 20.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")));

	return S_OK;
}


void CInkerton_State_DodgeRight::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::DODGE_RIGHT, false, 0.2f);
	CGameInstance::GetInstance()->Play_Sound(L"WaterWhoosh_Light_4.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE3, 0.5f);
	super::Enter(pObj);
}

void CInkerton_State_DodgeRight::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::DODGE_RIGHT)) {
		static_cast<CBoss_Inkerton*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_DodgeRight::Exit(void* pObj)
{
	super::Exit(pObj);
}

CInkerton_State_DodgeRight* CInkerton_State_DodgeRight::Create(void* pArg)
{
	CInkerton_State_DodgeRight* pInstance = new CInkerton_State_DodgeRight();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_DodgeRight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_DodgeRight::Free()
{
	__super::Free();
}
