#include "pch.h"
#include "UI_Inventory.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "FileData_Locator.h"
#include "FData.h"

#include "UI_DefaultStretch.h"
#include "UI_Description.h"
#include "UI_DynamicTexture.h"
#include "UI_DynamicButton.h"

CUI_Inventory::CUI_Inventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Inventory::CUI_Inventory(const CUI_Inventory& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_Inventory::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Inventory::Initialize(void* pArg)
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

	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	m_pOwner = pDesc->pOwner;
	m_pItemInventory = pUIManager->Get_ItemInventory();
	m_pShopItemInventory = pUIManager->Get_ShopItemInventory();
	m_pQuestItemInventory = pUIManager->Get_QuestItemInventory();

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	m_isActive = false;

	return S_OK;
}

void CUI_Inventory::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (m_pUIManager->Get_CopyTexture_IsActive())
		m_eRenderGroup = CRenderer::RENDERGROUP::UI_ANIM;
	else
		m_eRenderGroup = CRenderer::RENDERGROUP::UI;

	super::Priority_Update(fTimeDelta);
}

void CUI_Inventory::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	_int iInventoryCount = 0;

	for (_int i = 0; i < m_pItemInventory->Get_Size(); i++)
	{
		pair<_uint, _int> prItemData = m_pItemInventory->Get_ItemData(i);
		wstring result = to_wstring(prItemData.second);

		if (prItemData.second != -1)
		{
			m_pUIInventory[iInventoryCount]->Set_IsActive(true);
			m_pUIInventory[iInventoryCount]->Set_Text(result.c_str());
			m_pUIInventory[iInventoryCount]->Set_HoverCallback([this, i, prItemData]()->void
				{
					FItemData tData = CFileData_Locator<FItemData>::Get_FileData()->Get_Data(0, prItemData.first);

					m_pUIDesc->Change_Texture(m_strItemTextureTag, prItemData.first);
					m_pUIDesc->Set_Name(tData.szName);
					m_pUIDesc->Set_Description(tData.szDescription);

					m_pUIDesc->Show(true);
				});
			m_pUIItem[iInventoryCount]->Change_Texture(m_strItemTextureTag, prItemData.first);
			iInventoryCount++;
		}
		else
			break;
	}
	for (_int i = 0; i < m_pQuestItemInventory->Get_Size(); i++)
	{
		pair<_uint, _int> prItemData = m_pQuestItemInventory->Get_ItemData(i);
		wstring result = to_wstring(prItemData.second);

		if (prItemData.second != -1)
		{
			m_pUIInventory[iInventoryCount]->Set_IsActive(true);
			m_pUIInventory[iInventoryCount]->Set_Text(result.c_str());
			m_pUIInventory[iInventoryCount]->Set_HoverCallback([this, i, prItemData]()->void
				{
					FItemData tData = CFileData_Locator<FItemData>::Get_FileData()->Get_Data(1, prItemData.first);

					m_pUIDesc->Change_Texture(m_strQuestItemTextureTag, prItemData.first);
					m_pUIDesc->Set_Name(tData.szName);
					m_pUIDesc->Set_Description(tData.szDescription);

					m_pUIDesc->Show(true);
				});
			m_pUIItem[iInventoryCount]->Change_Texture(m_strQuestItemTextureTag, prItemData.first);
			iInventoryCount++;
		}
		else
			break;
	}
	for (_int i = 0; i < m_pShopItemInventory->Get_Size(); i++)
	{
		pair<_uint, _int> prItemData = m_pShopItemInventory->Get_ItemData(i);
		wstring result = to_wstring(prItemData.second);

		if (prItemData.second != -1)
		{
			m_pUIInventory[iInventoryCount]->Set_IsActive(true);
			m_pUIInventory[iInventoryCount]->Set_Text(result.c_str());
			m_pUIInventory[iInventoryCount]->Set_HoverCallback([this, i, prItemData]()->void
				{
					FItemData tData = CFileData_Locator<FItemData>::Get_FileData()->Get_Data(2, prItemData.first);

					m_pUIDesc->Change_Texture(m_strShopItemTextureTag, prItemData.first);
					m_pUIDesc->Set_Name(tData.szName);
					m_pUIDesc->Set_Description(tData.szDescription);

					m_pUIDesc->Show(true);
				});
			m_pUIItem[iInventoryCount]->Change_Texture(m_strShopItemTextureTag, prItemData.first);
			iInventoryCount++;
		}
		else
			break;
	}

	for (_int i = 0; i < 15; i++)
	{
		if (i < iInventoryCount)
		{
			m_pUIInventory[i]->Set_IsActive(true);
			m_pUIItem[i]->Set_IsActive(true);
		}
		else
		{
			m_pUIInventory[i]->Set_IsActive(false);
			m_pUIItem[i]->Set_IsActive(false);
		}
	}

	super::Update(fTimeDelta);
}

