#include "pch.h"
#include "Splitter_State_Aggroed.h"
#include "Animation.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"

CSplitter_State_Aggroed::CSplitter_State_Aggroed()
{
}

HRESULT CSplitter_State_Aggroed::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Walk";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Model")));	

	return S_OK;
}

void CSplitter_State_Aggroed::Enter(void* pObj)
{
	m_pMovement = static_cast<CMovement*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Movement")));
	m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::GUILLOTINE_UP_IDLE, true, 0.3f);

	CGameInstance::GetInstance()->Play_Sound(L"Executioner_Aggro1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.6f);
	__super::Enter(pObj);
}

void CSplitter_State_Aggroed::Update(_float fTimeDelta, void* pObj)
{
	_vector vLook = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	if(m_pMovement != nullptr)
		m_pMovement->TurnToDirection(vLook, fTimeDelta);	

	if (!m_pModel->Get_IsPlaying((_uint)SPLITTER_ANIM_INDEX::GUILLOTINE_UP_IDLE) || m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		static_cast<CEnemy_Splitter*>(pObj)->Select_Action();
	}
}

void CSplitter_State_Aggroed::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CSplitter_State_Aggroed* CSplitter_State_Aggroed::Create(void* pObj)
{
	CSplitter_State_Aggroed* pInstance = new CSplitter_State_Aggroed();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Aggroed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_State_Aggroed::Free()
{
	__super::Free();
}
