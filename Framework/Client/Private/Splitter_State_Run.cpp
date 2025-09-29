#include "pch.h"
#include "Splitter_State_Run.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"


CSplitter_State_Run::CSplitter_State_Run()
{
}

HRESULT CSplitter_State_Run::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Run";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Transform")));	

	return S_OK;
}

void CSplitter_State_Run::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::CHARGE, true, 0.3f);
	static_cast<CEnemy_Splitter*>(pObj)->Set_IsChase(true, 1.5f);

	__super::Enter(pObj);
}

void CSplitter_State_Run::Update(_float fTimeDelta, void* pObj)
{	
	_vector vLookTarget = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);		
	_vector vDis = vLookTarget - m_pTransform->Get_State(CTransform::STATE::POSITION);	
	
	_float fDis = XMVectorGetX(XMVector3Length(vDis));

	if (fDis <= 20) {
		static_cast<CEnemy_Splitter*>(pObj)->Set_IsChase(false);
		static_cast<CEnemy_Splitter*>(pObj)->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CSplitter_State_Run::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CSplitter_State_Run* CSplitter_State_Run::Create(void* pObj)
{
	CSplitter_State_Run* pInstance = new CSplitter_State_Run();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Run");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSplitter_State_Run::Free()
{
	__super::Free();
}
