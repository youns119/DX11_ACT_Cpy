#include "pch.h"
#include "Splitter_State_Stagger.h"
#include "Animation.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"

CSplitter_State_Stagger::CSplitter_State_Stagger()
{
}

HRESULT CSplitter_State_Stagger::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Stagger";
#endif

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Model")));
		
	return S_OK;
}

void CSplitter_State_Stagger::Enter(void* pObj)
{	
	CEnemy_Splitter* pSplitter = static_cast<CEnemy_Splitter*>(pObj);
	static_cast<CCollider*>(pSplitter->Find_PartObject({ TEXT("Part_LeftClaw") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pSplitter->Find_PartObject({ TEXT("Part_Weapon") })->Find_Component(TEXT("Com_UpperCollider")))->Set_Active(false);
	static_cast<CCollider*>(pSplitter->Find_PartObject({ TEXT("Part_Weapon") })->Find_Component(TEXT("Com_LowerCollider")))->Set_Active(false);

	pSplitter->Set_IsChase(false);
	pSplitter->Set_IsGrayScaling(false);
	
	CGameInstance::GetInstance()->Play_Sound(L"Executioner_Hurt2.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.6f);

	m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::HURT, false, 0.3f);
	__super::Enter(pObj);
}

void CSplitter_State_Stagger::Update(_float fTimeDelta, void* pObj)
{
	if (!m_pModel->Get_IsPlaying((_uint)SPLITTER_ANIM_INDEX::HURT) || m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		static_cast<CEnemy_Splitter*>(pObj)->Select_Action();
	}
}

void CSplitter_State_Stagger::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CSplitter_State_Stagger* CSplitter_State_Stagger::Create(void* pObj)
{
	CSplitter_State_Stagger* pInstance = new CSplitter_State_Stagger();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Stagger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_State_Stagger::Free()
{
	__super::Free();
}
