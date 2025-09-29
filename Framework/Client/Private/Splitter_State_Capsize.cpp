#include "pch.h"
#include "Splitter_State_Capsize.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"
#include "Player.h"
#include "Splitter_LeftClaw.h"

CSplitter_State_Capsize::CSplitter_State_Capsize()
{
}

HRESULT CSplitter_State_Capsize::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Capsize";
#endif

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CSplitter_State_Capsize::Enter(void* pObj)
{	
	m_pSplitter = static_cast<CEnemy_Splitter*>(pObj);
	CSplitter_LeftClaw* m_pClaw = static_cast<CSplitter_LeftClaw*>(m_pSplitter->Find_PartObject({ TEXT("Part_LeftClaw" })));
	m_pClaw->Get_Collider()->Set_Active(false);
	static_cast<CCollider*>(m_pSplitter->Find_PartObject({ TEXT("Part_Weapon") })->Find_Component(TEXT("Com_UpperCollider")))->Set_Active(false);
	static_cast<CCollider*>(m_pSplitter->Find_PartObject({ TEXT("Part_Weapon") })->Find_Component(TEXT("Com_LowerCollider")))->Set_Active(false);

	m_pSplitter->Set_IsChase(false);
	m_pSplitter->Set_IsUnblockable(false);
	m_pSplitter->Set_IsGrayScaling(false);

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pClaw->Get_GrabObject());
	//if (pPlayer)
	//	pPlayer->UnGrab(static_cast<CTransform*>(m_pSplitter->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::LOOK));
	
	CGameInstance::GetInstance()->Play_Sound(L"Executioner_Proned.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::CAPSIZE, false, 0.3f);
	m_iAnimIndex = (_uint)SPLITTER_ANIM_INDEX::CAPSIZE;

	__super::Enter(pObj);
}

void CSplitter_State_Capsize::Update(_float fTimeDelta, void* pObj)
{
	if (m_iAnimIndex == (_uint)SPLITTER_ANIM_INDEX::CAPSIZE && Decrease_CapsizedDuration(fTimeDelta) && !m_pSplitter->IsFinisher()) {
		m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::CAPSIZE_EXIT, false, 0.2f);
		m_iAnimIndex = (_uint)SPLITTER_ANIM_INDEX::CAPSIZE_EXIT;
		m_pSplitter->Set_Capsized(false);
	}
	else if (m_iAnimIndex == (_uint)SPLITTER_ANIM_INDEX::CAPSIZE_EXIT && !m_pModel->Get_IsPlaying((_uint)SPLITTER_ANIM_INDEX::CAPSIZE_EXIT)) {
		m_pSplitter->Reset_Balance();
		m_pSplitter->Select_Action();
	}
}

void CSplitter_State_Capsize::Exit(void* pObj)
{
	m_pSplitter->Set_Finished(false);	
	m_fCapsizeTime = 0;
	__super::Exit(pObj);
}

_bool CSplitter_State_Capsize::Decrease_CapsizedDuration(_float fTimeDelta)
{
	if (m_fCapsizeTime > m_fCapsizeDuration)
		return true;
	else
		m_fCapsizeTime += fTimeDelta;

	return false;
}

CSplitter_State_Capsize* CSplitter_State_Capsize::Create(void* pObj)
{
	CSplitter_State_Capsize* pInstance = new CSplitter_State_Capsize();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Capsize");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSplitter_State_Capsize::Free()
{
	__super::Free();
}