#include "pch.h"
#include "Inkerton_State_Retreat.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "Animation.h"

#include "UI_Manager.h"

CInkerton_State_Retreat::CInkerton_State_Retreat()
{
}

HRESULT CInkerton_State_Retreat::Initialize(void* pObj)
{
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")));	

	static_cast<CBoss_Inkerton*>(pObj)->Set_IsChase(false);
	static_cast<CBoss_Inkerton*>(pObj)->Set_IsSideMove(false);
	return S_OK;
}

void CInkerton_State_Retreat::Enter(void* pObj)
{	
	static_cast<CCollider*>(static_cast<CBoss_Inkerton*>(pObj)->Find_PartObject({ TEXT("Part_Nutcracker") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(static_cast<CBoss_Inkerton*>(pObj)->Find_PartObject({ TEXT("Part_Nutcracker") })->Find_Component(TEXT("Com_HandleCollider")))->Set_Active(false);
	static_cast<CCollider*>(static_cast<CBoss_Inkerton*>(pObj)->Find_PartObject({ TEXT("Part_StretchedNutcracker") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(static_cast<CBoss_Inkerton*>(pObj)->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::PEACE_OUT, false, 0.2f);

	CGameInstance::GetInstance()->Play_Sound(L"NotWorth.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	CGameInstance::GetInstance()->FadeSwap_Sound(L"Field_BGM.wav", (_uint)SOUND_CHANNEL::BGM, 0.1f, 0.5f);

	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
	pUIManager->Show_Caption(L"ÃÂ, ½Ã°£³¶ºñ±º", 2.f);

	super::Enter(pObj);
}

void CInkerton_State_Retreat::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::PEACE_OUT) || (m_iAnimIndex == 0 && m_pModel->Get_CurrAnimation()->Get_Blendable())) {
		m_iAnimIndex = 1;
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::IDLE, true, 0.2f);
		if (m_fCurRetreatTime > m_fMaxRetreatTime) {
			static_cast<CBoss_Inkerton*>(pObj)->Deactive();
			return;
		}
		else
			m_fCurRetreatTime += fTimeDelta;

		m_fSacle = m_pTransform->Get_Scale();
		m_fSacle = _float3(m_fSacle.x - m_fScaleDecreaseAmount, m_fSacle.y - m_fScaleDecreaseAmount, m_fSacle.z - m_fScaleDecreaseAmount);
		m_pTransform->Set_Scale(m_fSacle);
		m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), fTimeDelta * 2.5f);
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_Retreat::Exit(void* pObj)
{
	super::Exit(pObj);
}

CInkerton_State_Retreat* CInkerton_State_Retreat::Create(void* pArg)
{
	CInkerton_State_Retreat* pInstance = new CInkerton_State_Retreat();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_Retreat");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_Retreat::Free()
{
	__super::Free();
}
