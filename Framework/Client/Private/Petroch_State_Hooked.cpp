#include "pch.h"
#include "Petroch_State_Hooked.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"
#include "Animation.h"


CPetroch_State_Hooked::CPetroch_State_Hooked()
{
}

HRESULT CPetroch_State_Hooked::Initialize(void* pObj)
{
	m_strStateName = "Stagger";

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	m_pPetroch = static_cast<CBoss_Petroch*>(pObj);
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	m_pTransform = static_cast<CTransform*>(m_pPetroch->Find_Component(TEXT("Com_Transform")));

	return S_OK;
}

void CPetroch_State_Hooked::Enter(void* pObj)
{	
	static_cast<CCollider*>(m_pPetroch->Find_PartObject({ TEXT("Part_Shell") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(m_pPetroch->Find_PartObject({ TEXT("Part_LeftClaw") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);

	m_pPetroch->Set_Hooked(true);
	m_pPetroch->Set_IsChase(false);
	m_pPetroch->Set_IsSideMove(false);

	m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::HOOKED, true, 0.2f);	
	CGameInstance::GetInstance()->Play_Sound(L"PetrochVO_Hurt4.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);

	_vector temp = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector sour = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	sour = XMVectorSetY(sour, XMVectorGetY(temp));
	m_pTransform->LookAt(sour);

	super::Enter(pObj);
}

void CPetroch_State_Hooked::Update(_float fTimeDelta, void* pObj)
{
	if (!m_pPetroch->IsHooked()) {
		m_pPetroch->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}


void CPetroch_State_Hooked::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CPetroch_State_Hooked* CPetroch_State_Hooked::Create(void* pArg)
{
	CPetroch_State_Hooked* pInstance = new CPetroch_State_Hooked();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CPetroch_State_Hooked");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CPetroch_State_Hooked::Free()
{
	__super::Free();
}
