#include "pch.h"
#include "Inkerton_State_LightChargeAttack.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "Animation.h"


CInkerton_State_LightChargeAttack::CInkerton_State_LightChargeAttack()
{
}

HRESULT CInkerton_State_LightChargeAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Light Charge Attack";
#endif

	m_iPriorityLevel = 1;	
	m_fMaxCoolTime = 3.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}


void CInkerton_State_LightChargeAttack::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::CHARGE_LIGHT_ATTACK, false, 0.2f);
	static_cast<CBoss_Inkerton*>(pObj)->Play_BaterQuote();	
	super::Enter(pObj);
}

void CInkerton_State_LightChargeAttack::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::CHARGE_LIGHT_ATTACK)) {
		if (After_AttackDelay(fTimeDelta))
			static_cast<CBoss_Inkerton*>(pObj)->Select_Action();		
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_LightChargeAttack::Exit(void* pObj)
{		
	super::Exit(pObj);
}

_bool CInkerton_State_LightChargeAttack::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = CUtils::Compute_Random(0.1f, 0.2f);
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::IDLE, true, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;

	return false;
}

CInkerton_State_LightChargeAttack* CInkerton_State_LightChargeAttack::Create(void* pArg)
{
	CInkerton_State_LightChargeAttack* pInstance = new CInkerton_State_LightChargeAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_LightChargeAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_LightChargeAttack::Free()
{
	__super::Free();
}
