#include "pch.h"
#include "Cuttlefish_State_Punch.h"
#include "GameInstance.h"
#include "Enemy_Cuttlefish.h"


CCuttlefish_State_Punch::CCuttlefish_State_Punch()
{
}

HRESULT CCuttlefish_State_Punch::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Punch Attack";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 5.f;
	
	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Cuttlefish*>(pObj)->Find_Component(TEXT("Com_Model")));	

	return S_OK;
}

void CCuttlefish_State_Punch::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::PUNCH, false);

	CGameInstance::GetInstance()->Play_Sound(L"Cuttle_Wooh4.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK, 0.5f);
	__super::Enter(pObj);
}

void CCuttlefish_State_Punch::Update(_float fTimeDelta, void* pObj)
{			
	if (!m_pModel->Get_IsPlaying((_uint)CUTTLEFISH_ANIM_INDEX::PUNCH)) {
		if (After_AttackDelay(fTimeDelta)) {
			static_cast<CEnemy_Cuttlefish*>(pObj)->Select_Action();
		}
	}

	__super::Update(fTimeDelta, pObj);
}

void CCuttlefish_State_Punch::Exit(void* pObj)
{
	m_fCurActionDelay = 0;
	__super::Exit(pObj);
}

_bool CCuttlefish_State_Punch::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = CUtils::Compute_Random(0.1f, 0.3f);
		m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::IDLE, false, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;
	return false;
}

CCuttlefish_State_Punch* CCuttlefish_State_Punch::Create(void* pArg)
{
	CCuttlefish_State_Punch* pInstance = new CCuttlefish_State_Punch();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_State_Punch");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCuttlefish_State_Punch::Free()
{
	__super::Free();
}
