#include "pch.h"
#include "Inkerton_State_HeavyChargeAttack.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "Animation.h"

CInkerton_State_HeavyChargeAttack::CInkerton_State_HeavyChargeAttack()
{
}

HRESULT CInkerton_State_HeavyChargeAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Heavy Charge Attack";
#endif

	m_iPriorityLevel = 1;	
	m_fMaxCoolTime = 10.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));		

	return S_OK;
}

void CInkerton_State_HeavyChargeAttack::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::CHARGE_HEAVY_ATTACK, false, 0.2f);
	m_iCurAnimIndex = (_uint)INKERTON_ANIM_INDEX::CHARGE_HEAVY_ATTACK;

	static_cast<CBoss_Inkerton*>(pObj)->Play_BaterQuote();

	super::Enter(pObj);
}

void CInkerton_State_HeavyChargeAttack::Update(_float fTimeDelta, void* pObj)
{
	if (m_iCurAnimIndex == (_uint)INKERTON_ANIM_INDEX::CHARGE_HEAVY_ATTACK && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::CHARGE_HEAVY_ATTACK)) {
		if (After_AttackDelay(fTimeDelta))
			static_cast<CBoss_Inkerton*>(pObj)->Select_Action();		
	}
	
	
	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_HeavyChargeAttack::Exit(void* pObj)
{	
	super::Exit(pObj);
}

_bool CInkerton_State_HeavyChargeAttack::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = CUtils::Compute_Random(0.2f, 1.f);;
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::IDLE, true, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;
	return false;
}

CInkerton_State_HeavyChargeAttack* CInkerton_State_HeavyChargeAttack::Create(void* pArg)
{
	CInkerton_State_HeavyChargeAttack* pInstance = new CInkerton_State_HeavyChargeAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_HeavyChargeAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_HeavyChargeAttack::Free()
{
	__super::Free();
}
