#include "pch.h"
#include "UI_MoonSnail.h"

#include "GameInstance.h"
#include "UI_Manager.h"

#include "UI_Texture.h"
#include "UI_Shake.h"
#include "UI_DynamicButton.h"

#include "Player.h"

CUI_MoonSnail::CUI_MoonSnail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_MoonSnail::CUI_MoonSnail(const CUI_MoonSnail& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_MoonSnail::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_MoonSnail::Initialize(void* pArg)
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

	m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CUI_MoonSnail::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	super::Priority_Update(fTimeDelta);
}

void CUI_MoonSnail::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (m_pUIManager->Get_CopyTexture_IsActive())
		m_eRenderGroup = CRenderer::RENDERGROUP::UI_ANIM;
	else
		m_eRenderGroup = CRenderer::RENDERGROUP::UI;

	if (m_pGameInstance->Key_Down(KEY::ESCAPE))
	{
		m_pUIManager->Hide_MoonSnailUI();

		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player", 0));
		pPlayer->MoonShell_Interact(false);
	}

	super::Update(fTimeDelta);
}

void CUI_MoonSnail::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);

	super::Late_Update(fTimeDelta);
}

HRESULT CUI_MoonSnail::Render()
{
	m_pGameInstance->Render_Font(L"Font_Cafe_20", m_szLocation, { 350.f, 270.f }, XMLoadFloat4(&m_vLocationColor), 0.f, { 0.f, 0.f }, 1.0f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", m_szName, { 200.f, 285.f }, XMLoadFloat4(&m_vNameColor), 0.f, { 0.f, 0.f }, 0.8f);

    return S_OK;
}

void CUI_MoonSnail::Set_Data(_tchar* szLocation, _tchar* szName)
{
	lstrcpy(m_szLocation, szLocation);
	lstrcpy(m_szName, szName);
}

HRESULT CUI_MoonSnail::Ready_PartObject()
{
	/* Fill */
	CUI_Texture::DESC Desc{};
	{
		Desc.UIDesc.pOwner = this;
		Desc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/UI_MoonSnail_Fill.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Fill", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &Desc)))
		return E_FAIL;

	/* Border */
	{
		Desc.UIDesc.pOwner = this;
		Desc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/UI_MoonSnail_Border.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Border", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &Desc)))
		return E_FAIL;
	
	{
		Desc.UIDesc.pOwner = this;
		Desc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/UI_MoonSnail_Deco_MoonBorder.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Deco1", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &Desc)))
		return E_FAIL;

	{
		Desc.UIDesc.pOwner = this;
		Desc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/UI_MoonSnail_Deco_MoonFill.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Deco2", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &Desc)))
		return E_FAIL;

	CUI_Shake::DESC ShakeDesc{};
	{
		ShakeDesc.UIDesc.pOwner = this;
		ShakeDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/UI_MoonSnail_Deco_Grass.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Deco3", LEVEL_STATIC, L"Prototype_GameObject_UI_Shake", &ShakeDesc)))
		return E_FAIL;

	{
		ShakeDesc.UIDesc.pOwner = this;
		ShakeDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/UI_MoonSnail_Deco_Star1.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Deco4", LEVEL_STATIC, L"Prototype_GameObject_UI_Shake", &ShakeDesc)))
		return E_FAIL;

	{
		ShakeDesc.UIDesc.pOwner = this;
		ShakeDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/UI_MoonSnail_Deco_Star2.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Deco5", LEVEL_STATIC, L"Prototype_GameObject_UI_Shake", &ShakeDesc)))
		return E_FAIL;

	{
		ShakeDesc.UIDesc.pOwner = this;
		ShakeDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/UI_MoonSnail_Deco_Star3.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Deco6", LEVEL_STATIC, L"Prototype_GameObject_UI_Shake", &ShakeDesc)))
		return E_FAIL;

	CUI_DynamicButton::DESC DynamicButtonDesc{};
	DynamicButtonDesc.UIDesc.pOwner = this;
	DynamicButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/UI_MoonSnail_Button.dat";

	DynamicButtonDesc.HoverCallback = [this]()->void
		{
			m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
		};
	_float2 vStart = { 367.f, 366.f };
	_float fOffsetY = 50.f;

	{
		DynamicButtonDesc.vLocate = vStart;
		lstrcpy(DynamicButtonDesc.szText, TEXT("골레포트"));

		DynamicButtonDesc.Callback = [this]()->void
			{
				m_pUIManager->Hide_MoonSnailUI(false);
				m_pUIManager->Show_SheleportUI();
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_Sheleport_Button", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &DynamicButtonDesc)))
		return E_FAIL;

	{
		DynamicButtonDesc.vLocate = { vStart.x, vStart.y + fOffsetY };

		lstrcpy(DynamicButtonDesc.szText, TEXT("레벨 업"));

		DynamicButtonDesc.Callback = [this]()->void
			{
				m_pUIManager->Hide_MoonSnailUI(false);
				m_pUIManager->Show_StatUI();
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_LevelUp_Button", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &DynamicButtonDesc)))
		return E_FAIL;

	{
		DynamicButtonDesc.vLocate = { vStart.x, vStart.y + fOffsetY * 2 };

		lstrcpy(DynamicButtonDesc.szText, TEXT("나가기"));

		DynamicButtonDesc.Callback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
				m_pUIManager->Hide_MoonSnailUI();

				CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player", 0));
				pPlayer->MoonShell_Interact(false);
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_Quit_Button", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &DynamicButtonDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_MoonSnail* CUI_MoonSnail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MoonSnail* pInstance = new CUI_MoonSnail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_MoonSnail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_MoonSnail::Clone(void* pArg)
{
	CUI_MoonSnail* pInstance = new CUI_MoonSnail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_MoonSnail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MoonSnail::Free()
{
    CUIObject::Free();
}

