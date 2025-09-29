#include "pch.h"
#include "Splitter_State_Walk.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"


CSplitter_State_Walk::CSplitter_State_Walk()
{
}

HRESULT CSplitter_State_Walk::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Walk";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Transform")));	

	return S_OK;
}

void CSplitter_State_Walk::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::WALK, true, 0.3f);
	static_cast<CEnemy_Splitter*>(pObj)->Set_IsChase(true, 1.f);

	__super::Enter(pObj);
}

void CSplitter_State_Walk::Update(_float fTimeDelta, void* pObj)
{	
	_vector vLookTarget = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);		
	_vector vDis = vLookTarget - m_pTransform->Get_State(CTransform::STATE::POSITION);	
	
	_float fDis = XMVectorGetX(XMVector3Length(vDis));

	if (fDis <= 5 || fDis >= 30) {
		static_cast<CEnemy_Splitter*>(pObj)->Set_IsChase(false);
		static_cast<CEnemy_Splitter*>(pObj)->Select_Action();	
	}

	__super::Update(fTimeDelta, pObj);
}

void CSplitter_State_Walk::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CSplitter_State_Walk* CSplitter_State_Walk::Create(void* pObj)
{
	CSplitter_State_Walk* pInstance = new CSplitter_State_Walk();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Walk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSplitter_State_Walk::Free()
{
	__super::Free();
}
