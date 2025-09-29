#include "pch.h"
#include "Splitter_State_Fear.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"

#include "Player.h"

CSplitter_State_Fear::CSplitter_State_Fear()
{
}

HRESULT CSplitter_State_Fear::Initialize(void* pObj)
{	
#ifdef _DEBUG
	m_strStateName = "Fear";
#endif

	m_iPriorityLevel = 0;
	m_fMaxCoolTime = 30.f;

	m_pSplitter = static_cast<CEnemy_Splitter*>(pObj);
	m_pPlayer = static_cast<CPlayer*>(CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back());	
	m_pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(TEXT("Com_Transform")));
	m_pTransform = static_cast<CTransform*>(m_pSplitter->Find_Component(TEXT("Com_Transform")));
	m_pModel = static_cast<CModel*>(m_pSplitter->Find_Component(TEXT("Com_Model")));
	__super::Enter(pObj);
	return S_OK;
}

void CSplitter_State_Fear::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::FEAR, false, 0.3f);
	__super::Enter(pObj);
}

void CSplitter_State_Fear::Update(_float fTimeDelta, void* pObj)
{	
	_vector temp = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector sour = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_float dist = XMVectorGetX(XMVector4Length(temp - sour));
	if (m_pSplitter->Is_Fear() &&  dist < 10.f) {
		m_pPlayer->Feared(fTimeDelta * 10.f);
	}

	if (!m_pModel->Get_IsPlaying((_uint)SPLITTER_ANIM_INDEX::FEAR)) {
		if (After_AttackDelay(fTimeDelta))
			static_cast<CEnemy_Splitter*>(pObj)->Select_Action();
	}
}

void CSplitter_State_Fear::Exit(void* pObj)
{	
	m_fCurActionDelay = 0;
	m_bIsAnimChanged = false;

	__super::Exit(pObj);
}

_bool CSplitter_State_Fear::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = CUtils::Compute_Random(0.1f, 0.3f);
		m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::GUILLOTINE_UP_IDLE, true, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;

	return false;
}

CSplitter_State_Fear* CSplitter_State_Fear::Create(void* pObj)
{
	CSplitter_State_Fear* pInstance = new CSplitter_State_Fear();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Fear");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSplitter_State_Fear::Free()
{
	__super::Free();
}
