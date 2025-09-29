#include "pch.h"
#include "Inkerton_State_Death.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "Inkerton_Nutcracker.h"
#include "Inkerton_StretchedNutcracker.h"
#include "Player.h"
#include "UI_Manager.h"

CInkerton_State_Death::CInkerton_State_Death()
{
}

HRESULT CInkerton_State_Death::Initialize(void* pObj)
{
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CInkerton_State_Death::Enter(void* pObj)
{	
	CBoss_Inkerton* pInkerton = static_cast<CBoss_Inkerton*>(pObj);
	CInkerton_Nutcracker* pNutcracker = static_cast<CInkerton_Nutcracker*>(pInkerton->Find_PartObject({ TEXT("Part_Nutcracker") }));
	CInkerton_StretchedNutcracker* pStretchedNutcracker = static_cast<CInkerton_StretchedNutcracker*>(pInkerton->Find_PartObject({ TEXT("Part_StretchedNutcracker") }));
	static_cast<CCollider*>(pNutcracker->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pNutcracker->Find_Component(TEXT("Com_HandleCollider")))->Set_Active(false);
	static_cast<CCollider*>(pStretchedNutcracker->Find_Component(TEXT("Com_Collider")))->Set_Active(false);

	pInkerton->Set_IsChase(false);
	pInkerton->Set_IsSideMove(false);	
	//pInkerton->Drop_Loot();

	_vector vLook = static_cast<CTransform*>(pInkerton->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::LOOK);

	CPlayer* pPlayer = static_cast<CPlayer*>(pNutcracker->Get_GrabObject());
	//if (pPlayer) pPlayer->UnGrab(vLook);
	pPlayer = static_cast<CPlayer*>(pStretchedNutcracker->Get_GrabObject());
	//if (pPlayer) pPlayer->UnGrab(vLook);

	pInkerton->Death();

	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::DIE, false, 0.2f);
	
	CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Effort_Death.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 1.f);	

	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
	pUIManager->Show_Caption(L"롤랜드.. 이 망할 놈, 나도 너만큼이나 쓸모없었군..", 3.f);
	super::Enter(pObj);
}

void CInkerton_State_Death::Update(_float fTimeDelta, void* pObj)
{			
	/*if (!m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::DIE)) {
		static_cast<CBoss_Inkerton*>(pObj)->Death();
	}*/
	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_Death::Exit(void* pObj)
{	
	super::Exit(pObj);
}

CInkerton_State_Death* CInkerton_State_Death::Create(void* pArg)
{
	CInkerton_State_Death* pInstance = new CInkerton_State_Death();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_Death");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_Death::Free()
{
	__super::Free();
}