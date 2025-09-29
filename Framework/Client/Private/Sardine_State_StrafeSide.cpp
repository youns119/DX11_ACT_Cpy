 #include "pch.h"
#include "Sardine_State_StrafeSide.h"
#include "GameInstance.h"
#include "Enemy_Sardine.h"


CSardine_State_StrafeSide::CSardine_State_StrafeSide()
{
}

HRESULT CSardine_State_StrafeSide::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Strage Side";
#endif
	m_iPriorityLevel = 2;
	m_fMaxCoolTime = 0.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Sardine*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")));

	return S_OK;
}

void CSardine_State_StrafeSide::Enter(void* pObj)
{		
	m_pMovement = static_cast<CMovement*>(static_cast<CEnemy_Sardine*>(pObj)->Find_Component(TEXT("Com_Movement")));
	 
	_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vRight = m_pTransform->Get_State(CTransform::STATE::RIGHT);

	m_fStrafeSpeed = ((rand() % 20) / 10.f) - 1;
	m_fStrafeSpeed = m_fStrafeSpeed == 0 ? m_fStrafeSpeed++ : m_fStrafeSpeed;
	m_vLookTarget = (vPosition * (XMVector3Normalize(vRight) * (m_fStrafeSpeed * 1000)));
	
	m_pModel->SetUp_Animation((_uint)SARDINE_ANIM_INDEX::SWIM_FORWARD, true, 0.01f);

	__super::Enter(pObj);
} 

void CSardine_State_StrafeSide::Update(_float fTimeDelta, void* pObj)
{			
	m_pMovement->TurnToDirection(m_vLookTarget._vector(), fTimeDelta);
	m_pTransform->Go_Straight(fTimeDelta * 1.2f);

	if (m_fCurActionDuration > m_fMaxActionDuration)
		static_cast<CEnemy_Sardine*>(pObj)->Select_Action();
	else
		m_fCurActionDuration += fTimeDelta;

	__super::Update(fTimeDelta, pObj);
}

void CSardine_State_StrafeSide::Exit(void* pObj)
{
	m_fCurActionDuration = 0;
	//m_fMaxActionDuration = ((rand() % 10) / 10) + 1.f;

	__super::Exit(pObj);
}

CSardine_State_StrafeSide* CSardine_State_StrafeSide::Create(void* pArg)
{
	CSardine_State_StrafeSide* pInstance = new CSardine_State_StrafeSide();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSardine_State_StrafeSide::Free()
{
	__super::Free();
}