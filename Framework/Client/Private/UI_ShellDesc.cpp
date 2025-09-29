#include "pch.h"
#include "UI_ShellDesc.h"

#include "GameInstance.h"
#include "FShell.h"
#include "FileData_Locator.h"
#include "FData.h"
#include "Player.h"
#include "Shell_Part.h"
#include "UI_Manager.h"

#include "UI_Line.h"
#include "UI_texture.h"
#include "UI_DefaultStretch.h"
#include "UI_FixStretchBar.h"

CUI_ShellDesc::CUI_ShellDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_ShellDesc::CUI_ShellDesc(const CUI_ShellDesc& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_ShellDesc::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_ShellDesc::Initialize(void* pArg)
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

	m_pShell = pDesc->pShell;
	m_pShellStat = pDesc->pShellStat;

	m_isActive = false;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	return S_OK;
}

void CUI_ShellDesc::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_ShellDesc::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_pUIBorder->Set_ObjectLocate(m_pShell, {0.f, -200.f});
	m_pUIStatLine->Set_ObjectLocate(m_pShell, { 100.f, -285.f });
	m_pUILine->Set_ObjectLocate(m_pShell, {0.f, -210.f});
	m_pUIStar->Set_ObjectLocate(m_pShell, { 0.f, -90.f });
	m_pUIDefIcon->Set_ObjectLocate(m_pShell, { -50.f, -190.f });
	m_pUIWeightIcon->Set_ObjectLocate(m_pShell, { -50.f, -170.f });

	m_pUIIcon->Set_ObjectLocate(m_pShell, {0.f, -290.f});
	m_pUIIcon->Change_TextureNum((_uint)m_pShellStat->eShell);

	m_pUIBar->Set_ObjectLocate(m_pShell, { 100.f, -180.f });
	m_pUIBar->Set_Text(m_pShellStat->fShellMaxHP);

	m_pUIDefBar->Set_ObjectLocate(m_pShell, { 100.f, -180.f });
	m_pUIDefBar->Set_Text(m_pShellStat->fShellCurrHP);

	m_pUICompareLine->Set_ObjectLocate(m_pShell, { 60.f, -285.f });
	m_pUIArrow->Set_ObjectLocate(m_pShell, { 80.f, -285.f });

	m_pUIPointArrow->Set_ObjectLocate(m_pShell, {0.f, -122.5f});

	FShellDescData tData = CFileData_Locator<FShellDescData>::Get_FileData()->Get_Data(0, (_int)m_pShellStat->eShell);

	m_pUILine->Set_Text(tData.szShellName);
	

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player")));
	CShell_Part* pPlayerShell = { nullptr };
	if(pPlayer)
		pPlayerShell = pPlayer->Get_PlayerShell();

	if (pPlayerShell)
	{
		m_pUICompareLine->Set_IsActive(true);
		m_pUIArrow->Set_IsActive(true);
		m_pUIPointArrow->Set_IsActive(true);

		_float fPlayerShellSkillPoint = pPlayerShell->Get_SkillPoint();
		_float fCurShellSkillPoint = m_pShellStat->fSkillPoint;

		if (fPlayerShellSkillPoint == 0.f)
		{
			m_pUIPoint[0]->Set_IsActive(false);
			m_pUIPoint[1]->Set_IsActive(false);

			if (fCurShellSkillPoint == 0.f)
			{
				m_pUIPoint[2]->Set_IsActive(false);
				m_pUIPoint[3]->Set_IsActive(false);
			}
			else if (fCurShellSkillPoint == 1.f)
			{
				m_pUIPoint[2]->Set_IsActive(true);
				m_pUIPoint[2]->Set_ObjectLocate(m_pShell, { 40.f, -115.f });

				m_pUIPoint[3]->Set_IsActive(false);
			}
			else if (fCurShellSkillPoint == 2.f)
			{
				m_pUIPoint[2]->Set_IsActive(true);
				m_pUIPoint[2]->Set_ObjectLocate(m_pShell, { 33.f, -115.f });

				m_pUIPoint[3]->Set_IsActive(true);
				m_pUIPoint[3]->Set_ObjectLocate(m_pShell, { 47.f, -115.f });
			}
		}
		else if (fPlayerShellSkillPoint == 1.f)
		{
			m_pUIPoint[0]->Set_IsActive(true);
			m_pUIPoint[0]->Set_ObjectLocate(m_pShell, {-40.f, -115.f });

			m_pUIPoint[1]->Set_IsActive(false);

			if (fCurShellSkillPoint == 0.f)
			{
				m_pUIPoint[2]->Set_IsActive(false);
				m_pUIPoint[3]->Set_IsActive(false);
			}
			else if (fCurShellSkillPoint == 1.f)
			{
				m_pUIPoint[2]->Set_IsActive(true);
				m_pUIPoint[2]->Set_ObjectLocate(m_pShell, { 40.f, -115.f });

				m_pUIPoint[3]->Set_IsActive(false);
			}
			else if (fCurShellSkillPoint == 2.f)
			{
				m_pUIPoint[2]->Set_IsActive(true);
				m_pUIPoint[2]->Set_ObjectLocate(m_pShell, { 33.f, -115.f });

				m_pUIPoint[3]->Set_IsActive(true);
				m_pUIPoint[3]->Set_ObjectLocate(m_pShell, { 47.f, -115.f });
			}
		}
		else if (fPlayerShellSkillPoint == 2.f)
		{
			m_pUIPoint[0]->Set_IsActive(true);
			m_pUIPoint[0]->Set_ObjectLocate(m_pShell, { -47.f, -115.f });

			m_pUIPoint[1]->Set_IsActive(true);
			m_pUIPoint[1]->Set_ObjectLocate(m_pShell, { -33.f, -115.f });

			if (fCurShellSkillPoint == 0.f)
			{
				m_pUIPoint[2]->Set_IsActive(false);
				m_pUIPoint[3]->Set_IsActive(false);
			}
			else if (fCurShellSkillPoint == 1.f)
			{
				m_pUIPoint[2]->Set_IsActive(true);
				m_pUIPoint[2]->Set_ObjectLocate(m_pShell, { 40.f, -115.f });

				m_pUIPoint[3]->Set_IsActive(false);
			}
			else if (fCurShellSkillPoint == 2.f)
			{
				m_pUIPoint[2]->Set_IsActive(true);
				m_pUIPoint[2]->Set_ObjectLocate(m_pShell, { 33.f, -115.f });

				m_pUIPoint[3]->Set_IsActive(true);
				m_pUIPoint[3]->Set_ObjectLocate(m_pShell, { 47.f, -115.f });
			}
		}
	}
	else
	{
		m_pUIPoint[0]->Set_IsActive(false);
		m_pUIPoint[1]->Set_IsActive(false);
		m_pUIPointArrow->Set_IsActive(false);

		_float fCurShellSkillPoint = m_pShellStat->fSkillPoint;

		m_pUICompareLine->Set_IsActive(false);
		m_pUIArrow->Set_IsActive(false);

		if (fCurShellSkillPoint == 0.f)
		{
			m_pUIPoint[2]->Set_IsActive(false);
			m_pUIPoint[3]->Set_IsActive(false);
		}
		else if (fCurShellSkillPoint == 1.f)
		{
			m_pUIPoint[2]->Set_IsActive(true);
			m_pUIPoint[2]->Set_ObjectLocate(m_pShell, { 0.f, -115.f });

			m_pUIPoint[3]->Set_IsActive(false);
		}
		else if (fCurShellSkillPoint == 2.f)
		{
			m_pUIPoint[2]->Set_IsActive(true);
			m_pUIPoint[2]->Set_ObjectLocate(m_pShell, { -7.f, -115.f });

			m_pUIPoint[3]->Set_IsActive(true);
			m_pUIPoint[3]->Set_ObjectLocate(m_pShell, { 7.f, -115.f });
		}
	}

	super::Update(fTimeDelta);
}

