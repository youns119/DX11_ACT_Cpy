#include "pch.h"
#include "UI_PlayerShellDesc.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "FShell.h"
#include "FileData_Locator.h"
#include "FData.h"
#include "Shell_Part.h"
#include "Player.h"

#include "UI_Line.h"
#include "UI_texture.h"
#include "UI_DefaultStretch.h"
#include "UI_FixStretchBar.h"
#include "UI_Button.h"

CUI_PlayerShellDesc::CUI_PlayerShellDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_PlayerShellDesc::CUI_PlayerShellDesc(const CUI_PlayerShellDesc& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_PlayerShellDesc::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_PlayerShellDesc::Initialize(void* pArg)
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

	m_pOwner = pDesc->pOwner;

	m_pPlayerStat = pDesc->pPlayerStat;
	m_ppShellStat = pDesc->ppShellStat;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	return S_OK;
}

void CUI_PlayerShellDesc::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_PlayerShellDesc::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (*m_ppShellStat)
	{
		_int iTextureNum = (_int)(*m_ppShellStat)->eShell;
		FShellDescData tData = CFileData_Locator<FShellDescData>::Get_FileData()->Get_Data(0, iTextureNum);

		m_pUIIcon->Change_TextureNum(iTextureNum);
		m_pUILine->Set_Text(tData.szShellName);

		m_pUIDefBar->Set_BarValue(&(*m_ppShellStat)->fShellCurrHP, &(*m_ppShellStat)->fShellMaxHP);

		m_pUIBar->Set_Text((*m_ppShellStat)->fShellMaxHP);

		m_pUIDefBar->Set_Text((*m_ppShellStat)->fShellCurrHP);

		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back());
 
		_float fSkillPoint = pPlayer->Get_PlayerShell()->Get_SkillPoint();

		if (fSkillPoint == 0.f)
		{
			m_pUISkillPoint[0]->Set_IsActive(false);
			m_pUISkillPoint[1]->Set_IsActive(false);
		}
		else if (fSkillPoint == 1.f)
		{
			m_pUISkillPoint[0]->Set_IsActive(true);
			m_pUISkillPoint[0]->Set_Locate({640.f, 385.f, 0.5f});
			m_pUISkillPoint[1]->Set_IsActive(false);
		}
		else if (fSkillPoint == 2.f)
		{
			m_pUISkillPoint[0]->Set_IsActive(true);
			m_pUISkillPoint[0]->Set_Locate({ 630.f, 385.f, 0.5f });

			m_pUISkillPoint[1]->Set_IsActive(true);
			m_pUISkillPoint[1]->Set_Locate({ 650.f, 385.f, 0.5f });

		}
	}
	
	super::Update(fTimeDelta);
}

void CUI_PlayerShellDesc::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);

		super::Late_Update(fTimeDelta);
	}
}

HRESULT CUI_PlayerShellDesc::Render()
{
	if (*m_ppShellStat)
	{
		_wstring strShellDef = to_wstring((_int)(*m_ppShellStat)->fShellDefense);

		_wstring strShellWeight = {};
		switch ((_int)(*m_ppShellStat)->eShellWeight)
		{
		case 0:
			strShellWeight = L"S";
			break;
		case 1:
			strShellWeight = L"M";
			break;
		case 2:
			strShellWeight = L"L";
			break;
		}

		FShellDescData tData = CFileData_Locator<FShellDescData>::Get_FileData()->Get_Data(0, (_int)(*m_ppShellStat)->eShell);

		_wstring strSkillName = tData.szSkillName;

		_float2 vDefText = { 690.f, 330.f };
		_float2 vWeightText = { 690.f, 310.f };
		_float2 vSkillNameText = { 640.f, 370.f };

		m_pGameInstance->Render_Font(L"Font_Cafe_10", strShellDef.c_str(), vDefText, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::RIGHT);
		m_pGameInstance->Render_Font(L"Font_Cafe_10", strShellWeight.c_str(), vWeightText, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::RIGHT);
		m_pGameInstance->Render_Font(L"Font_Main_40", strSkillName.c_str(), vSkillNameText, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 0.3f, SORTING::MIDDLE);
	}
	
	return S_OK;
}

