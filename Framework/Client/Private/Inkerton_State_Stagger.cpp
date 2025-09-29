#include "pch.h"
#include "Inkerton_State_Stagger.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"


CInkerton_State_Stagger::CInkerton_State_Stagger()
{
}

HRESULT CInkerton_State_Stagger::Initialize(void* pObj)
{
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}


void CInkerton_State_Stagger::Enter(void* pObj)
{		
	static_cast<CCollider*>(static_cast<CBoss_Inkerton*>(pObj)->Find_PartObject({ TEXT("Part_Nutcracker") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(static_cast<CBoss_Inkerton*>(pObj)->Find_PartObject({ TEXT("Part_Nutcracker") })->Find_Component(TEXT("Com_HandleCollider")))->Set_Active(false);
	static_cast<CCollider*>(static_cast<CBoss_Inkerton*>(pObj)->Find_PartObject({ TEXT("Part_StretchedNutcracker") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);

	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::HURT, false, 0.2f);

	super::Enter(pObj);
}

void CInkerton_State_Stagger::Update(_float fTimeDelta, void* pObj)
{			
	if (!m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::HURT)) {
		static_cast<CBoss_Inkerton*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_Stagger::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CInkerton_State_Stagger* CInkerton_State_Stagger::Create(void* pArg)
{
	CInkerton_State_Stagger* pInstance = new CInkerton_State_Stagger();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_Stagger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_Stagger::Free()
{
	__super::Free();
}