void CUI_ShellDesc::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (m_pUIManager->Get_IsActive_MenuUI())
		return;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	
	super::Late_Update(fTimeDelta);	
}

HRESULT CUI_ShellDesc::Render()
{
	_wstring strFinalDef = {};
	_wstring strFinalWeight = {};
	_wstring strFinalSkillName = {};

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back());
	if (pPlayer->Get_PlayerShell())
	{
		_wstring strPlayerDef = to_wstring((_int)pPlayer->Get_PlayerShellStat_Const()->fShellDefense);
		_wstring strDef = to_wstring((_int)m_pShellStat->fShellDefense);

		strFinalDef = strPlayerDef + L" -> " + strDef;

		_wstring strPlayerWeight = {};
		switch ((_int)pPlayer->Get_PlayerShellStat_Const()->eShellWeight)
		{
		case 0:
			strPlayerWeight = L"S";
			break;
		case 1:
			strPlayerWeight = L"M";
			break;
		case 2:
			strPlayerWeight = L"L";
			break;
		}

		_wstring strWeight = {};
		switch ((_int)m_pShellStat->eShellWeight)
		{
		case 0:
			strWeight = L"S";
			break;
		case 1:
			strWeight = L"M";
			break;
		case 2:
			strWeight = L"L";
			break;
		}


		strFinalWeight = strPlayerWeight + L" -> " + strWeight;

		_float2 vCurText = m_pUICompareLine->Get_ObjectLocate(m_pShell, { 60.f, -295.f });
		_float2 vMaxText = m_pUICompareLine->Get_ObjectLocate(m_pShell, { 60.f, -275.f });

		_wstring strCur = to_wstring((_int)pPlayer->Get_PlayerShellStat_Const()->fShellCurrHP);
		_wstring strMax = to_wstring((_int)pPlayer->Get_PlayerShellStat_Const()->fShellMaxHP);

		m_pGameInstance->Render_Font(L"Font_Cafe_10", strCur.c_str(), vCurText, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::MIDDLE);
		m_pGameInstance->Render_Font(L"Font_Cafe_10", strMax.c_str(), vMaxText, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::MIDDLE);

		FShellDescData tPlayerData = CFileData_Locator<FShellDescData>::Get_FileData()->Get_Data(0, (_int)pPlayer->Get_PlayerShell()->Get_ShellStat()->eShell);
		FShellDescData tData = CFileData_Locator<FShellDescData>::Get_FileData()->Get_Data(0, (_int)m_pShellStat->eShell);

		_wstring strPlayerSkillName = tPlayerData.szSkillName;
		strFinalSkillName = tData.szSkillName;

		_float2 vPlayerSkillNameText = m_pUIWeightIcon->Get_ObjectLocate(m_pShell, { -40.f, -130.f });
		m_pGameInstance->Render_Font(L"Font_Main_40", strPlayerSkillName.c_str(), vPlayerSkillNameText, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 0.3f, SORTING::MIDDLE);

		_float2 vSkillNameText = m_pUIWeightIcon->Get_ObjectLocate(m_pShell, { 40.f, -130.f });
		m_pGameInstance->Render_Font(L"Font_Main_40", strFinalSkillName.c_str(), vSkillNameText, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 0.3f, SORTING::MIDDLE);
	}
	else
	{
		strFinalDef = to_wstring((_int)m_pShellStat->fShellDefense);
		switch ((_int)m_pShellStat->eShellWeight)
		{
		case 0:
			strFinalWeight = L"S";
			break;
		case 1:
			strFinalWeight = L"M";
			break;
		case 2:
			strFinalWeight = L"L";
			break;
		}

		FShellDescData tData = CFileData_Locator<FShellDescData>::Get_FileData()->Get_Data(0, (_int)m_pShellStat->eShell);

		strFinalSkillName = tData.szSkillName;

		_float2 vSkillNameText = m_pUIWeightIcon->Get_ObjectLocate(m_pShell, { 0.f, -130.f });
		m_pGameInstance->Render_Font(L"Font_Main_40", strFinalSkillName.c_str(), vSkillNameText, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 0.3f, SORTING::MIDDLE);
	}
	
	_float2 vDefText = m_pUIDefIcon->Get_ObjectLocate(m_pShell, { 60.f, -190.f });
	_float2 vWeightText = m_pUIWeightIcon->Get_ObjectLocate(m_pShell, { 60.f, -170.f });

	m_pGameInstance->Render_Font(L"Font_Cafe_10", strFinalDef.c_str(), vDefText, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::RIGHT);
	m_pGameInstance->Render_Font(L"Font_Cafe_10", strFinalWeight.c_str(), vWeightText, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::RIGHT);
	return S_OK;
}

