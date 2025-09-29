#include "pch.h"
#include "Bowman_State_Death.h"
#include "GameInstance.h"
#include "Enemy_Bowman.h"


CBowman_State_Death::CBowman_State_Death()
{
}

HRESULT CBowman_State_Death::Initialize(void* pObj)
{
	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Bowman*>(pObj)->Find_Component(TEXT("Com_Model")));
	return S_OK;
}

void CBowman_State_Death::Enter(void* pObj)
{		
	CEnemy_Bowman* pBowman = static_cast<CEnemy_Bowman*>(pObj);
	static_cast<CCollider*>(pBowman->Find_PartObject({ TEXT("Part_RightClaw") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pBowman->Find_PartObject({ TEXT("Part_Arrow") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pBowman->Find_Component(TEXT("Com_Collider")))->Set_Active(false);	

	pBowman->Set_IsChase(false);
	pBowman->Drop_Loot();	
	pBowman->Death();

	m_pModel->SetUp_Animation((_uint)BOWMAN_ANIM_INDEX::DIE, false, 0.2f);

	CGameInstance::GetInstance()->Play_Sound(L"Bowman Death Main 1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE2, 0.5f);

	__super::Enter(pObj);
}

void CBowman_State_Death::Update(_float fTimeDelta, void* pObj)
{			
	__super::Update(fTimeDelta, pObj);
}

void CBowman_State_Death::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CBowman_State_Death* CBowman_State_Death::Create(void* pArg)
{
	CBowman_State_Death* pInstance = new CBowman_State_Death();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Walk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBowman_State_Death::Free()
{
	__super::Free();
}
