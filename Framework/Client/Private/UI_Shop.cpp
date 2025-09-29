#include "pch.h"
#include "UI_Shop.h"

#include "GameInstance.h"
#include "FileData_Locator.h"
#include "FData.h"
#include "UI_Manager.h"
#include "UI_DefaultStretch.h"
#include "UI_Texture.h"
#include "UI_Shake.h"
#include "UI_DynamicTexture.h"
#include "UI_ShopButton.h"
#include "UI_Button.h"
#include "UI_Description.h"

CUI_Shop::CUI_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Shop::CUI_Shop(const CUI_Shop& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_Shop::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Shop::Initialize(void* pArg)
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

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CUI_Shop::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	super::Priority_Update(fTimeDelta);
}

void CUI_Shop::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (m_pGameInstance->Key_Down(KEY::ESCAPE))
	{
		CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
		pUIManager->Hide_ShopUI([this, pUIManager]()->void
			{
				pUIManager->Show_ShopSelectUI();
				//m_pGameInstance->Play_Sound(L"상점나가기.wav", (_uint)SOUND_CHANNEL::UI, 0.9f);
			});
	}

	if (m_bIsSellShop)
	{
		m_pUIDeco[0]->Change_SrcColor(m_vSellColor);
		m_pUIDeco[1]->Change_SrcColor(m_vSellColor);
	}
	else
	{
		m_pUIDeco[0]->Change_SrcColor(m_vBuyColor);
		m_pUIDeco[1]->Change_SrcColor(m_vBuyColor);
	}

	_wstring strTextureTag = {};

	for (_int i = 0; i < 8; i++)
	{
		if (i < m_pFromInventory->Get_Size())
		{
			m_pUIButton[i]->Set_IsActive(true);

			_int iTextureNum = m_pFromInventory->Get_ItemData(i).first;
			_int iItemCount = m_pFromInventory->Get_ItemData(i).second;
			
			switch (m_pFromInventory->Get_InvenType())
			{
			case CInventory::INVEN_TRASH:
			{
				strTextureTag = L"Prototype_Component_Texture_Item";

				FItemData tData = CFileData_Locator<FItemData>::Get_FileData()->Get_Data(0, iTextureNum);
				m_pUIButton[i]->Change_ItemTexture(strTextureTag, iTextureNum);

				m_pUIButton[i]->Set_ItemData(iItemCount, tData.szName, tData.iPrice);
				m_pUIButton[i]->Set_HoverCallback([this, strTextureTag, iTextureNum, iItemCount, tData, i]()->void
					{
						m_pUIDesc->Show(true);
						m_pUIDesc->Change_Texture(strTextureTag, iTextureNum);
						m_pUIDesc->Set_Name(tData.szName);
						m_pUIDesc->Set_Description(tData.szDescription);
					});
				m_pUIButton[i]->Set_Callback([this, iTextureNum, iItemCount, tData, i]()->void
					{
						CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

						if (m_bIsSellShop)
						{
							m_pGameInstance->Play_Sound(L"Buy.wav", (_uint)SOUND_CHANNEL::UI, 0.9f);
							pUIManager->Set_MicroPlastic(iItemCount * tData.iPrice);
							m_pFromInventory->Delete(iTextureNum, iItemCount);
						}
						else
						{
							if (pUIManager->Get_Micro_Ref() >= tData.iPrice)
							{
								m_pGameInstance->Play_Sound(L"Buy.wav", (_uint)SOUND_CHANNEL::UI, 0.9f);
								pUIManager->Set_MicroPlastic(-1 * tData.iPrice);
								m_pFromInventory->Delete(iTextureNum, 1);
								m_pToInventory->Add(iTextureNum, 1);
							}
							else
								m_pGameInstance->Play_Sound(L"BuyCancel.wav", (_uint)SOUND_CHANNEL::UI, 0.9f);
						}

					});
			}
				break;
			case CInventory::INVEN_SHOP: 
			{
				strTextureTag = L"Prototype_Component_Texture_ShopItem";

				FItemData tData = CFileData_Locator<FItemData>::Get_FileData()->Get_Data(3, iTextureNum);
				m_pUIButton[i]->Change_ItemTexture(strTextureTag, iTextureNum);

				m_pUIButton[i]->Set_ItemData(iItemCount, tData.szName, tData.iPrice);
				m_pUIButton[i]->Set_HoverCallback([this, strTextureTag, iTextureNum, iItemCount, tData, i]()->void
					{
						m_pUIDesc->Show(true);
						m_pUIDesc->Change_Texture(strTextureTag, iTextureNum);
						m_pUIDesc->Set_Name(tData.szName);
						m_pUIDesc->Set_Description(tData.szDescription);
					});
				m_pUIButton[i]->Set_Callback([this, iTextureNum, iItemCount, tData, i]()->void
					{
						CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

						if (m_bIsSellShop)
						{
							m_pGameInstance->Play_Sound(L"Buy.wav", (_uint)SOUND_CHANNEL::UI, 0.9f);
							pUIManager->Set_MicroPlastic(iItemCount * tData.iPrice);
							m_pFromInventory->Delete(iTextureNum, iItemCount);
						}
						else
						{
							if (pUIManager->Get_Micro_Ref() >= tData.iPrice)
							{
								m_pGameInstance->Play_Sound(L"Buy.wav", (_uint)SOUND_CHANNEL::UI, 0.9f);
								pUIManager->Set_MicroPlastic(-1 * tData.iPrice);
								m_pFromInventory->Delete(iTextureNum, 1);
								m_pToInventory->Add(iTextureNum, 1);
							}
							else
								m_pGameInstance->Play_Sound(L"BuyCancel.wav", (_uint)SOUND_CHANNEL::UI, 0.9f);
						}
					});
			}
				break;
			default:
				break;
			}
		}
		else
		{
			m_pUIButton[i]->Set_IsActive(false);
		}
	}
	super::Update(fTimeDelta);
}

