#include "pch.h"
#include "Sardine_State_Hooked.h"
#include "GameInstance.h"
#include "Enemy_Sardine.h"


CSardine_State_Hooked::CSardine_State_Hooked()
{
}

HRESULT CSardine_State_Hooked::Initialize(void* pObj)
{
	m_strStateName = "Hooked";

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	m_pSardine = static_cast<CEnemy_Sardine*>(pObj);
	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Sardine*>(pObj)->Find_Component(TEXT("Com_Model")));	
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	m_pTransform = static_cast<CTransform*>(m_pSardine->Find_Component(TEXT("Com_Transform")));

	return S_OK;
}

void CSardine_State_Hooked::Enter(void* pObj)
{	
	m_pSardine->Set_Hooked(true);

	static_cast<CCollider*>(m_pSardine->Find_PartObject({ TEXT("Part_Horn") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(m_pSardine->Find_PartObject({ TEXT("Part_Tail") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	m_pSardine->Set_IsChase(false);

	m_pModel->SetUp_Animation((_uint)SARDINE_ANIM_INDEX::HOOKED, true, 0.2f);	

	m_pTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE::POSITION));

	CGameInstance::GetInstance()->Play_Sound(L"Sardine Death 2.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	__super::Enter(pObj);
}

void CSardine_State_Hooked::Update(_float fTimeDelta, void* pObj)
{		
	if (!m_pSardine->IsHooked()) {
		m_pSardine->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CSardine_State_Hooked::Exit(void* pObj)
{
	_vector temp = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	temp = XMVectorSetY(temp, 0.f);
	m_pTransform->LookAt(temp);

	__super::Exit(pObj);
}

CSardine_State_Hooked* CSardine_State_Hooked::Create(void* pArg)
{
	CSardine_State_Hooked* pInstance = new CSardine_State_Hooked();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSardine_State_Hooked::Free()
{
	__super::Free();
}
