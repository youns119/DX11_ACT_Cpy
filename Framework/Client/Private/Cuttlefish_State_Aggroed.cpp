#include "pch.h"
#include "Cuttlefish_State_Aggroed.h"
#include "GameInstance.h"
#include "Enemy_Cuttlefish.h"


CCuttlefish_State_Aggroed::CCuttlefish_State_Aggroed()
{
}

HRESULT CCuttlefish_State_Aggroed::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Aggroed";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 10.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	
	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Cuttlefish*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));

	m_Sounds.push_back(L"Cuttle_Aggro1.wav");
	m_Sounds.push_back(L"Cuttle_Aggro2.wav");
	m_Sounds.push_back(L"Cuttle_Aggro4.wav");
	m_Sounds.push_back(L"Cuttle_Aggro5.wav");

	return S_OK;
}

void CCuttlefish_State_Aggroed::Enter(void* pObj)
{	
	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));

	m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::AGGRO, false);

	_int ISoundIndex = CUtils::Compute_Random(0, m_Sounds.size() - 1);
	CGameInstance::GetInstance()->Play_Sound(m_Sounds[ISoundIndex], (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	__super::Enter(pObj);
}

void CCuttlefish_State_Aggroed::Update(_float fTimeDelta, void* pObj)
{			
	_vector vLookTarget = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	m_pMovement->TurnToDirection(vLookTarget, fTimeDelta);

	if (!m_pModel->Get_IsPlaying((_uint)CUTTLEFISH_ANIM_INDEX::AGGRO)) {
		static_cast<CEnemy_Cuttlefish*>(pObj)->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CCuttlefish_State_Aggroed::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CCuttlefish_State_Aggroed* CCuttlefish_State_Aggroed::Create(void* pArg)
{
	CCuttlefish_State_Aggroed* pInstance = new CCuttlefish_State_Aggroed();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_State_Aggroed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCuttlefish_State_Aggroed::Free()
{
	__super::Free();
}
