#include "pch.h"
#include "Splitter_State_BashAttack.h"
#include "Animation.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"



CSplitter_State_BashAttack::CSplitter_State_BashAttack()
{
}

HRESULT CSplitter_State_BashAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Bash Attack";
#endif

	m_iPriorityLevel = 2;
	//m_fMaxCoolTime = 1.5f;
	m_fMaxCoolTime = 5.5f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CSplitter_State_BashAttack::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::CHASE_BASH_ATTACK, false, 0.3f);
	
	CGameInstance::GetInstance()->Play_Sound(L"Executioner_PushGrunt2.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.6f);
	m_fMaxActionDelay = CUtils::Compute_Random(0.3f, 1.f);
	__super::Enter(pObj);
}

void CSplitter_State_BashAttack::Update(_float fTimeDelta, void* pObj)
{
	if (!m_pModel->Get_IsPlaying((_uint)SPLITTER_ANIM_INDEX::CHASE_BASH_ATTACK) || m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		if (After_AttackDelay(fTimeDelta))
			static_cast<CEnemy_Splitter*>(pObj)->Select_Action();
	}
}

void CSplitter_State_BashAttack::Exit(void* pObj)
{
	m_fCurActionDelay = 0;
	m_bIsAnimChanged = false;

	__super::Exit(pObj);
}

_bool CSplitter_State_BashAttack::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = CUtils::Compute_Random(0.1f, 0.2f);;
		m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::GUILLOTINE_UP_IDLE, true, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;

	return false;
}

CSplitter_State_BashAttack* CSplitter_State_BashAttack::Create(void* pObj)
{
	CSplitter_State_BashAttack* pInstance = new CSplitter_State_BashAttack();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_BashAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_State_BashAttack::Free()
{
	__super::Free();
}
