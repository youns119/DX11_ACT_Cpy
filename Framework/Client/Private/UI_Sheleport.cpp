#include "pch.h"
#include "UI_Sheleport.h"

#include "GameInstance.h"
#include "Player.h"
#include "FileData_Locator.h"
#include "Level_Loading.h"
#include "ShellHouse.h"
#include "UI_Manager.h"
#include "CallbackUpdate.h"
#include "Theme_Manager.h"

#include "UI_Texture.h"
#include "UI_DefaultStretch.h"
#include "UI_Shake.h"
#include "UI_DynamicButton.h"

CUI_Sheleport::CUI_Sheleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Sheleport::CUI_Sheleport(const CUI_Sheleport& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_Sheleport::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Sheleport::Initialize(void* pArg)
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

	Calculate_Count();

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CUI_Sheleport::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (m_pUIManager->Get_CopyTexture_IsActive())
		m_eRenderGroup = CRenderer::RENDERGROUP::UI_ANIM;
	else
		m_eRenderGroup = CRenderer::RENDERGROUP::UI;

	super::Priority_Update(fTimeDelta);
}

void CUI_Sheleport::Update(_float fTimeDelta)
{
	if (m_pUIManager->Get_CopyTexture_IsActive())
		m_eRenderGroup = CRenderer::RENDERGROUP::UI_ANIM;
	else
		m_eRenderGroup = CRenderer::RENDERGROUP::UI;

	if (!m_isActive)
		return;

	if (m_pGameInstance->Key_Down(KEY::ESCAPE))
	{
		m_pUIManager->Hide_SheleportUI([this]()->void
			{
				m_pUIManager->Show_MoonSnailUI();
			});
	}

	_int iVisitedLocationNum = (_int)m_mapVisited.size();
	_wstring strLocation = L"";
	_wstring strName = L"";

	for (_int i = 0; i < m_vecLeftButton.size(); i++)
	{
		if (i < iVisitedLocationNum)
		{
			auto iter = m_mapVisited.begin();
			std::advance(iter, i);

			if (iter != m_mapVisited.end())
				strLocation = iter->first;

			m_vecLeftButton[i]->Set_IsActive(true);
			m_vecLeftButton[i]->Set_Text(strLocation.c_str());
			m_vecLeftButton[i]->Set_HoverCallback([this, iter, i]()->void
				{
					m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
					m_iCurHoverIndex = i;
					m_pUICover->Delete_CoverTexture();
				});
		}
		else
			m_vecLeftButton[i]->Set_IsActive(false);
	}

	_wstring strCurHoverLocation = m_vecLeftButton[m_iCurHoverIndex]->Get_Text();
	_int iVisitedNameNum = (_int)m_mapVisited[strCurHoverLocation].size();

	for (_int i = 0; i < m_vecRightButton.size(); i++)
	{
		if (i < iVisitedNameNum)
		{
			auto iter = m_mapVisited[strCurHoverLocation].begin();
			std::advance(iter, i);

			if (iter != m_mapVisited[strCurHoverLocation].end())
				strName = iter->second.SheleportData.szName;

			m_vecRightButton[i]->Set_IsActive(true);
			m_vecRightButton[i]->Set_Text(strName.c_str());
			m_vecRightButton[i]->Set_HoverCallback([this, iter]()->void
				{
					m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);

					m_pUICover->Set_CoverTexture(L"Prototype_Component_Texture_SheleportPrev", iter->second.SheleportData.iSheleportNum);
				});
			m_vecRightButton[i]->Set_Callback([this, iter]()->void
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back());

					pPlayer->Get_PlayerStat()->iShellLevel = iter->second.iLevelIndex;
					pPlayer->Get_PlayerStat()->iShellNum = iter->second.SheleportData.iSheleportNum;
					pPlayer->Get_PlayerStat()->vShellPos = iter->second.vPosition;
					pPlayer->Get_PlayerStat()->vShellLook = iter->second.vLook;
					pPlayer->Get_PlayerStat()->vShellRight = iter->second.vRight;
					pPlayer->Get_PlayerStat()->bTeleport = true;

					// 다른 레벨 골레포트로 이동할때
					if (iter->second.iLevelIndex != m_pGameInstance->Get_CurrentLevelIndex())
					{
						m_pGameInstance->Play_Sound(L"MSS_Teleport.wav", (_uint)SOUND_CHANNEL::UI, 0.7f);
						m_pUIManager->FadeOut();

						CCallbackUpdate* pCallbackUpdate = static_cast<CCallbackUpdate*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CallbackUpdate"));
						pCallbackUpdate->Add_Update_Callback([this, iter](_float fElapsed, _float fTimeDelta)->_bool
							{
								_bool bFadeEnd = m_pUIManager->End_FadeOut();
								if (bFadeEnd)
								{
									CTheme_Manager::Set_PendingThemeFilePath_FromStatic(iter->second.SheleportData.szThemeFilePath);
									m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, (LEVELID)iter->second.iLevelIndex));
								}

								return bFadeEnd;
							});
					}
					// 같은 레벨 골레포트로 이동할때
					else
					{
						m_pUIManager->FadeOut();

						CCallbackUpdate* pCallbackUpdate = static_cast<CCallbackUpdate*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CallbackUpdate"));
						pCallbackUpdate->Add_Update_Callback([this, pPlayer, iter](_float fElapsed, _float fTimeDelta)->_bool
							{
								_bool bFadeEnd = m_pUIManager->End_FadeOut();
								if (bFadeEnd)
								{
									pPlayer->Get_PlayerStat()->bTeleport = false;
									pPlayer->MoonShell_Interact(false);

									THEME_DESC* pThemeDesc { nullptr };
									vector<_wstring> FilePaths { iter->second.SheleportData.szThemeFilePath };
									THEME_DESC::Load_Themes(&pThemeDesc, FilePaths, 1);
									pThemeDesc->Set_Atmosphere();
									Safe_Delete(pThemeDesc);

									m_pUIManager->FadeIn();
								}
								return bFadeEnd;
							});
						
					}
					m_pUIManager->Hide_SheleportUI();
				});
		}
		else
			m_vecRightButton[i]->Set_IsActive(false);
	}
	super::Update(fTimeDelta);
}

