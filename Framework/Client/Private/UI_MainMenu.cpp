#include "pch.h"
#include "UI_MainMenu.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "UI_Manager.h"
#include "CallbackUpdate.h"

#include "UI_Logo.h"
#include "UI_Button.h"
#include "UI_Credit.h"

CUI_MainMenu::CUI_MainMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_MainMenu::CUI_MainMenu(const CUI_MainMenu& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_MainMenu::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_MainMenu::Initialize(void* pArg)
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

	return S_OK;
}

void CUI_MainMenu::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_MainMenu::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CUI_MainMenu::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);

	super::Late_Update(fTimeDelta);
}

HRESULT CUI_MainMenu::Render()
{
    return S_OK;
}

HRESULT CUI_MainMenu::Ready_PartObject()
{
	/* Logo */
	CUI_Logo::DESC LogoDesc{};
	{
		LogoDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MainMenu/UI_Logo.dat";
		LogoDesc.UIDesc.eLevelID = LEVEL_LOGO;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Logo", LEVEL_LOGO, L"Prototype_GameObject_UI_Logo", &LogoDesc)))
		return E_FAIL;

	/* Continue Button */
	CUI_Button::DESC ButtonDesc{};
	{
		ButtonDesc.UIDesc.eSorting = SORTING::LEFT;
		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MainMenu/UI_Button1.dat";
		lstrcpy(ButtonDesc.szText, TEXT("게속하기"));

		ButtonDesc.HoverCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
			};
		ButtonDesc.ClickCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);

				CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

				pUIManager->FadeOut();

				CCallbackUpdate* pCallbackUpdate = static_cast<CCallbackUpdate*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CallbackUpdate"));

				pCallbackUpdate->Add_Update_Callback([pUIManager, this](_float fElapsed, _float fTimeDelta)->_bool
					{
						_bool bFadeEnd = pUIManager->End_FadeOut();
						if (bFadeEnd)
							m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BEACH));

						return bFadeEnd;
					});
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_Continue_Button", LEVEL_STATIC, L"Prototype_GameObject_UI_Button", &ButtonDesc)))
		return E_FAIL;

	/* Credits Button */
	{
		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MainMenu/UI_Button2.dat";
		lstrcpy(ButtonDesc.szText, TEXT("크레딧"));
		ButtonDesc.ClickCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
				
				m_pUICredit->Set_IsActive(true);
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_Credits_Button", LEVEL_STATIC, L"Prototype_GameObject_UI_Button", &ButtonDesc)))
		return E_FAIL;

	/* Quit Button */
	{
		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MainMenu/UI_Button3.dat";
		lstrcpy(ButtonDesc.szText, TEXT("종료"));
		ButtonDesc.ClickCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
				PostQuitMessage(0);
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_Quit_Button", LEVEL_STATIC, L"Prototype_GameObject_UI_Button", &ButtonDesc)))
		return E_FAIL;

	CUI_Credit::DESC CreditDesc{};
	{
		CreditDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Credit.dat";
		CreditDesc.UIDesc.isActive = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Credit", LEVEL_STATIC, L"Prototype_GameObject_UI_Credit", &CreditDesc)))
		return E_FAIL;

	m_pUICredit = static_cast<CUI_Credit*>(Find_PartObject({ L"Part_UI_Credit" }));

	return S_OK;
}

CUI_MainMenu* CUI_MainMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MainMenu* pInstance = new CUI_MainMenu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_MainMenu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_MainMenu::Clone(void* pArg)
{
	CUI_MainMenu* pInstance = new CUI_MainMenu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_MainMenu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MainMenu::Free()
{
    CUIObject::Free();
}

