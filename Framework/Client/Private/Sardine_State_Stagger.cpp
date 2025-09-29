#include "pch.h"
#include "Sardine_State_Stagger.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Enemy_Sardine.h"


CSardine_State_Stagger::CSardine_State_Stagger()
{
}

HRESULT CSardine_State_Stagger::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Stagger";
#endif
	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	CEnemy_Sardine* pSardine = static_cast<CEnemy_Sardine*>(pObj);
	m_pModel = static_cast<CModel*>(pSardine->Find_Component(TEXT("Com_Model")));
	CTransform* m_pTransform = static_cast<CTransform*>(pSardine->Find_Component(TEXT("Com_Transform")));		

	return S_OK;
}

void CSardine_State_Stagger::Enter(void* pObj)
{		
	static_cast<CCollider*>(static_cast<CEnemy_Sardine*>(pObj)->Find_PartObject({ TEXT("Part_Horn") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(static_cast<CEnemy_Sardine*>(pObj)->Find_PartObject({ TEXT("Part_Tail") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);

	m_pModel->SetUp_Animation((_uint)SARDINE_ANIM_INDEX::HURT_REAL, false, 0.05f);
	m_pModel->Get_CurrAnimation()->Set_BaseAnimSpeed(1.5f);

	__super::Enter(pObj);
}

void CSardine_State_Stagger::Update(_float fTimeDelta, void* pObj)
{					
	if (!m_pModel->Get_IsPlaying((_uint)SARDINE_ANIM_INDEX::HURT_REAL)) {
		static_cast<CEnemy_Sardine*>(pObj)->Select_Action();
		m_pModel->Get_CurrAnimation()->Set_BaseAnimSpeed(1.f);
	}	

	__super::Update(fTimeDelta, pObj);
}

void CSardine_State_Stagger::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CSardine_State_Stagger* CSardine_State_Stagger::Create(void* pArg)
{
	CSardine_State_Stagger* pInstance = new CSardine_State_Stagger();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSardine_State_Stagger::Free()
{
	__super::Free();
}
