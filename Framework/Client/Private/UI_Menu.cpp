#include "pch.h"
#include "UI_Menu.h"

#include "GameInstance.h"
#include "UI_Manager.h"

#include "UI_DefaultStretch.h"
#include "UI_Button.h"
#include "UI_Shake.h"
#include "UI_DynamicTexture.h"

#include "UI_Inventory.h"
#include "UI_Status.h"
#include "UI_Option.h"

CUI_Menu::CUI_Menu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Menu::CUI_Menu(const CUI_Menu& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_Menu::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Menu::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	pDesc->UIObjectDesc.fSizeX = g_iWindowSizeX;
	pDesc->UIObjectDesc.fSizeY = g_iWindowSizeY;
	pDesc->UIObjectDesc.fX = g_iWindowSizeX * 0.5f;
	pDesc->UIObjectDesc.fY = g_iWindowSizeY * 0.5f;
	pDesc->UIObjectDesc.fZ = 0.01f;

	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	m_eRenderGroup = CRenderer::RENDERGROUP::UI_ANIM;

	if (FAILED(CUIObject::Initialize(&pDesc->UIObjectDesc)))
		return E_FAIL;

	m_pPlayerStat = pDesc->pPlayerStat;
	m_ppShellStat = pDesc->ppShellStat;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	m_pUIDot[m_iCurMenuNum]->Change_SrcColor(m_vBlueColor);
	m_pUIDot[m_iCurMenuNum]->Change_Texture(L"Prototype_Component_Texture_MenuStar", 0);

	m_isActive = false;

	m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	return S_OK;
}

void CUI_Menu::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_Menu::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (m_pUIManager->Get_CopyTexture_IsActive())
		m_eRenderGroup = CRenderer::RENDERGROUP::UI_ANIM;
	else
		m_eRenderGroup = CRenderer::RENDERGROUP::UI;

	switch (m_iCurMenuNum)
	{
	case 0:
		Change_Color({ 136.f / 255.f, 166.f / 255.f, 215.f / 255.f, 1.f });
		m_pUIStatus->Set_IsActive(true);
		m_pUIInventory->Set_IsActive(false);
		m_pUIOption->Set_IsActive(false);
		break;
	case 1:
		Change_Color({ 238.f / 255.f, 242.f / 255.f, 126.f / 255.f, 1.f });
		m_pUIStatus->Set_IsActive(false);
		m_pUIInventory->Set_IsActive(true);
		m_pUIOption->Set_IsActive(false);
		break;
	case 2:
		Change_Color({ 218.f / 255.f, 182.f / 255.f, 242.f / 255.f, 1.f });
		m_pUIStatus->Set_IsActive(false);
		m_pUIInventory->Set_IsActive(false);
		m_pUIOption->Set_IsActive(true);
		break;
	default:
		break;
	}

	super::Update(fTimeDelta);
}

void CUI_Menu::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);

		super::Late_Update(fTimeDelta);
	}
}

