#include "pch.h"
#include "Sardine_State_SpinAttack.h"
#include "GameInstance.h"
#include "Enemy_Sardine.h"


CSardine_State_SpinAttack::CSardine_State_SpinAttack()
{
}

HRESULT CSardine_State_SpinAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Spin Attack";
#endif
	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 5.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Sardine*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CSardine_State_SpinAttack::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)SARDINE_ANIM_INDEX::SPIN_ATTACK, false, 0.3f);

	m_fMaxActionDelay = CUtils::Compute_Random(0.1f, 1.f);
	
	__super::Enter(pObj);
}

void CSardine_State_SpinAttack::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)SARDINE_ANIM_INDEX::SPIN_ATTACK)) {
		if (After_AttackDelay(fTimeDelta))
			static_cast<CEnemy_Sardine*>(pObj)->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CSardine_State_SpinAttack::Exit(void* pObj)
{
	m_bIsAnimChanged = false;
	m_fCurActionDelay = 0;
	__super::Exit(pObj);
}

_bool CSardine_State_SpinAttack::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = CUtils::Compute_Random(0.1f, 1.f);
		m_pModel->SetUp_Animation((_uint)SARDINE_ANIM_INDEX::SWIM_FORWARD, true, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;

	return false;
}

CSardine_State_SpinAttack* CSardine_State_SpinAttack::Create(void* pArg)
{
	CSardine_State_SpinAttack* pInstance = new CSardine_State_SpinAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSardine_State_SpinAttack::Free()
{
	__super::Free();	
}
