#include "pch.h"
#include "Inkerton_State_Aggroed.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"


CInkerton_State_Aggroed::CInkerton_State_Aggroed()
{
}

HRESULT CInkerton_State_Aggroed::Initialize(void* pObj)
{
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CInkerton_State_Aggroed::Enter(void* pObj)
{			
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::IDLE, false, 0.2f);
	static_cast<CBoss_Inkerton*>(pObj)->Set_IsChase(true, 0.f);
	super::Enter(pObj);
}

void CInkerton_State_Aggroed::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::IDLE)) {
		CBoss_Inkerton* pInkerton = static_cast<CBoss_Inkerton*>(pObj);
		pInkerton->Set_IsChase(false);
		pInkerton->Set_Aggroed(true);
		pInkerton->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_Aggroed::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CInkerton_State_Aggroed* CInkerton_State_Aggroed::Create(void* pArg)
{
	CInkerton_State_Aggroed* pInstance = new CInkerton_State_Aggroed();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_Aggroed");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CInkerton_State_Aggroed::Free()
{
	__super::Free();
}