HRESULT CUI_ShellDesc::Ready_PartObject()
{
	/* Border */
	CUI_DefaultStretch::DESC StretchDesc{};
	{
		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shell/UI_ShellBorder.dat";
		
		StretchDesc.fStretchFactor = 10.f;
		StretchDesc.UIDesc.bStretchY = true;
		StretchDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Border", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	m_pUIBorder = static_cast<CUI_DefaultStretch*>(Find_PartObject({ L"Part_UI_Border" }));

	/* Bar */
	{
		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shell/UI_ShellBar.dat";

		StretchDesc.fStretchFactor = 130.f;
		StretchDesc.UIDesc.bStretchY = false;
		StretchDesc.bMiddle = true;
		StretchDesc.UIDesc.eSorting = SORTING::MIDDLE;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Bar", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	m_pUIBar = static_cast<CUI_DefaultStretch*>(Find_PartObject({ L"Part_UI_Bar" }));

	/* DefBar */
	CUI_FixStretchBar::DESC FixStretchBarDesc{};
	{
		FixStretchBarDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shell/UI_ShellDefBar.dat";

		FixStretchBarDesc.bMiddle = true;
		FixStretchBarDesc.fStartFactor = 210.f;
		FixStretchBarDesc.fEndFactor = 210.f;

		FixStretchBarDesc.pCur = &m_pShellStat->fShellCurrHP;
		FixStretchBarDesc.pMax = &m_pShellStat->fShellMaxHP;

		FixStretchBarDesc.UIDesc.eSorting = SORTING::MIDDLE;
	}
	if (FAILED(Add_PartObject(L"Part_UI_DefBar", LEVEL_STATIC, L"Prototype_GameObject_UI_FixStretchBar", &FixStretchBarDesc)))
		return E_FAIL;

	m_pUIDefBar = static_cast<CUI_FixStretchBar*>(Find_PartObject({ L"Part_UI_DefBar" }));

	/* Icon */
	CUI_Texture::DESC TextureDesc{};
	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shell/UI_Shell.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Icon", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUIIcon = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_Icon" }));

	/* Line */
	CUI_Line::DESC LineDesc{};
	{
		LineDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shell/UI_ShellLine.dat";
		LineDesc.UIDesc.eSorting = SORTING::MIDDLE;
		LineDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Line", LEVEL_STATIC, L"Prototype_GameObject_UI_Line", &LineDesc)))
		return E_FAIL;

	m_pUILine = static_cast<CUI_Line*>(Find_PartObject({ L"Part_UI_Line" }));

	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shell/UI_ShellStar.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Star", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUIStar = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_Star" }));

	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shell/UI_ShellDefIcon.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_DefIcon", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUIDefIcon = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_DefIcon" }));
	m_pUIDefIcon->Set_Text(L"방어력");

	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shell/UI_ShellWeightIcon.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_WeightIcon", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUIWeightIcon = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_WeightIcon" }));
	m_pUIWeightIcon->Set_Text(L"무게");

	/* StatLine */
	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shell/UI_ShellStatLine.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_StatLine", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUIStatLine = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_StatLine" }));

	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shell/UI_ShellStatLineCompare.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_CompareLine", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUICompareLine = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_CompareLine" }));

	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shell/UI_ShellCompareArrow.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Arrow", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUIArrow = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_Arrow" }));

	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shell/UI_ShellPointCompareArrow.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_PointArrow", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUIPointArrow = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_PointArrow" }));

	for (_int i = 0; i < 4; i++)
	{
		_wstring strTag = L"Part_UI_Point" + to_wstring(i);

		{
			TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shell/UI_ShellPoint.dat";
		}
		if (FAILED(Add_PartObject(strTag, LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
			return E_FAIL;

		m_pUIPoint[i] = static_cast<CUI_Texture*>(Find_PartObject({ strTag }));
	}
	
	return S_OK;
}

CUI_ShellDesc* CUI_ShellDesc::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ShellDesc* pInstance = new CUI_ShellDesc(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_ShellDesc");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_ShellDesc::Clone(void* pArg)
{
	CUI_ShellDesc* pInstance = new CUI_ShellDesc(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_ShellDesc");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_ShellDesc::Free()
{
    CUIObject::Free();
}

