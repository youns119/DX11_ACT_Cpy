#include "pch.h"
#include "Sardine_State_Retreat.h"
#include "GameInstance.h"
#include "Enemy_Sardine.h"


CSardine_State_Retreat::CSardine_State_Retreat()
{
}

HRESULT CSardine_State_Retreat::Initialize(void* pObj)
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
	m_vOriginalPos = static_cast<CEnemy_Sardine*>(pObj)->Get_OriginalPos();

	return S_OK;
}

void CSardine_State_Retreat::Enter(void* pObj)
{	
	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));

	m_pModel->SetUp_Animation((_uint)SARDINE_ANIM_INDEX::SWIM_FORWARD, true, 0.3f);
	static_cast<CEnemy*>(pObj)->Set_Aggroed(false);

	__super::Enter(pObj);
}

void CSardine_State_Retreat::Update(_float fTimeDelta, void* pObj)
{			
	_vector vLookTarget = m_vOriginalPos._vector();
	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	
	vPlayerPos = XMVectorSetY(vPlayerPos, 0.f);
	vLookTarget = XMVectorSetY(vLookTarget, 0.f);

	_vector vDis = vLookTarget - m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vPlayerDis = vPlayerPos - m_pTransform->Get_State(CTransform::STATE::POSITION);

	_float fDis = XMVectorGetX(XMVector3Length(vDis));
	_float fPlayerDis = XMVectorGetX(XMVector3Length(vPlayerDis));

	if (fPlayerDis < 8.f) {
		static_cast<CEnemy_Sardine*>(pObj)->Select_Action();
	}
	else if (fDis < 1.f) {
		static_cast<CEnemy_Sardine*>(pObj)->Select_Action();
	}		
	else {
		m_pMovement->TurnToDirection(vLookTarget, fTimeDelta);
		m_pTransform->Go_Straight(fTimeDelta * 1.f);
	}

	__super::Update(fTimeDelta, pObj);
}

void CSardine_State_Retreat::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CSardine_State_Retreat* CSardine_State_Retreat::Create(void* pArg)
{
	CSardine_State_Retreat* pInstance = new CSardine_State_Retreat();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSardine_State_Retreat::Free()
{
	__super::Free();
}