#include "pch.h"
#include "Splitter_State_Hooked.h"
#include "Animation.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"

CSplitter_State_Hooked::CSplitter_State_Hooked()
{
}

HRESULT CSplitter_State_Hooked::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Hooked";
#endif
	m_pSplitter = static_cast<CEnemy_Splitter*>(pObj);
	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Splitter*>(pObj)->Find_Component(TEXT("Com_Model")));

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	m_pTransform = static_cast<CTransform*>(m_pSplitter->Find_Component(TEXT("Com_Transform")));
	return S_OK;
}

void CSplitter_State_Hooked::Enter(void* pObj)
{		
	static_cast<CCollider*>(m_pSplitter->Find_PartObject({ TEXT("Part_LeftClaw") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(m_pSplitter->Find_PartObject({ TEXT("Part_Weapon") })->Find_Component(TEXT("Com_UpperCollider")))->Set_Active(false);
	static_cast<CCollider*>(m_pSplitter->Find_PartObject({ TEXT("Part_Weapon") })->Find_Component(TEXT("Com_LowerCollider")))->Set_Active(false);

	m_pSplitter->Set_Hooked(true);
	m_pSplitter->Set_IsChase(false);
	m_pSplitter->Set_IsGrayScaling(false);	

	m_pModel->SetUp_Animation((_uint)SPLITTER_ANIM_INDEX::HOOKED, true, 0.3f);	

	_vector temp = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector sour = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	sour = XMVectorSetY(sour, XMVectorGetY(temp));
	m_pTransform->LookAt(sour);

	CGameInstance::GetInstance()->Play_Sound(L"Executioner_Hurt3.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	__super::Enter(pObj);
}

void CSplitter_State_Hooked::Update(_float fTimeDelta, void* pObj)
{
	if (!m_pSplitter->IsHooked()) {
		m_pSplitter->Select_Action();
	}
}

void CSplitter_State_Hooked::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CSplitter_State_Hooked* CSplitter_State_Hooked::Create(void* pObj)
{
	CSplitter_State_Hooked* pInstance = new CSplitter_State_Hooked();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Hooked");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_State_Hooked::Free()
{
	__super::Free();
}
