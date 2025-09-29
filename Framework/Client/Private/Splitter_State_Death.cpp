#include "pch.h"
#include "Splitter_State_Death.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"
#include "Player.h"
#include "Splitter_LeftClaw.h"

CSplitter_State_Death::CSplitter_State_Death()
{
}

HRESULT CSplitter_State_Death::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Death";
#endif

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Model")));	

	return S_OK;
}

void CSplitter_State_Death::Enter(void* pObj)
{				
	CEnemy_Splitter* pSplitter = static_cast<CEnemy_Splitter*>(pObj);
	CSplitter_LeftClaw* m_pClaw = static_cast<CSplitter_LeftClaw*>(pSplitter->Find_PartObject({ TEXT("Part_LeftClaw" })));
	m_pClaw->Get_Collider()->Set_Active(false);
	static_cast<CCollider*>(pSplitter->Find_Component(TEXT("Com_Collider")))->Set_Active(false);	
	static_cast<CCollider*>(pSplitter->Find_PartObject({ TEXT("Part_Weapon") })->Find_Component(TEXT("Com_UpperCollider")))->Set_Active(false);
	static_cast<CCollider*>(pSplitter->Find_PartObject({ TEXT("Part_Weapon") })->Find_Component(TEXT("Com_LowerCollider")))->Set_Active(false);

	pSplitter->Set_IsChase(false);
	pSplitter->Drop_Loot();
	pSplitter->Death();
	pSplitter->Set_IsGrayScaling(false);
	pSplitter->Set_IsUnblockable(false);

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pClaw->Get_GrabObject());
	//if(pPlayer)
	//	pPlayer->UnGrab(static_cast<CTransform*>(pSplitter->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::LOOK));

	CGameInstance::GetInstance()->Play_Sound(L"Executioner_Die4.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::DIE, false, 0.3f);
	__super::Enter(pObj);
}

void CSplitter_State_Death::Update(_float fTimeDelta, void* pObj)
{
	super::Update(fTimeDelta, pObj);
}

void CSplitter_State_Death::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CSplitter_State_Death* CSplitter_State_Death::Create(void* pObj)
{
	CSplitter_State_Death* pInstance = new CSplitter_State_Death();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Death");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSplitter_State_Death::Free()
{
	__super::Free();
}
