#include "pch.h"
#include "Pagurus_State_Patrol.h"
#include "GameInstance.h"
#include "Boss_Pagurus.h"


CPagurus_State_Patrol::CPagurus_State_Patrol()
{
}

HRESULT CPagurus_State_Patrol::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Retreat";
#endif
	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pPagurus = static_cast<CBoss_Pagurus*>(pObj);
	m_pModel = static_cast<CModel*>(m_pPagurus->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(m_pPagurus->Find_Component(TEXT("Com_Transform")));

	m_pParolPoints = static_cast<CBoss_Pagurus*>(pObj)->Get_PatrolPoints();
	if(m_pParolPoints.size() > 0)
		m_vTargetPos = m_pParolPoints[m_iNextPosIndex];

	return S_OK;
}

void CPagurus_State_Patrol::Enter(void* pObj)
{	
	m_pMovement = static_cast<CMovement*>(m_pPagurus->Find_Component(TEXT("Com_Movement")));

	m_pModel->SetUp_Animation((_uint)PAGURUS_ANIM_INDEX::IDLE, true, 0.3f);

	__super::Enter(pObj);
}

void CPagurus_State_Patrol::Update(_float fTimeDelta, void* pObj)
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
	
	if (m_pPagurus->Detect_Player()) {
		m_pPagurus->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CPagurus_State_Patrol::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CPagurus_State_Patrol* CPagurus_State_Patrol::Create(void* pArg)
{
	CPagurus_State_Patrol* pInstance = new CPagurus_State_Patrol();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPagurus_State_Patrol::Free()
{
	__super::Free();
}