#include "pch.h"
#include "Inkerton_State_DodgeLeftAttack.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"


CInkerton_State_DodgeLeftAttack::CInkerton_State_DodgeLeftAttack()
{
}

HRESULT CInkerton_State_DodgeLeftAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Dodge Left Attack";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;	

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")));

	return S_OK;
}

void CInkerton_State_DodgeLeftAttack::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::LEFT_DODGE_ATTACK, false, 0.2f);
	static_cast<CBoss_Inkerton*>(pObj)->Set_IsChase(true, 0.f);
	CGameInstance::GetInstance()->Play_Sound(L"WaterWhoosh_Light_4.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE3, 0.5f);
	super::Enter(pObj);
}

void CInkerton_State_DodgeLeftAttack::Update(_float fTimeDelta, void* pObj)
{			
	if (!m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::LEFT_DODGE_ATTACK)) {
		CBoss_Inkerton* pInkerton = static_cast<CBoss_Inkerton*>(pObj);
		pInkerton->Set_IsChase(false);
		pInkerton->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_DodgeLeftAttack::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CInkerton_State_DodgeLeftAttack* CInkerton_State_DodgeLeftAttack::Create(void* pArg)
{
	CInkerton_State_DodgeLeftAttack* pInstance = new CInkerton_State_DodgeLeftAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_DodgeLeftAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_DodgeLeftAttack::Free()
{
	__super::Free();
}
