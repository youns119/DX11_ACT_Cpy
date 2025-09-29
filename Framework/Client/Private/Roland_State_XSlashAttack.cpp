#include "pch.h"
#include "Roland_State_XSlashAttack.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"


CRoland_State_XSlashAttack::CRoland_State_XSlashAttack()
{
}

HRESULT CRoland_State_XSlashAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "XSlash Attack";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CRoland_State_XSlashAttack::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::XSLASH, false, 0.2f);

	super::Enter(pObj);
}

void CRoland_State_XSlashAttack::Update(_float fTimeDelta, void* pObj)
{				

	if (!m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::XSLASH) || m_iAnimIndex == 0 && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		static_cast<CBoss_Roland*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_XSlashAttack::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CRoland_State_XSlashAttack* CRoland_State_XSlashAttack::Create(void* pArg)
{
	CRoland_State_XSlashAttack* pInstance = new CRoland_State_XSlashAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_XSlash");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_XSlashAttack::Free()
{
	__super::Free();
}