HRESULT CUI_Menu::Render()
{
	_int iPrevMenuNum = {};
	_int iNextMenuNum = {};

	if (m_iCurMenuNum == 0)
		iPrevMenuNum = 2;
	else
		iPrevMenuNum = m_iCurMenuNum - 1;

	if (m_iCurMenuNum == 2)
		iNextMenuNum = 0;
	else
		iNextMenuNum = m_iCurMenuNum + 1;

	m_pGameInstance->Render_Font(L"Font_Main_40", m_szMenuText[iPrevMenuNum], { 280.f, 100.f }, XMLoadFloat4(&m_vRedColor), 0.f, { 0.f, 0.f }, 0.6f, SORTING::LEFT);
	m_pGameInstance->Render_Font(L"Font_Main_40", m_szMenuText[m_iCurMenuNum], { 640.f, 100.f }, XMLoadFloat4(&m_vBlueColor), 0.f, {0.f, 0.f}, 1.5f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", m_szMenuText[iNextMenuNum], { 1000.f, 100.f }, XMLoadFloat4(&m_vRedColor), 0.f, { 0.f, 0.f }, 0.6f, SORTING::RIGHT);

	return S_OK;
}

void CUI_Menu::Left_Menu()
{
	m_pUIDot[m_iCurMenuNum]->Change_SrcColor(m_vRedColor);
	m_pUIDot[m_iCurMenuNum]->Change_Texture(L"Prototype_Component_Texture_MenuDot", 0);

	if (m_iCurMenuNum == 0)
		m_iCurMenuNum = 2;
	else
		m_iCurMenuNum--;

	m_pUIDot[m_iCurMenuNum]->Change_SrcColor(m_vBlueColor);
	m_pUIDot[m_iCurMenuNum]->Change_Texture(L"Prototype_Component_Texture_MenuStar", 0);
}

void CUI_Menu::Change_Color(_float4 vColor)
{
	m_pUITopBorder->Change_SrcColor(vColor);
	m_pUIBottomBorder->Change_SrcColor(vColor);
}

void CUI_Menu::Right_Menu()
{
	m_pUIDot[m_iCurMenuNum]->Change_SrcColor(m_vRedColor);
	m_pUIDot[m_iCurMenuNum]->Change_Texture(L"Prototype_Component_Texture_MenuDot", 0);

	if (m_iCurMenuNum == 2)
		m_iCurMenuNum = 0;
	else
		m_iCurMenuNum++;

	m_pUIDot[m_iCurMenuNum]->Change_SrcColor(m_vBlueColor);
	m_pUIDot[m_iCurMenuNum]->Change_Texture(L"Prototype_Component_Texture_MenuStar", 0);
}

HRESULT CUI_Menu::Ready_PartObject()
{
	CUI_DefaultStretch::DESC StretchDesc{};
	{
		StretchDesc.UIDesc.pOwner = this;

		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/UI_Menu_Board1.dat";
		StretchDesc.fStretchFactor = 75.f;
		StretchDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Board1", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	m_pUITopBorder = static_cast<CUI_DefaultStretch*>(Find_PartObject({ L"Part_UI_Board1" }));

	{
		StretchDesc.UIDesc.pOwner = this;

		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/UI_Menu_Board2.dat";
		StretchDesc.fStretchFactor = 75.f;
		StretchDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Board2", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	m_pUIBottomBorder = static_cast<CUI_DefaultStretch*>(Find_PartObject({ L"Part_UI_Board2" }));

	/* Left Button */
	CUI_Button::DESC ButtonDesc{};
	{
		ButtonDesc.UIDesc.pOwner = this;
		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/UI_Menu_Button1.dat";

		ButtonDesc.ClickCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
				Left_Menu();
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_Left_Button", LEVEL_STATIC, L"Prototype_GameObject_UI_Button", &ButtonDesc)))
		return E_FAIL;

	{
		ButtonDesc.UIDesc.pOwner = this;
		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/UI_Menu_Button2.dat";

		ButtonDesc.ClickCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
				Right_Menu();
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_Right_Button", LEVEL_STATIC, L"Prototype_GameObject_UI_Button", &ButtonDesc)))
		return E_FAIL;

	CUI_Shake::DESC ShakeDesc{};
	{
		ShakeDesc.UIDesc.pOwner = this;
		ShakeDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/UI_Menu_Title.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Title", LEVEL_STATIC, L"Prototype_GameObject_UI_Shake", &ShakeDesc)))
		return E_FAIL;

	{
		ShakeDesc.UIDesc.pOwner = this;
		ShakeDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/UI_Menu_Deco1.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Deco1", LEVEL_STATIC, L"Prototype_GameObject_UI_Shake", &ShakeDesc)))
		return E_FAIL;

	{
		ShakeDesc.UIDesc.pOwner = this;
		ShakeDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/UI_Menu_Deco2.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Deco2", LEVEL_STATIC, L"Prototype_GameObject_UI_Shake", &ShakeDesc)))
		return E_FAIL;

	_float2 vStart = { 600.f, 640.f };
	_float	fOffsetX = { 40.f };

	for (_int i = 0; i < 3; i++)
	{
		_wstring strPartTag = L"Part_UI_Dot" + to_wstring(i);

		_float2 vLocate = { vStart.x + fOffsetX * i, vStart.y};

		/* Dot */
		CUI_DynamicTexture::DESC Desc{};
		{
			Desc.UIDesc.pOwner = this;
			Desc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/UI_Menu_Dot.dat";
			Desc.vLocate = vLocate;
		}
		if (FAILED(Add_PartObject(strPartTag, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &Desc)))
			return E_FAIL;

		m_pUIDot[i] = static_cast<CUI_DynamicTexture*>(Find_PartObject({ strPartTag }));
	}

	CUI_Status::DESC StatusDesc{};
	{
		StatusDesc.pOwner = this;

		StatusDesc.pPlayerStat = m_pPlayerStat;
		StatusDesc.ppShellStat = m_ppShellStat;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Status", LEVEL_STATIC, L"Prototype_GameObject_UI_Status", &StatusDesc)))
		return E_FAIL;

	m_pUIStatus = static_cast<CUI_Status*>(Find_PartObject({ L"Part_UI_Status" }));

	CUI_Inventory::DESC InvenDesc{};
	{
		InvenDesc.pOwner = this;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Inventory", LEVEL_STATIC, L"Prototype_GameObject_UI_Inventory", &InvenDesc)))
		return E_FAIL;

	m_pUIInventory = static_cast<CUI_Inventory*>(Find_PartObject({ L"Part_UI_Inventory" }));

	CUI_Option::DESC OptionDesc{};
	{
		OptionDesc.pOwner = this;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Option", LEVEL_STATIC, L"Prototype_GameObject_UI_Option", &OptionDesc)))
		return E_FAIL;

	m_pUIOption = static_cast<CUI_Option*>(Find_PartObject({ L"Part_UI_Option" }));

	return S_OK;
}

CUI_Menu* CUI_Menu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Menu* pInstance = new CUI_Menu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Menu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Menu::Clone(void* pArg)
{
	CUI_Menu* pInstance = new CUI_Menu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Menu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Menu::Free()
{
	CUIObject::Free();
}