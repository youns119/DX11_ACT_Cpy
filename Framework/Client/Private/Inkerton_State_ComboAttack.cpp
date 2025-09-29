#include "pch.h"
#include "Inkerton_State_ComboAttack.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "Animation.h"


CInkerton_State_ComboAttack::CInkerton_State_ComboAttack()
{
}

HRESULT CInkerton_State_ComboAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Combo Attack";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 1.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}


void CInkerton_State_ComboAttack::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::COMBO_1, false, 0.2f);
	m_pModel->Get_CurrAnimation()->Set_BaseAnimSpeed(1.2f);
	m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::COMBO_1;

	CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Attack_Effort_1.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE2, 0.6f);
	super::Enter(pObj);
}

void CInkerton_State_ComboAttack::Update(_float fTimeDelta, void* pObj)
{		
	if (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::COMBO_1 && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::COMBO_1) || m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::COMBO_1 && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		_float fCurHP = static_cast<CBoss_Inkerton*>(pObj)->Get_CurrentHP();
		_float fMaxHP = static_cast<CBoss_Inkerton*>(pObj)->Get_MaxHP();
		if (fCurHP > (fMaxHP * 0.7f)) {
			m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::COMBO_2, false, 0.3f);
			m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::COMBO_2;
			CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Attack_Effort_1.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE2, 0.6f);
		}
		else {
			m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::COMBO_2_2, false, 0.3f);
			m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::COMBO_2_2;
			CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Attack_Effort_1.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE2, 0.6f);
		}
	}
	else if (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::COMBO_2 && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::COMBO_2) || m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::COMBO_2 && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::COMBO_3, false, 0.3f);
		m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::COMBO_3;
		CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Attack_Effort_2.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE2, 0.6f);
	}
	else if (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::COMBO_2_2 && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::COMBO_2_2) || m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::COMBO_2_2 && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::COMBO_3, false, 0.3f);
		CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Attack_Effort_2.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE2, 0.6f);
		m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::COMBO_3;
	}
	else if (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::COMBO_3 && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::COMBO_3) || m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::COMBO_3 && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		if (After_AttackDelay(fTimeDelta))
			CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Attack_Effort_3.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE2, 0.6f);
			static_cast<CBoss_Inkerton*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_ComboAttack::Exit(void* pObj)
{	
	m_pModel->Get_CurrAnimation()->Set_BaseAnimSpeed(1.f);

	super::Exit(pObj);
}

_bool CInkerton_State_ComboAttack::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = CUtils::Compute_Random(0.5f, 1.f);
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::IDLE, true, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;

	return false;
}

CInkerton_State_ComboAttack* CInkerton_State_ComboAttack::Create(void* pArg)
{
	CInkerton_State_ComboAttack* pInstance = new CInkerton_State_ComboAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_ComboAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_ComboAttack::Free()
{
	__super::Free();
}
