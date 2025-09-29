#include "pch.h"
#include "UI_ShopSelect.h"

#include "GameInstance.h"
#include "Player.h"
#include "Weapon_Player.h"
#include "UI_Manager.h"

#include "UI_Texture.h"
#include "UI_DynamicButton.h"
#include "UI_DefaultStretch.h"

CUI_ShopSelect::CUI_ShopSelect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_ShopSelect::CUI_ShopSelect(const CUI_ShopSelect& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_ShopSelect::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_ShopSelect::Initialize(void* pArg)
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

void CUI_ShopSelect::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	super::Priority_Update(fTimeDelta);
}

void CUI_ShopSelect::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	lstrcpy(m_szText, L"새우전 당포");

	m_pUITrashButton1->Set_Text(L"쓰레기 구매");
	m_pUITrashButton1->Set_Callback([this]()->void
		{
			CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
			pUIManager->Hide_ShopSelectUI([this, pUIManager]()->void
				{
					pUIManager->Show_BuyShopUI();
				});
		});

	m_pUITrashButton2->Set_Text(L"쓰레기 판매");
	m_pUITrashButton2->Set_Callback([this]()->void
		{
			CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
			pUIManager->Hide_ShopSelectUI([this, pUIManager]()->void
			{
					pUIManager->Show_SellShopUI();
			});
		});

	m_pUIExitButton->Set_Text(L"나가기");
	m_pUIExitButton->Set_Callback([this]()->void
		{
			CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
			pUIManager->Hide_ShopSelectUI();
		});

	super::Update(fTimeDelta);
}

void CUI_ShopSelect::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);

		super::Late_Update(fTimeDelta);
	}
}

HRESULT CUI_ShopSelect::Render()
{
	m_pGameInstance->Render_Font(L"Font_Main_40", m_szText, { 330.f, 220.f }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 0.5f, SORTING::MIDDLE);
	return S_OK;
}

HRESULT CUI_ShopSelect::Ready_PartObject()
{
	CUI_DefaultStretch::DESC DefaultDesc{};
	{
		DefaultDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/ShopSelect/UI_Line.dat";
		DefaultDesc.fStretchFactor = 150.f;
		DefaultDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Line", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &DefaultDesc)))
		return E_FAIL;

	CUI_DynamicButton::DESC ButtonDesc{};
	ButtonDesc.HoverCallback = [this]()->void
		{
			m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
		};

	{
		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/ShopSelect/UI_Button_Trash.dat";
		ButtonDesc.UIDesc.eSorting = SORTING::MIDDLE;
		ButtonDesc.vLocate = { 287.f, 295.f };
	}
	if (FAILED(Add_PartObject(L"Part_UI_Trash1", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &ButtonDesc)))
		return E_FAIL;

	m_pUITrashButton1 = static_cast<CUI_DynamicButton*>(Find_PartObject({ L"Part_UI_Trash1" }));

	{
		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/ShopSelect/UI_Button_Trash.dat";
		ButtonDesc.UIDesc.eSorting = SORTING::MIDDLE;
		ButtonDesc.vLocate = { 287.f, 360.f };
	}
	if (FAILED(Add_PartObject(L"Part_UI_Trash2", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &ButtonDesc)))
		return E_FAIL;

	m_pUITrashButton2 = static_cast<CUI_DynamicButton*>(Find_PartObject({ L"Part_UI_Trash2" }));

	{
		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/ShopSelect/UI_Button_Exit.dat";
		ButtonDesc.UIDesc.eSorting = SORTING::MIDDLE;
		ButtonDesc.vLocate = { 260.f, 425.f };
	}
	if (FAILED(Add_PartObject(L"Part_UI_Exit", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &ButtonDesc)))
		return E_FAIL;

	m_pUIExitButton = static_cast<CUI_DynamicButton*>(Find_PartObject({ L"Part_UI_Exit" }));

	return S_OK;
}

CUI_ShopSelect* CUI_ShopSelect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ShopSelect* pInstance = new CUI_ShopSelect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_ShopSelect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_ShopSelect::Clone(void* pArg)
{
	CUI_ShopSelect* pInstance = new CUI_ShopSelect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_ShopSelect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_ShopSelect::Free()
{
    CUIObject::Free();
}

