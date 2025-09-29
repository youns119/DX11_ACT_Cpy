#include "pch.h"
#include "Inkerton_State_LightAttack.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "Animation.h"


CInkerton_State_LightAttack::CInkerton_State_LightAttack()
{
}

HRESULT CInkerton_State_LightAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Light Attack";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 1.f;


	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}


void CInkerton_State_LightAttack::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::COMBO_1, false, 0.2f);
	CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Attack_1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Effort_Prone_2.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE2, 0.5f);
	super::Enter(pObj);
}

void CInkerton_State_LightAttack::Update(_float fTimeDelta, void* pObj)
{		
	if (!m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::COMBO_1)) {
		if (After_AttackDelay(fTimeDelta))
			static_cast<CBoss_Inkerton*>(pObj)->Select_Action();		
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_LightAttack::Exit(void* pObj)
{	
	super::Exit(pObj);
}

_bool CInkerton_State_LightAttack::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = CUtils::Compute_Random(0.1f, 0.3f);;
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::IDLE, true, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;

	return false;
}

CInkerton_State_LightAttack* CInkerton_State_LightAttack::Create(void* pArg)
{
	CInkerton_State_LightAttack* pInstance = new CInkerton_State_LightAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_LightAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_LightAttack::Free()
{
	__super::Free();
}
