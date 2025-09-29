#include "pch.h"
#include "Inkerton_State_DodgeRightAttack.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "Animation.h"

CInkerton_State_DodgeRightAttack::CInkerton_State_DodgeRightAttack()
{
}

HRESULT CInkerton_State_DodgeRightAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Dodge Right Attack";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 20.f;	

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")));

	return S_OK;
}

void CInkerton_State_DodgeRightAttack::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::RIGHT_DODGE_ATTACK, false, 0.2f);	
	m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::RIGHT_DODGE_ATTACK;
	CGameInstance::GetInstance()->Play_Sound(L"WaterWhoosh_Light_4.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE3, 0.5f);
	super::Enter(pObj);
}

void CInkerton_State_DodgeRightAttack::Update(_float fTimeDelta, void* pObj)
{			
	if (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::RIGHT_DODGE_ATTACK && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::RIGHT_DODGE_ATTACK) || m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::RIGHT_DODGE_ATTACK && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::AIM, false, 0.2f);
		m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::AIM;
	}
	else if (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::AIM && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::AIM)) {		
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::FIRE, false, 0.2f);
		m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::FIRE;
	}
	else if (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::FIRE && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::FIRE)) {
		CBoss_Inkerton* pInkerton = static_cast<CBoss_Inkerton*>(pObj);
		pInkerton->Select_Action();		
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_DodgeRightAttack::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CInkerton_State_DodgeRightAttack* CInkerton_State_DodgeRightAttack::Create(void* pArg)
{
	CInkerton_State_DodgeRightAttack* pInstance = new CInkerton_State_DodgeRightAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_DodgeRightAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_DodgeRightAttack::Free()
{
	__super::Free();
}
