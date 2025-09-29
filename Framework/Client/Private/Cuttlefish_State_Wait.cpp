#include "pch.h"
#include "Cuttlefish_State_Wait.h"
#include "GameInstance.h"
#include "Enemy_Cuttlefish.h"


CCuttlefish_State_Wait::CCuttlefish_State_Wait()
{
}

HRESULT CCuttlefish_State_Wait::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Wait";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	
	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Cuttlefish*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));

	return S_OK;
}

void CCuttlefish_State_Wait::Enter(void* pObj)
{	
	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));

	m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::IDLE, false);

	CGameInstance::GetInstance()->Play_Sound(L"Cuttle_Idle.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.2f);
	__super::Enter(pObj);
}

void CCuttlefish_State_Wait::Update(_float fTimeDelta, void* pObj)
{			
	_vector vLookTarget = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	m_pMovement->TurnToDirection(vLookTarget, fTimeDelta);

	if (!m_pModel->Get_IsPlaying((_uint)CUTTLEFISH_ANIM_INDEX::IDLE)) {
		static_cast<CEnemy_Cuttlefish*>(pObj)->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CCuttlefish_State_Wait::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CCuttlefish_State_Wait* CCuttlefish_State_Wait::Create(void* pArg)
{
	CCuttlefish_State_Wait* pInstance = new CCuttlefish_State_Wait();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_State_Wait");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCuttlefish_State_Wait::Free()
{
	__super::Free();
}