HRESULT CUI_PlayerShellDesc::Ready_PartObject()
{
	/* Border */
	CUI_DefaultStretch::DESC StretchDesc{};
	{
		StretchDesc.UIDesc.pOwner = m_pOwner;
		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Shell/UI_Shell_Border.dat";
		
		StretchDesc.fStretchFactor = 10.f;
		StretchDesc.UIDesc.bStretchY = true;
		StretchDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Border", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	/* Bar */
	{
		StretchDesc.UIDesc.pOwner = m_pOwner;
		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Shell/UI_Shell_Bar.dat";

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
		FixStretchBarDesc.UIDesc.pOwner = m_pOwner;
		FixStretchBarDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Shell/UI_Shell_DefBar.dat";

		FixStretchBarDesc.bMiddle = true;
		FixStretchBarDesc.fStartFactor = 210.f;
		FixStretchBarDesc.fEndFactor = 210.f;

		//FixStretchBarDesc.pCur = &pPlayer->Get_();
		//FixStretchBarDesc.pMax = &m_pShellStat->fShellMaxHP;

		FixStretchBarDesc.UIDesc.eSorting = SORTING::MIDDLE;
		FixStretchBarDesc.UIDesc.bStretchY = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_DefBar", LEVEL_STATIC, L"Prototype_GameObject_UI_FixStretchBar", &FixStretchBarDesc)))
		return E_FAIL;

	m_pUIDefBar = static_cast<CUI_FixStretchBar*>(Find_PartObject({ L"Part_UI_DefBar" }));

	/* Icon */
	CUI_Texture::DESC TextureDesc{};
	{
		TextureDesc.UIDesc.pOwner = m_pOwner;
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Shell/UI_Shell.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Icon", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUIIcon = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_Icon" }));

	/* Line */
	CUI_Line::DESC LineDesc{};
	{
		LineDesc.UIDesc.pOwner = m_pOwner;
		LineDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Shell/UI_Shell_Line.dat";
		LineDesc.UIDesc.eSorting = SORTING::MIDDLE;
		LineDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Line", LEVEL_STATIC, L"Prototype_GameObject_UI_Line", &LineDesc)))
		return E_FAIL;

	m_pUILine = static_cast<CUI_Line*>(Find_PartObject({ L"Part_UI_Line" }));

	{
		TextureDesc.UIDesc.pOwner = m_pOwner;
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Shell/UI_Shell_DefIcon.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_DefIcon", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUIDefIcon = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_DefIcon" }));
	m_pUIDefIcon->Set_Text(L"방어력");

	{
		TextureDesc.UIDesc.pOwner = m_pOwner;
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Shell/UI_Shell_WeightIcon.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_WeightIcon", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUIWeightIcon = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_WeightIcon" }));
	m_pUIWeightIcon->Set_Text(L"무게");

	/* StatLine */
	{
		TextureDesc.UIDesc.pOwner = m_pOwner;
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Shell/UI_Shell_StatLine.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_StatLine", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	/* PointStar */
	{
		TextureDesc.UIDesc.pOwner = m_pOwner;
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Shell/UI_Shell_Point.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Point1", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUISkillPoint[0] = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_Point1" }));

	/* PointStar */
	{
		TextureDesc.UIDesc.pOwner = m_pOwner;
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Shell/UI_Shell_Point.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Point2", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUISkillPoint[1] = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_Point2" }));

	return S_OK;
}

CUI_PlayerShellDesc* CUI_PlayerShellDesc::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_PlayerShellDesc* pInstance = new CUI_PlayerShellDesc(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_PlayerShellDesc");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_PlayerShellDesc::Clone(void* pArg)
{
	CUI_PlayerShellDesc* pInstance = new CUI_PlayerShellDesc(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_PlayerShellDesc");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PlayerShellDesc::Free()
{
    CUIObject::Free();
}

