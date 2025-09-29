#include "pch.h"
#include "Inkerton_State_Hooked.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"


CInkerton_State_Hooked::CInkerton_State_Hooked()
{
}

HRESULT CInkerton_State_Hooked::Initialize(void* pObj)
{
	m_pInkerton = static_cast<CBoss_Inkerton*>(pObj);
	m_pModel = static_cast<CModel*>(m_pInkerton->Find_Component(TEXT("Com_Model")));


	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	m_pTransform = static_cast<CTransform*>(m_pInkerton->Find_Component(TEXT("Com_Transform")));

	return S_OK;
}


void CInkerton_State_Hooked::Enter(void* pObj)
{		
	static_cast<CCollider*>(m_pInkerton->Find_PartObject({ TEXT("Part_Nutcracker") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(m_pInkerton->Find_PartObject({ TEXT("Part_Nutcracker") })->Find_Component(TEXT("Com_HandleCollider")))->Set_Active(false);
	static_cast<CCollider*>(m_pInkerton->Find_PartObject({ TEXT("Part_StretchedNutcracker") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);

	m_pInkerton->Set_Hooked(true);
	m_pInkerton->Set_IsChase(false);
	m_pInkerton->Set_IsSideMove(false);

	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::FISHING_PARRY, true, 0.2f);
	
	_vector temp = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector sour = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	sour = XMVectorSetY(sour, XMVectorGetY(temp));
	m_pTransform->LookAt(sour);

	CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Effort_Prone_2.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE2, 0.5f);

	super::Enter(pObj);
}

void CInkerton_State_Hooked::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pInkerton->IsHooked()) {
		static_cast<CBoss_Inkerton*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_Hooked::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CInkerton_State_Hooked* CInkerton_State_Hooked::Create(void* pArg)
{
	CInkerton_State_Hooked* pInstance = new CInkerton_State_Hooked();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_Hooked");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_Hooked::Free()
{
	__super::Free();
}