void CUI_Inventory::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);

	super::Late_Update(fTimeDelta);
}

HRESULT CUI_Inventory::Render()
{
	m_pGameInstance->Render_Font(L"Font_Main_40", L"인벤토리", {400.f, 165.f}, XMLoadFloat4(&m_vColor), 0.f, {0.f, 0.f}, 0.8f);
    return S_OK;
}

HRESULT CUI_Inventory::Ready_PartObject()
{
	CUI_DefaultStretch::DESC StretchDesc{};
	{
		StretchDesc.UIDesc.pOwner = m_pOwner;
		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Inventory/UI_Inven_BigBorder.dat";
		StretchDesc.fStretchFactor = 10.f;
		StretchDesc.bMiddle = true;
		StretchDesc.UIDesc.bRightAngle = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_BigBorder", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	{
		StretchDesc.UIDesc.pOwner = m_pOwner;
		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Inventory/UI_Inven_SmallBorder.dat";
		StretchDesc.fStretchFactor = 10.f;
		StretchDesc.bMiddle = true;
		StretchDesc.UIDesc.bRightAngle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_SmallBorder", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	CUI_Description::DESC DescDesc{};
	{
		DescDesc.pOwner = m_pOwner;

		DescDesc.szBoardFileName =	L"../Bin/DataFiles/UI_Data/Menu/Inventory/Description/UI_DescBoard.dat";
		DescDesc.szBorderFileName = L"../Bin/DataFiles/UI_Data/Menu/Inventory/Description/UI_DescBorder.dat";
		DescDesc.szLogoFileName =	L"../Bin/DataFiles/UI_Data/Menu/Inventory/Description/UI_DescLogo.dat";
		DescDesc.szIconFileName =	L"../Bin/DataFiles/UI_Data/Menu/Inventory/Description/UI_DescIcon.dat";
		DescDesc.szLineFileName =	L"../Bin/DataFiles/UI_Data/Menu/Inventory/Description/UI_DescLine.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Description", LEVEL_STATIC, L"Prototype_GameObject_UI_Description", &DescDesc)))
		return E_FAIL;
	
	m_pUIDesc = static_cast<CUI_Description*>(Find_PartObject({ L"Part_UI_Description" }));

	_float2 vSize = { 80.f, 80.f };
	_float2 vStart = { 310.f, 270.f };
	// 시작점
	// offset

	_float2 vOffset = { 5.f, 5.f };
	/* Button */
	CUI_DynamicTexture::DESC TextureDesc{};
	CUI_DynamicTexture::DESC ItemDesc{};

	TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Inventory/UI_InventoryBorder.dat";
	TextureDesc.UIDesc.pOwner = m_pOwner;

	CUI_DynamicButton::DESC InventoryDesc{};
	InventoryDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Inventory/UI_InventoryButton.dat";
	InventoryDesc.UIDesc.pOwner = m_pOwner;

	ItemDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Inventory/UI_InventoryItem.dat";
	ItemDesc.UIDesc.pOwner = m_pOwner;

	for (_int i = 0; i < 15; i++)
	{
		wstring strBorderPartTag = L"Part_UI_Border" + to_wstring(i);
		wstring strButtonPartTag = L"Part_UI_ItemInventory" + to_wstring(i);
		wstring strItemPartTag = L"Part_UI_Item" + to_wstring(i);

		_float2 vLocate = { vStart.x + (vSize.x + vOffset.x) * (i % 5), vStart.y + (vSize.y + vOffset.y) * (i / 5) };
		TextureDesc.vLocate = vLocate;
		InventoryDesc.vLocate = vLocate;
		ItemDesc.vLocate = vLocate;

		if (FAILED(Add_PartObject(strBorderPartTag, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &TextureDesc)))
			return E_FAIL;

		if (FAILED(Add_PartObject(strButtonPartTag, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &InventoryDesc)))
			return E_FAIL;

		if (FAILED(Add_PartObject(strItemPartTag, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &ItemDesc)))
			return E_FAIL;

		m_pUIInventory[i] = static_cast<CUI_DynamicButton*>(Find_PartObject({ strButtonPartTag }));
		m_pUIItem[i] = static_cast<CUI_DynamicTexture*>(Find_PartObject({ strItemPartTag }));
	}
	
	return S_OK;
}

CUI_Inventory* CUI_Inventory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Inventory* pInstance = new CUI_Inventory(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Inventory");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Inventory::Clone(void* pArg)
{
	CUI_Inventory* pInstance = new CUI_Inventory(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Inventory");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Inventory::Free()
{
    CUIObject::Free();
}

