#include "pch.h"
#include "Splitter_State_GuillotineSlam.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Enemy_Splitter.h"


CSplitter_State_GuillotineSlam::CSplitter_State_GuillotineSlam()
{
}

HRESULT CSplitter_State_GuillotineSlam::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Guillo Slam";
#endif

	m_iPriorityLevel = 1;
	//m_fMaxCoolTime = 2.f;
	m_fMaxCoolTime = 5.5f;
	
	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CSplitter_State_GuillotineSlam::Enter(void* pObj)
{
	m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::GUILLOTINE_SLAM, false, 0.3f);
	static_cast<CEnemy_Splitter*>(pObj)->Set_IsUnblockable(true);	
	
	CGameInstance::GetInstance()->Play_Sound(L"Executioner_OverheadCharge.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	__super::Enter(pObj);
}

void CSplitter_State_GuillotineSlam::Update(_float fTimeDelta, void* pObj)
{
	if (!m_pModel->Get_IsPlaying((_uint)SPLITTER_ANIM_INDEX::GUILLOTINE_SLAM) || m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		if (After_AttackDelay(fTimeDelta)) {
			static_cast<CEnemy_Splitter*>(pObj)->Select_Action();
			static_cast<CEnemy_Splitter*>(pObj)->Set_IsUnblockable(false);
		}			
	}
}

void CSplitter_State_GuillotineSlam::Exit(void* pObj)
{
	m_fCurActionDelay = 0;
	m_bIsAnimChanged = false;

	__super::Exit(pObj);
}

_bool CSplitter_State_GuillotineSlam::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = CUtils::Compute_Random(0.1f, .2f);;
		m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::GUILLOTINE_UP_IDLE, true, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;

	return false;
}

CSplitter_State_GuillotineSlam* CSplitter_State_GuillotineSlam::Create(void* pObj)
{
	CSplitter_State_GuillotineSlam* pInstance = new CSplitter_State_GuillotineSlam();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_GuillotineSlam");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSplitter_State_GuillotineSlam::Free()
{
	__super::Free();
}
