#include "pch.h"
#include "Bowman_State_Stagger.h"
#include "GameInstance.h"
#include "Enemy_Bowman.h"


CBowman_State_Stagger::CBowman_State_Stagger()
{
}

HRESULT CBowman_State_Stagger::Initialize(void* pObj)
{
	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Bowman*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CBowman_State_Stagger::Enter(void* pObj)
{		
	CEnemy_Bowman* pBowman = static_cast<CEnemy_Bowman*>(pObj);
	static_cast<CCollider*>(pBowman->Find_PartObject({ TEXT("Part_RightClaw") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pBowman->Find_PartObject({ TEXT("Part_Arrow") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);

	pBowman->Set_IsChase(false);
	m_pModel->SetUp_Animation((_uint)BOWMAN_ANIM_INDEX::HURT, false, 0.2f);

	CGameInstance::GetInstance()->Play_Sound(L"Bowman Hurt Main 3.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE2, 0.5f);
	__super::Enter(pObj);
}

void CBowman_State_Stagger::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)BOWMAN_ANIM_INDEX::HURT)) {
		static_cast<CEnemy_Bowman*>(pObj)->Select_Action();
	}
	__super::Update(fTimeDelta, pObj);
}

void CBowman_State_Stagger::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CBowman_State_Stagger* CBowman_State_Stagger::Create(void* pArg)
{
	CBowman_State_Stagger* pInstance = new CBowman_State_Stagger();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Walk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBowman_State_Stagger::Free()
{
	__super::Free();	
}
