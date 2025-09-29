#include "pch.h"
#include "Splitter_State_GuillotineHit.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Enemy_Splitter.h"


CSplitter_State_GuillotineHit::CSplitter_State_GuillotineHit()
{
}

HRESULT CSplitter_State_GuillotineHit::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Guillo Hit";
#endif

	m_iPriorityLevel = 2;
	m_fMaxCoolTime = 5.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CSplitter_State_GuillotineHit::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::GUILLOTINE_HIT, false, 0.5f);

	__super::Enter(pObj);
}

void CSplitter_State_GuillotineHit::Update(_float fTimeDelta, void* pObj)
{	
	if (!m_pModel->Get_IsPlaying((_uint)SPLITTER_ANIM_INDEX::GUILLOTINE_HIT) || m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		if (After_AttackDelay(fTimeDelta))
			static_cast<CEnemy_Splitter*>(pObj)->Select_Action();
	}
}

void CSplitter_State_GuillotineHit::Exit(void* pObj)
{
	m_fCurActionDelay = 0;
	m_bIsAnimChanged = false;

	__super::Exit(pObj);
}

_bool CSplitter_State_GuillotineHit::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = CUtils::Compute_Random(0.1f, .2f);
		m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::GUILLOTINE_UP_IDLE, true, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;

	return false;
}

CSplitter_State_GuillotineHit* CSplitter_State_GuillotineHit::Create(void* pObj)
{
	CSplitter_State_GuillotineHit* pInstance = new CSplitter_State_GuillotineHit();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_GuillotineHit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSplitter_State_GuillotineHit::Free()
{
	__super::Free();
}