void CUI_Sheleport::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);

	super::Late_Update(fTimeDelta);
}

HRESULT CUI_Sheleport::Render()
{
	m_pGameInstance->Render_Font(L"Font_Main_40", L"골레포트", {220.f, 135.f}, XMLoadFloat4(&m_vTextColor), 0.f, {0.f, 0.f}, 0.8f);
	return S_OK;
}

void CUI_Sheleport::Add_Visited(CShellHouse::DATA tData)
{
	//_int iSheleportNum, _wstring strLocation, _wstring strName
	m_mapVisited[tData.SheleportData.szLocation][tData.SheleportData.iSheleportNum] = tData;
}

HRESULT CUI_Sheleport::Ready_PartObject()
{
	/* Border_Left */
	CUI_DefaultStretch::DESC DefaultDesc{};
	{
		DefaultDesc.UIDesc.pOwner = this;

		DefaultDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/Sheleport/UI_Border_Left.dat";
		DefaultDesc.fStretchFactor = 13.f;
		DefaultDesc.UIDesc.bStretchY = true;
		DefaultDesc.UIDesc.bRightAngle = true;
		DefaultDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Border_Left", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &DefaultDesc)))
		return E_FAIL;

	/* Border_Left_Bottom */
	{
		DefaultDesc.UIDesc.pOwner = this;

		DefaultDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/Sheleport/UI_Border_Left_Bottom.dat";
		DefaultDesc.fStretchFactor = 7.f;
		DefaultDesc.UIDesc.bStretchY = true;
		DefaultDesc.UIDesc.bRightAngle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Border_Left_Bottom", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &DefaultDesc)))
		return E_FAIL;

	/* Border_Right */
	CUI_Texture::DESC Desc{};
	{
		Desc.UIDesc.pOwner = this;

		Desc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/Sheleport/UI_Border_Right.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Border_Right", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &Desc)))
		return E_FAIL;

	/* Border_Right_Bottom */
	{
		DefaultDesc.UIDesc.pOwner = this;

		DefaultDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/Sheleport/UI_Border_Right_Bottom.dat";
		DefaultDesc.fStretchFactor = 3.f;
		DefaultDesc.UIDesc.bStretchY = true;
		DefaultDesc.UIDesc.bRightAngle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Border_Right_Bottom", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &DefaultDesc)))
		return E_FAIL;

	CUI_Shake::DESC ShakeDesc{};
	{
		ShakeDesc.UIDesc.pOwner = this;
		ShakeDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/Sheleport/UI_Deco_Star1.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Deco1", LEVEL_STATIC, L"Prototype_GameObject_UI_Shake", &ShakeDesc)))
		return E_FAIL;

	{
		ShakeDesc.UIDesc.pOwner = this;
		ShakeDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/Sheleport/UI_Deco_Star2.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Deco2", LEVEL_STATIC, L"Prototype_GameObject_UI_Shake", &ShakeDesc)))
		return E_FAIL;

	/* Rect_Left */
	{
		DefaultDesc.UIDesc.pOwner = this;

		DefaultDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/Sheleport/UI_Rect_Left.dat";
		DefaultDesc.fStretchFactor = 20.f;
		DefaultDesc.UIDesc.bStretchY = true;
		DefaultDesc.UIDesc.bRightAngle = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Rect_Left", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &DefaultDesc)))
		return E_FAIL;

	/* Rect_Right */
	CUI_Texture::DESC TextureDesc{};
	{
		TextureDesc.UIDesc.pOwner = this;
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/Sheleport/UI_Rect_Right.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Rect_Right", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUICover = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_Rect_Right" }));

	/* Rect_Right_Bottom */
	{
		DefaultDesc.UIDesc.pOwner = this;

		DefaultDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/Sheleport/UI_Rect_Right_Bottom.dat";
		DefaultDesc.fStretchFactor = 7.f;
		DefaultDesc.UIDesc.bStretchY = true;
		DefaultDesc.UIDesc.bRightAngle = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Rect_Right_Bottom", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &DefaultDesc)))
		return E_FAIL;

	/* ESC_Button */
	CUI_Button::DESC ButtonDesc{};
	{
		ButtonDesc.UIDesc.pOwner = this;

		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/Sheleport/UI_ESC_Button.dat";
		lstrcpy(ButtonDesc.szText, TEXT("뒤로"));
		ButtonDesc.ClickCallback = [this]()->void
		{
			m_pUIManager->Hide_SheleportUI([this]()->void
				{
					m_pUIManager->Show_MoonSnailUI();
				});
		};
	}
	if (FAILED(Add_PartObject(L"Part_UI_ESC_Button", LEVEL_STATIC, L"Prototype_GameObject_UI_Button", &ButtonDesc)))
		return E_FAIL;

	_float2 vSize = { 300.f, 40.f };
	_float2 vStart = { 332.f, 226.f };

	_float vOffsetY = 5.f;

	for (_int i = 0; i < m_iLocationCount; i++)
	{
		wstring strPartTag = L"Part_UI_Left_Button" + to_wstring(i);
		_float2 vLocate = { vStart.x, vStart.y + (vSize.y + vOffsetY) * i };

		/* Left Button */
		CUI_DynamicButton::DESC DynamicButtonDesc{};
		{
			DynamicButtonDesc.UIDesc.pOwner = this;

			DynamicButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/Sheleport/UI_Left_Button.dat";
			DynamicButtonDesc.vLocate = vLocate;
		}
		if (FAILED(Add_PartObject(strPartTag, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &DynamicButtonDesc)))
			return E_FAIL;

		m_vecLeftButton.push_back(static_cast<CUI_DynamicButton*>(Find_PartObject({ strPartTag })));
	}	

	vStart = { 686.f, 345.f };

	for (_int i = 0; i < m_iMaxNameCount; i++)
	{
		wstring strPartTag = L"Part_UI_Right_Button" + to_wstring(i);
		_float2 vLocate = { vStart.x, vStart.y + (vSize.y + vOffsetY) * i };

		CUI_DynamicButton::DESC DynamicButtonDesc{};
		{
			DynamicButtonDesc.UIDesc.pOwner = this;

			DynamicButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/MoonSnail/Sheleport/UI_Right_Button.dat";
			DynamicButtonDesc.vLocate = vLocate;
		}
		if (FAILED(Add_PartObject(strPartTag, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &DynamicButtonDesc)))
			return E_FAIL;

		m_vecRightButton.push_back(static_cast<CUI_DynamicButton*>(Find_PartObject({ strPartTag })));
	}
	

	return S_OK;
}

void CUI_Sheleport::Calculate_Count()
{
	// 위치가 총 몇개고, 한 맵에 가장 많이 찍힌 개수

	vector<FSheleportData> vecData = CFileData_Locator<FSheleportData>::Get_FileData()->Get_Datas(0);
	_int iSheleportCount = (_int)vecData.size();

	map<_wstring, _int> vecCount;

	for (_int i = 0; i < iSheleportCount; i++)
	{
		vecCount[vecData[i].szLocation]++;

		if (m_iMaxNameCount < vecCount[vecData[i].szLocation])
			m_iMaxNameCount = vecCount[vecData[i].szLocation];
	}

	m_iLocationCount = (_int)vecCount.size();
}

CUI_Sheleport* CUI_Sheleport::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Sheleport* pInstance = new CUI_Sheleport(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Sheleport");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Sheleport::Clone(void* pArg)
{
	CUI_Sheleport* pInstance = new CUI_Sheleport(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Sheleport");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Sheleport::Free()
{
    CUIObject::Free();
}

