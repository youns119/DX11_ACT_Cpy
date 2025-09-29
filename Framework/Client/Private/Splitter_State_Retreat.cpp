#include "pch.h"
#include "Splitter_State_Retreat.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"

//Engine
#include "Transform.h"
#include "Movement.h"

CSplitter_State_Retreat::CSplitter_State_Retreat()
{
}

HRESULT CSplitter_State_Retreat::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Retreat";
#endif


	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")));
	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));
	m_vOriginalPos = static_cast<CEnemy_Splitter*>(pObj)->Get_OriginalPos();

	return S_OK;
}

void CSplitter_State_Retreat::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::WALK, true, 0.3f);
	static_cast<CEnemy*>(pObj)->Set_Aggroed(false);

	__super::Enter(pObj);
}

void CSplitter_State_Retreat::Update(_float fTimeDelta, void* pObj)
{	
	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_vector vDis = m_vOriginalPos._vector() - m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vPlayerDis = vPlayerPos - m_pTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = XMVectorGetX(XMVector3Length(vDis));
	_float fPlayerDis = XMVectorGetX(XMVector3Length(vPlayerDis));

	if (fPlayerDis < 8.f) {
		static_cast<CEnemy_Splitter*>(pObj)->Select_Action();
	}
	else if (fDis < 2.5f) {
		static_cast<CEnemy_Splitter*>(pObj)->Select_Action();		
	}
	else {
		m_pMovement->TurnToDirection(m_vOriginalPos._vector(), fTimeDelta);
		m_pTransform->Go_Straight(fTimeDelta * 1.f);
	}

	__super::Update(fTimeDelta, pObj);
}

void CSplitter_State_Retreat::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CSplitter_State_Retreat* CSplitter_State_Retreat::Create(void* pObj)
{
	CSplitter_State_Retreat* pInstance = new CSplitter_State_Retreat();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Retreat");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSplitter_State_Retreat::Free()
{
	__super::Free();
}
