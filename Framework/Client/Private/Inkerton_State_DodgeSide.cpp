#include "pch.h"
#include "Inkerton_State_DodgeSide.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"


CInkerton_State_DodgeSide::CInkerton_State_DodgeSide()
{
}

HRESULT CInkerton_State_DodgeSide::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Dodge Left";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 20.f;	

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")));

	return S_OK;
}

void CInkerton_State_DodgeSide::Enter(void* pObj)
{		
	_uint iRanNum = (_uint)CUtils::Compute_Random(1.f, 10.f);
	if(iRanNum > 5)
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::DODGE_LEFT, false, 0.2f);
	else
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::DODGE_RIGHT, false, 0.2f);
	CGameInstance::GetInstance()->Play_Sound(L"WaterWhoosh_Light_4.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE3, 0.5f);
	super::Enter(pObj);
}

void CInkerton_State_DodgeSide::Update(_float fTimeDelta, void* pObj)
{			
	if (!m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::DODGE_LEFT) && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::DODGE_RIGHT)) {
		static_cast<CBoss_Inkerton*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_DodgeSide::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CInkerton_State_DodgeSide* CInkerton_State_DodgeSide::Create(void* pArg)
{
	CInkerton_State_DodgeSide* pInstance = new CInkerton_State_DodgeSide();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_DodgeLeft");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_DodgeSide::Free()
{
	__super::Free();
}
