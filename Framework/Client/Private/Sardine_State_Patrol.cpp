#include "pch.h"
#include "Sardine_State_Patrol.h"
#include "GameInstance.h"
#include "Enemy_Sardine.h"


CSardine_State_Patrol::CSardine_State_Patrol()
{
}

HRESULT CSardine_State_Patrol::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Retreat";
#endif
	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")));	

	m_pParolPoints = static_cast<CEnemy_Sardine*>(pObj)->Get_PatrolPoints();
	if(m_pParolPoints.size() > 0)
		m_vTargetPos = m_pParolPoints[m_iNextPosIndex];

	return S_OK;
}

void CSardine_State_Patrol::Enter(void* pObj)
{	
	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));

	m_pModel->SetUp_Animation((_uint)SARDINE_ANIM_INDEX::SWIM_FORWARD, true, 0.3f);

	__super::Enter(pObj);
}

void CSardine_State_Patrol::Update(_float fTimeDelta, void* pObj)
{			
	_vector vLookTarget = m_vTargetPos._vector();
	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);

	vPos = XMVectorSetY(vPos, 0.f);	
	vPlayerPos = XMVectorSetY(vPlayerPos, 0.f);
	vLookTarget = XMVectorSetY(vLookTarget, 0.f);		

	_vector vPointDis = vLookTarget - vPos;
	_vector vPlayerDis = vPlayerPos - vPos;

	_float fDis = XMVectorGetX(XMVector3Length(vPointDis));
	_float fPlayerDis = XMVectorGetX(XMVector3Length(vPlayerDis));

	if (fDis < 1.f) {
		m_iNextPosIndex++;
		m_iNextPosIndex %= m_pParolPoints.size();
		m_vTargetPos = m_pParolPoints[m_iNextPosIndex];
	}	
	else {
		m_pMovement->TurnToDirection(vLookTarget, fTimeDelta * 0.8f);
		m_pTransform->Go_Straight(fTimeDelta * 0.8f);
	}

	__super::Update(fTimeDelta, pObj);
}

void CSardine_State_Patrol::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CSardine_State_Patrol* CSardine_State_Patrol::Create(void* pArg)
{
	CSardine_State_Patrol* pInstance = new CSardine_State_Patrol();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSardine_State_Patrol::Free()
{
	__super::Free();
}