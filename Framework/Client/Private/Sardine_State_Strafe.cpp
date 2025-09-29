#include "pch.h"
#include "Sardine_State_Strafe.h"
#include "GameInstance.h"
#include "Enemy_Sardine.h"


CSardine_State_Strafe::CSardine_State_Strafe()
{
}

HRESULT CSardine_State_Strafe::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Strafe Back";
#endif
	m_iPriorityLevel = 2;
	m_fMaxCoolTime = 0.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Sardine*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")));

	return S_OK;
}

void CSardine_State_Strafe::Enter(void* pObj)
{		
	m_pMovement = static_cast<CMovement*>(static_cast<CEnemy_Sardine*>(pObj)->Find_Component(TEXT("Com_Movement")));
	_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

	m_vLookTarget = (vPosition -= XMVector3Normalize(vLook) * 300);

	m_pModel->SetUp_Animation((_uint)SARDINE_ANIM_INDEX::SWIM_FORWARD, true, 0.01f);
	CGameInstance::GetInstance()->Play_Sound(L"Sardine Aggro 2.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE2, 0.5f);
	__super::Enter(pObj);
}

void CSardine_State_Strafe::Update(_float fTimeDelta, void* pObj)
{					
	m_pMovement->TurnToDirection(m_vLookTarget._vector(), fTimeDelta);
	m_pTransform->Go_Straight(fTimeDelta * 1.2f); //* 5.f

	if (m_fCurActionDuration > m_fMaxActionDuration)
		static_cast<CEnemy_Sardine*>(pObj)->Select_Action();	
	else
		m_fCurActionDuration += fTimeDelta;

	__super::Update(fTimeDelta, pObj);
}

void CSardine_State_Strafe::Exit(void* pObj)
{
	m_fCurActionDuration = 0;
	//m_fMaxActionDuration = ((rand() % 10) / 10) + 1.f;

	__super::Exit(pObj);
}

CSardine_State_Strafe* CSardine_State_Strafe::Create(void* pArg)
{
	CSardine_State_Strafe* pInstance = new CSardine_State_Strafe();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSardine_State_Strafe::Free()
{
	__super::Free();
}
