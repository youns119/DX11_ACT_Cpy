#include "pch.h"
#include "Bowman_State_Capsize.h"
#include "GameInstance.h"
#include "Enemy_Bowman.h"


CBowman_State_Capsize::CBowman_State_Capsize()
{
}

HRESULT CBowman_State_Capsize::Initialize(void* pObj)
{
	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	m_pBowman = static_cast<CEnemy_Bowman*>(pObj);
	m_pModel = static_cast<CModel*>(m_pBowman->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CBowman_State_Capsize::Enter(void* pObj)
{	
	static_cast<CCollider*>(m_pBowman->Find_PartObject({ TEXT("Part_RightClaw") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(m_pBowman->Find_PartObject({ TEXT("Part_Arrow") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);

	m_pBowman->Set_IsChase(false);
	m_pModel->SetUp_Animation((_uint)BOWMAN_ANIM_INDEX::CAPSIZE, false, 0.2f);
	m_pBowman->Set_IKOperation(false);
	m_iAnimIndex = (_uint)BOWMAN_ANIM_INDEX::CAPSIZE;
	__super::Enter(pObj);
}

void CBowman_State_Capsize::Update(_float fTimeDelta, void* pObj)
{				
	if (m_iAnimIndex == (_uint)BOWMAN_ANIM_INDEX::CAPSIZE && !m_pModel->Get_IsPlaying((_uint)BOWMAN_ANIM_INDEX::CAPSIZE)) {
		m_pModel->SetUp_Animation((_uint)BOWMAN_ANIM_INDEX::CAPSIZE_IDLE, true, 0.2f);
		m_iAnimIndex = (_uint)BOWMAN_ANIM_INDEX::CAPSIZE_IDLE;
	}
	else if (m_iAnimIndex == (_uint)BOWMAN_ANIM_INDEX::CAPSIZE_IDLE && Decrease_CapsizedDuration(fTimeDelta) && !m_pBowman->IsFinisher()) {
		m_pModel->SetUp_Animation((_uint)BOWMAN_ANIM_INDEX::CAPSIZE_EXIT, false, 0.2f);
		m_iAnimIndex = (_uint)BOWMAN_ANIM_INDEX::CAPSIZE_EXIT;		
		m_pBowman->Set_Capsized(false);
	}
	else if (m_iAnimIndex == (_uint)BOWMAN_ANIM_INDEX::CAPSIZE_EXIT && !m_pModel->Get_IsPlaying((_uint)BOWMAN_ANIM_INDEX::CAPSIZE_EXIT)) {			
		m_pBowman->Reset_Balance();
		m_pBowman->Select_Action();
		m_pBowman->Set_IKOperation(true);
	}

	__super::Update(fTimeDelta, pObj);
}

void CBowman_State_Capsize::Exit(void* pObj)
{
	m_pBowman->Set_Finished(false);
	m_fCapsizeTime = 0;
	__super::Exit(pObj);
}

_bool CBowman_State_Capsize::Decrease_CapsizedDuration(_float fTimeDelta)
{
	if (m_fCapsizeTime > m_fCapsizeDuration)
		return true;
	else
		m_fCapsizeTime += fTimeDelta;

	return false;
}

CBowman_State_Capsize* CBowman_State_Capsize::Create(void* pArg)
{
	CBowman_State_Capsize* pInstance = new CBowman_State_Capsize();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Walk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBowman_State_Capsize::Free()
{
	__super::Free();
}
