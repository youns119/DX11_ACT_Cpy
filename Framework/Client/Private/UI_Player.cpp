#include "pch.h"
#include "UI_Player.h"

#include "GameInstance.h"

#include "FPlayerStat.h"
#include "FShell.h"
#include "UI_Manager.h"

#include "UI_PlayerLeft.h"
#include "UI_PlayerRight.h"
#include "UI_Compass.h"
#include "UI_Hurt.h"
#include "UI_Hide.h"
#include "UI_Ring.h"
#include "UI_Grapple.h"
#include "UI_Finish.h"
#include "UI_Dialogue.h"
#include "UI_Menu.h"
#include "UI_Debuff.h"

CUI_Player::CUI_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Player::CUI_Player(const CUI_Player& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_Player::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Player::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	pDesc->UIObjectDesc.fSizeX = g_iWindowSizeX;
	pDesc->UIObjectDesc.fSizeY = g_iWindowSizeY;
	pDesc->UIObjectDesc.fX = g_iWindowSizeX * 0.5f;
	pDesc->UIObjectDesc.fY = g_iWindowSizeY * 0.5f;
	pDesc->UIObjectDesc.fZ = 0.01f;

	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(CUIObject::Initialize(&pDesc->UIObjectDesc)))
		return E_FAIL;

	m_pPlayerStat = pDesc->pPlayerStat;
	m_ppShellStat = pDesc->ppShellStat;
	m_ppGrappleTarget = pDesc->ppGrappleTarget;
	m_ppFinishTarget = pDesc->ppFinishTarget;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	return S_OK;
}

void CUI_Player::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_Player::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CUI_Player::Late_Update(_float fTimeDelta)
{
	if (m_pUIManager->Get_DialogueUI_IsActive() || m_pUIManager->Get_CutSceneUI_IsActive() || m_pUIManager->Get_IsActive_CreditUI())
		return;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);

	super::Late_Update(fTimeDelta);
}

HRESULT CUI_Player::Render()
{
    return S_OK;
}

HRESULT CUI_Player::Ready_PartObject()
{
	/* Compass */
	CUI_Compass::DESC CompassDesc{};
	{
		CompassDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Compass.dat";
		CompassDesc.pPlayerStat = m_pPlayerStat;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Compass", LEVEL_STATIC, L"Prototype_GameObject_UI_Compass", &CompassDesc)))
		return E_FAIL;

	/* PlayerLeft */
	CUI_PlayerLeft::DESC LeftDesc{};
	{
		LeftDesc.pPlayerStat = m_pPlayerStat;
		LeftDesc.ppShellStat = m_ppShellStat;
	}
	if (FAILED(Add_PartObject(L"Part_UI_PlayerLeft", LEVEL_STATIC, L"Prototype_GameObject_UI_PlayerLeft", &LeftDesc)))
		return E_FAIL;

	/* PlayerRight */
	CUI_PlayerRight::DESC RightDesc{};
	{
		RightDesc.pPlayerStat = m_pPlayerStat;
	}
	if (FAILED(Add_PartObject(L"Part_UI_PlayerRight", LEVEL_STATIC, L"Prototype_GameObject_UI_PlayerRight", &RightDesc)))
		return E_FAIL;

	/* Hurt */
	CUI_Hurt::DESC HurtDesc{};
	{
		HurtDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_HurtBorder.dat";

		HurtDesc.pCurHp = &m_pPlayerStat->fCurrHP;
		HurtDesc.pMaxHp = &m_pPlayerStat->fMaxHP;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Hurt", LEVEL_STATIC, L"Prototype_GameObject_UI_Hurt", &HurtDesc)))
		return E_FAIL;

	CUI_Hide::DESC HideDesc{};
	{
		HideDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_HideBorder.dat";

		HideDesc.pPlayerStat = m_pPlayerStat;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Hide", LEVEL_STATIC, L"Prototype_GameObject_UI_Hide", &HideDesc)))
		return E_FAIL;

	CUI_Ring::DESC RingDesc{};
	{
		RingDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Ring.dat";

		RingDesc.pInAir = &m_pPlayerStat->bInAir;
		RingDesc.pGrappling = &m_pPlayerStat->bGrappling;
		RingDesc.pSwimmingTimeAcc = &m_pPlayerStat->fSwimmingTimeAcc;
		RingDesc.pSwimmingMaxTime = &m_pPlayerStat->fSwimmingMaxTime;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Ring", LEVEL_STATIC, L"Prototype_GameObject_UI_Ring", &RingDesc)))
		return E_FAIL;

	CUI_Grapple::DESC GrappleDesc{};
	{
		GrappleDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Grapple.dat";
		GrappleDesc.pPlayerStat = m_pPlayerStat;
		GrappleDesc.ppGrappleTarget = m_ppGrappleTarget;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Grapple", LEVEL_STATIC, L"Prototype_GameObject_UI_Grapple", &GrappleDesc)))
		return E_FAIL;

	CUI_Finish::DESC FinishDesc{};
	{
		FinishDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Finish.dat";
		FinishDesc.pPlayerStat = m_pPlayerStat;
		FinishDesc.ppFinishTarget = m_ppFinishTarget;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Finish", LEVEL_STATIC, L"Prototype_GameObject_UI_Finish", &FinishDesc)))
		return E_FAIL;

	CUI_Debuff::DESC DebuffDesc{};
	{
		DebuffDesc.pPlayerStat = m_pPlayerStat;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Debuff", LEVEL_STATIC, L"Prototype_GameObject_UI_Debuff", &DebuffDesc)))
		return E_FAIL;

	CUI_Menu::DESC MenuDesc{};
	{
		MenuDesc.pPlayerStat = m_pPlayerStat;
		MenuDesc.ppShellStat = m_ppShellStat;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Menu", LEVEL_STATIC, L"Prototype_GameObject_UI_Menu", &MenuDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_Player* CUI_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Player* pInstance = new CUI_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Player::Clone(void* pArg)
{
	CUI_Player* pInstance = new CUI_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Player::Free()
{
    CUIObject::Free();
}

