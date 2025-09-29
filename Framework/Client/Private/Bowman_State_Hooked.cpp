#include "pch.h"
#include "Bowman_State_Hooked.h"
#include "GameInstance.h"
#include "Enemy_Bowman.h"


CBowman_State_Hooked::CBowman_State_Hooked()
{
}

HRESULT CBowman_State_Hooked::Initialize(void* pObj)
{
	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	m_pBowman = static_cast<CEnemy_Bowman*>(pObj);
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	m_pTransform = static_cast<CTransform*>(m_pBowman->Find_Component(TEXT("Com_Transform")));

	m_pModel = static_cast<CModel*>(m_pBowman->Find_Component(TEXT("Com_Model")));
	return S_OK;
}

void CBowman_State_Hooked::Enter(void* pObj)
{		
	m_pBowman->Set_Hooked(true);
	m_pBowman->Set_IsChase(false);
	static_cast<CCollider*>(m_pBowman->Find_PartObject({ TEXT("Part_RightClaw") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(m_pBowman->Find_PartObject({ TEXT("Part_Arrow") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	
	m_pModel->SetUp_Animation((_uint)BOWMAN_ANIM_INDEX::HOOKED, true, 0.2f);

	_vector temp = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector sour = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	sour = XMVectorSetY(sour, XMVectorGetY(temp));
	m_pTransform->LookAt(sour);

	CGameInstance::GetInstance()->Play_Sound(L"Bowman Hurt Main 4.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE2, 0.5f);
	__super::Enter(pObj);
}

void CBowman_State_Hooked::Update(_float fTimeDelta, void* pObj)
{					
	if (!m_pBowman->IsHooked()) {
		m_pBowman->Select_Action();
	}
	__super::Update(fTimeDelta, pObj);
}

void CBowman_State_Hooked::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CBowman_State_Hooked* CBowman_State_Hooked::Create(void* pArg)
{
	CBowman_State_Hooked* pInstance = new CBowman_State_Hooked();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Walk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBowman_State_Hooked::Free()
{
	__super::Free();	
}