void CUI_Shop::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);

		super::Late_Update(fTimeDelta);
	}
}

HRESULT CUI_Shop::Render()
{
    return S_OK;
}

HRESULT CUI_Shop::Ready_PartObject()
{
	CUI_DefaultStretch::DESC DefaultDesc{};
	{
		DefaultDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shop/UI_Shop_Board_Top.dat";
		DefaultDesc.fStretchFactor = 100.f;
		DefaultDesc.UIDesc.bStretchY = false;
		DefaultDesc.UIDesc.bRightAngle = false;
		DefaultDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Board_Top", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &DefaultDesc)))
		return E_FAIL;

	{
		DefaultDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shop/UI_Shop_Board_Bottom.dat";
		DefaultDesc.fStretchFactor = 100.f;
		DefaultDesc.UIDesc.bStretchY = false;
		DefaultDesc.UIDesc.bRightAngle = false;
		DefaultDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Board_Bottom", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &DefaultDesc)))
		return E_FAIL;

	{
		DefaultDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shop/UI_Shop_Board_Left.dat";
		DefaultDesc.fStretchFactor = 60.f;
		DefaultDesc.UIDesc.bStretchY = true;
		DefaultDesc.UIDesc.bRightAngle = false;
		DefaultDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Board_Left", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &DefaultDesc)))
		return E_FAIL;

	{
		DefaultDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shop/UI_Shop_Board_Right.dat";
		DefaultDesc.fStretchFactor = 60.f;
		DefaultDesc.UIDesc.bStretchY = true;
		DefaultDesc.UIDesc.bRightAngle = false;
		DefaultDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Board_Right", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &DefaultDesc)))
		return E_FAIL;

	CUI_Texture::DESC TextureDesc{};
	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shop/UI_Shop_Board_Center.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Board_Center", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	{
		DefaultDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shop/UI_Shop_Border.dat";
		DefaultDesc.fStretchFactor = 3.f;
		DefaultDesc.UIDesc.bStretchY = false;
		DefaultDesc.UIDesc.bRightAngle = true;
		DefaultDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Border", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &DefaultDesc)))
		return E_FAIL;

	m_pUIBorder = static_cast<CUI_DefaultStretch*>(Find_PartObject({ L"Part_UI_Border" }));
	m_pUIBorder->Set_Text(L"상점");

	CUI_Shake::DESC ShakeDesc{};
	{
		ShakeDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shop/UI_Shop_Deco1.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Deco1", LEVEL_STATIC, L"Prototype_GameObject_UI_Shake", &ShakeDesc)))
		return E_FAIL;

	m_pUIDeco[0] = static_cast<CUI_Shake*>(Find_PartObject({L"Part_UI_Deco1"}));

	{
		ShakeDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shop/UI_Shop_Deco2.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Deco2", LEVEL_STATIC, L"Prototype_GameObject_UI_Shake", &ShakeDesc)))
		return E_FAIL;

	m_pUIDeco[1] = static_cast<CUI_Shake*>(Find_PartObject({ L"Part_UI_Deco2" }));

	_float2 vStart = { 400.f, 260.f };

	_float fOffsetY = 40.f;

	for (_int i = 0; i < 7; i++)
	{
		wstring strPartTag = L"Part_UI_Line" + to_wstring(i);
		_float2 vLocate = { vStart.x, vStart.y + fOffsetY * i };

		CUI_DynamicTexture::DESC DynamicDesc{};
		{
			DynamicDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shop/UI_Shop_Line.dat";
			DynamicDesc.vLocate = vLocate;
		}
		if (FAILED(Add_PartObject(strPartTag, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &DynamicDesc)))
			return E_FAIL;
	}

	vStart = { 400.f, 240.f };

	for (_int i = 0; i < 8; i++)
	{
		wstring strPartTag = L"Part_UI_Button" + to_wstring(i);
		_float2 vLocate = { vStart.x, vStart.y + fOffsetY * i };

		CUI_ShopButton::DESC ButtonDesc{};
		{
			ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shop/UI_Shop_Button.dat";
			ButtonDesc.vLocate = vLocate;
			ButtonDesc.fStretchFactor = 420.f;
		}
		if (FAILED(Add_PartObject(strPartTag, LEVEL_STATIC, L"Prototype_GameObject_UI_ShopButton", &ButtonDesc)))
			return E_FAIL;

		m_pUIButton[i] = static_cast<CUI_ShopButton*>(Find_PartObject({ strPartTag }));
	}

	/* ESC_Button */
	CUI_Button::DESC ButtonDesc{};
	{
		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shop/UI_ESC_Button.dat";
		lstrcpy(ButtonDesc.szText, TEXT("뒤로"));
		ButtonDesc.ClickCallback = [this]()->void
			{
				CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
				pUIManager->Hide_ShopUI([this, pUIManager]()->void
					{
						pUIManager->Show_ShopSelectUI();
					});
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_ESC_Button", LEVEL_STATIC, L"Prototype_GameObject_UI_Button", &ButtonDesc)))
		return E_FAIL;

	CUI_Description::DESC DescDesc{};
	{
		DescDesc.pOwner = this;

		DescDesc.szBoardFileName =	L"../Bin/DataFiles/UI_Data/Shop/Description/UI_DescBoard.dat";
		DescDesc.szBorderFileName = L"../Bin/DataFiles/UI_Data/Shop/Description/UI_DescBorder.dat";
		DescDesc.szLogoFileName =	L"../Bin/DataFiles/UI_Data/Shop/Description/UI_DescLogo.dat";
		DescDesc.szIconFileName =	L"../Bin/DataFiles/UI_Data/Shop/Description/UI_DescIcon.dat";
		DescDesc.szLineFileName =	L"../Bin/DataFiles/UI_Data/Shop/Description/UI_DescLine.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Description", LEVEL_STATIC, L"Prototype_GameObject_UI_Description", &DescDesc)))
		return E_FAIL;

	m_pUIDesc = static_cast<CUI_Description*>(Find_PartObject({ L"Part_UI_Description" }));


	return S_OK;
}

CUI_Shop* CUI_Shop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Shop* pInstance = new CUI_Shop(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Shop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Shop::Clone(void* pArg)
{
	CUI_Shop* pInstance = new CUI_Shop(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Shop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Shop::Free()
{
    CUIObject::Free();
}

