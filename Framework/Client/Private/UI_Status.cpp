#include "pch.h"
#include "UI_Status.h"

#include "GameInstance.h"
#include "FileData_Locator.h"
#include "FData.h"
#include "UI_Manager.h"
#include "FPlayerStat.h"
#include "Player.h"
#include "Skill_DataBase.h"

#include "UI_PlayerShellDesc.h"
#include "UI_DefaultStretch.h"
#include "UI_StatusButton.h"
#include "UI_Description.h"
#include "UI_Button.h"
#include "UI_DynamicTexture.h"
#include "UI_DynamicButton.h"


CUI_Status::CUI_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Status::CUI_Status(const CUI_Status& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_Status::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Status::Initialize(void* pArg)
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
	m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	if (FAILED(CUIObject::Initialize(&pDesc->UIObjectDesc)))
		return E_FAIL;

	m_pOwner = pDesc->pOwner;
	m_pPlayerStat = pDesc->pPlayerStat;
	m_ppShellStat = pDesc->ppShellStat;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CUI_Status::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (m_pUIManager->Get_CopyTexture_IsActive())
		m_eRenderGroup = CRenderer::RENDERGROUP::UI_ANIM;
	else
		m_eRenderGroup = CRenderer::RENDERGROUP::UI;

	super::Priority_Update(fTimeDelta);
}

void CUI_Status::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	Setting_ShellData();
	Setting_ActiveSkillData();
	Setting_PassiveSkillData();

	super::Update(fTimeDelta);
}

void CUI_Status::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);

	super::Late_Update(fTimeDelta);

}

HRESULT CUI_Status::Render()
{
	m_pGameInstance->Render_Font(L"Font_Main_40", L"해금", { 200.f, 160.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 0.75f);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"크릴", { 320.f, 160.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 0.75f);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"적응", { 320.f, 420.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 0.75f);
	
	_wstring strLevel = to_wstring(m_pPlayerStat->iLevel);
	m_pGameInstance->Render_Font(L"Font_Cafe_15", L"레벨", {380.f, 220.f}, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Font(L"Font_Cafe_15", strLevel.c_str(), {460.f, 231.f}, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, {0.f, 0.f}, 1.f, SORTING::RIGHT);
   
	return S_OK;
}

HRESULT CUI_Status::Ready_PartObject()
{
	CUI_DefaultStretch::DESC StretchDesc{};
	{
		StretchDesc.UIDesc.pOwner = m_pOwner;

		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_Border.dat";
		StretchDesc.fStretchFactor = 11.f;
		StretchDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Border", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	{
		StretchDesc.UIDesc.pOwner = m_pOwner;

		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_IconBorder.dat";
		StretchDesc.fStretchFactor = 15.f;
		StretchDesc.bMiddle = true;
		StretchDesc.UIDesc.bStretchY = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_IconBorder", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	{
		StretchDesc.UIDesc.pOwner = m_pOwner;

		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_WidthLine.dat";
		StretchDesc.fStretchFactor = 365.f;
		StretchDesc.bMiddle = true;
		StretchDesc.UIDesc.bStretchY = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_WidthLine", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	{
		StretchDesc.UIDesc.pOwner = m_pOwner;

		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_HeightLine.dat";
		StretchDesc.fStretchFactor = 335.f;
		StretchDesc.bMiddle = true;
		StretchDesc.UIDesc.bStretchY = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_HeightLine", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	CUI_StatusButton::DESC ButtonDesc{};
	{
		ButtonDesc.UIDesc.pOwner = m_pOwner;

		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_Button1.dat";
		ButtonDesc.fStretchFactor = 80.f;
		ButtonDesc.strIconFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_Icon1.dat";
		ButtonDesc.pCount = &m_pPlayerStat->iVitality;
		ButtonDesc.HoverCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
				m_pUIDesc->Change_Texture(m_strStatIconTextureTag, 0);
				m_pUIDesc->Set_Name(L"생명력");
				m_pUIDesc->Set_Description(L"내 체력의 양을 증가시킵니다.");

				m_pUIDesc->Change_IconShaderPass((_uint)SHADER_PASS_UI::BLEND);
				m_pUIDesc->Show(true);
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_StatButton1", LEVEL_STATIC, L"Prototype_GameObject_UI_StatusButton", &ButtonDesc)))
		return E_FAIL;

	m_pStatusButton[0] = static_cast<CUI_StatusButton*>(Find_PartObject({ L"Part_UI_StatButton1" }));

	{
		ButtonDesc.UIDesc.pOwner = m_pOwner;

		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_Button2.dat";
		ButtonDesc.fStretchFactor = 80.f;
		ButtonDesc.strIconFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_Icon2.dat";
		ButtonDesc.pCount = &m_pPlayerStat->iResistance;
		ButtonDesc.HoverCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
				m_pUIDesc->Change_Texture(m_strStatIconTextureTag, 1);
				m_pUIDesc->Set_Name(L"저항력");
				m_pUIDesc->Set_Description(L"등딱지가 받는 대미지를 감소시키고, 상태 효과 저항력을 증가시킵니다.");

				m_pUIDesc->Change_IconShaderPass((_uint)SHADER_PASS_UI::BLEND);
				m_pUIDesc->Show(true);
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_StatButton2", LEVEL_STATIC, L"Prototype_GameObject_UI_StatusButton", &ButtonDesc)))
		return E_FAIL;

	m_pStatusButton[1] = static_cast<CUI_StatusButton*>(Find_PartObject({ L"Part_UI_StatButton2" }));

	{
		ButtonDesc.UIDesc.pOwner = m_pOwner;

		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_Button3.dat";
		ButtonDesc.fStretchFactor = 80.f;
		ButtonDesc.strIconFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_Icon3.dat";
		ButtonDesc.pCount = &m_pPlayerStat->iAttackPower;
		ButtonDesc.HoverCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
				m_pUIDesc->Change_Texture(m_strStatIconTextureTag, 2);
				m_pUIDesc->Set_Name(L"공격력");
				m_pUIDesc->Set_Description(L"물리적인 공격의 대미지를 강화합니다.");

				m_pUIDesc->Change_IconShaderPass((_uint)SHADER_PASS_UI::BLEND);
				m_pUIDesc->Show(true);
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_StatButton3", LEVEL_STATIC, L"Prototype_GameObject_UI_StatusButton", &ButtonDesc)))
		return E_FAIL;

	m_pStatusButton[2] = static_cast<CUI_StatusButton*>(Find_PartObject({ L"Part_UI_StatButton3" }));

	{
		ButtonDesc.UIDesc.pOwner = m_pOwner;

		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_Button4.dat";
		ButtonDesc.fStretchFactor = 80.f;
		ButtonDesc.strIconFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_Icon4.dat";
		ButtonDesc.pCount = &m_pPlayerStat->iMSG;
		ButtonDesc.HoverCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
				m_pUIDesc->Change_Texture(m_strStatIconTextureTag, 3);
				m_pUIDesc->Set_Name(L"MSG");
				m_pUIDesc->Set_Description(L"감칠맛 기반 공격의 대미지를 강화합니다.");

				m_pUIDesc->Change_IconShaderPass((_uint)SHADER_PASS_UI::BLEND);
				m_pUIDesc->Show(true);
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_StatButton4", LEVEL_STATIC, L"Prototype_GameObject_UI_StatusButton", &ButtonDesc)))
		return E_FAIL;

	m_pStatusButton[3] = static_cast<CUI_StatusButton*>(Find_PartObject({ L"Part_UI_StatButton4" }));

	{
		ButtonDesc.UIDesc.pOwner = m_pOwner;

		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_Button5.dat";
		ButtonDesc.fStretchFactor = 80.f;
		ButtonDesc.strIconFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_Icon5.dat";
		ButtonDesc.pCount = &m_pPlayerStat->iDefense;
		ButtonDesc.HoverCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
				m_pUIDesc->Change_Texture(m_strStatIconTextureTag, 4);
				m_pUIDesc->Set_Name(L"방어력");
				m_pUIDesc->Set_Description(L"받는 대미지를 감소시킵니다.");

				m_pUIDesc->Change_IconShaderPass((_uint)SHADER_PASS_UI::BLEND);
				m_pUIDesc->Show(true);
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_StatButton5", LEVEL_STATIC, L"Prototype_GameObject_UI_StatusButton", &ButtonDesc)))
		return E_FAIL;

	m_pStatusButton[4] = static_cast<CUI_StatusButton*>(Find_PartObject({ L"Part_UI_StatButton5" }));

	CUI_PlayerShellDesc::DESC ShellDesc{};
	{
		ShellDesc.pOwner = m_pOwner;

		ShellDesc.pPlayerStat = m_pPlayerStat;
		ShellDesc.ppShellStat = m_ppShellStat;
	}
	if (FAILED(Add_PartObject(L"Part_UI_ShellDesc", LEVEL_STATIC, L"Prototype_GameObject_UI_PlayerShellDesc", &ShellDesc)))
		return E_FAIL;

	m_pUIShellDesc = static_cast<CUI_PlayerShellDesc*>(Find_PartObject({ L"Part_UI_ShellDesc" }));

	{
		StretchDesc.UIDesc.pOwner = m_pOwner;

		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Shell/UI_Shell_Nothing.dat";
		StretchDesc.fStretchFactor = 10.f;
		StretchDesc.bMiddle = true;
		StretchDesc.UIDesc.bStretchY = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Nothing", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	CUI_Button::DESC ShellButtonDesc{};
	{
		ShellButtonDesc.UIDesc.pOwner = m_pOwner;

		ShellButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Shell/UI_Shell_Button.dat";
		ShellButtonDesc.UIDesc.bStretchY = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Button", LEVEL_STATIC, L"Prototype_GameObject_UI_Button", &ShellButtonDesc)))
		return E_FAIL;

	m_pUIShellButton = static_cast<CUI_Button*>(Find_PartObject({ L"Part_UI_Button" }));


	_float2 vStart = { 450.f, 505.f };
	_float fOffsetX = { 120.f };

	CUI_DynamicButton::DESC SkillButtonDesc{};
	SkillButtonDesc.UIDesc.pOwner = m_pOwner;
	SkillButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_Skill_Button.dat";
	SkillButtonDesc.UIDesc.isActive = false;

	CUI_DynamicTexture::DESC SkillDesc{};
	SkillDesc.UIDesc.pOwner = m_pOwner;
	SkillDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_Skill.dat";
	SkillDesc.UIDesc.isActive = false;

	CUI_DynamicTexture::DESC SkillBorderDesc{};
	SkillBorderDesc.UIDesc.pOwner = m_pOwner;
	SkillBorderDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_SkillChoice.dat";
	SkillBorderDesc.UIDesc.isActive = false;

	for (_int i = 0; i < 3; i++)
	{
		wstring strSkillButtonPartTag = L"Part_UI_SkillButton" + to_wstring(i);
		wstring strSkillPartTag = L"Part_UI_Skill" + to_wstring(i);
		wstring strBorderPartTag = L"Part_UI_SkillBorder" + to_wstring(i);

		_float2 vLocate = { vStart.x + fOffsetX * i, vStart.y };
		SkillButtonDesc.vLocate = vLocate;
		SkillDesc.vLocate = vLocate;
		SkillBorderDesc.vLocate = vLocate;

		if (FAILED(Add_PartObject(strSkillButtonPartTag, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &SkillButtonDesc)))
			return E_FAIL;

		if (FAILED(Add_PartObject(strSkillPartTag, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &SkillDesc)))
			return E_FAIL;

		if (FAILED(Add_PartObject(strBorderPartTag, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &SkillBorderDesc)))
			return E_FAIL;

		m_pUISkillButton[i] = static_cast<CUI_DynamicButton*>(Find_PartObject({ strSkillButtonPartTag }));
		m_pUISkillIcon[i] = static_cast<CUI_DynamicTexture*>(Find_PartObject({ strSkillPartTag }));
		m_pUISkillBorder[i] = static_cast<CUI_DynamicTexture*>(Find_PartObject({ strBorderPartTag }));
	}

	SkillButtonDesc.UIDesc.pOwner = m_pOwner;
	SkillButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_PassiveSkill_Button.dat";
	SkillButtonDesc.UIDesc.isActive = false;

	SkillDesc.UIDesc.pOwner = m_pOwner;
	SkillDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/UI_Status_PassiveSkill.dat";
	SkillDesc.UIDesc.isActive = false;

	vStart = { 240.f, 270.f };
	_float fOffsetY = { 80.f };

	for (_int i = 0; i < 4; i++)
	{
		wstring strSkillButtonPartTag = L"Part_UI_PassiveSkillButton" + to_wstring(i);
		wstring strSkillPartTag = L"Part_UI_PassiveSkill" + to_wstring(i);

		_float2 vLocate = { vStart.x, vStart.y + fOffsetY * i };
		SkillButtonDesc.vLocate = vLocate;
		SkillDesc.vLocate = vLocate;
		SkillBorderDesc.vLocate = vLocate;

		if (FAILED(Add_PartObject(strSkillButtonPartTag, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &SkillButtonDesc)))
			return E_FAIL;

		if (FAILED(Add_PartObject(strSkillPartTag, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &SkillDesc)))
			return E_FAIL;

		m_pUIPassiveSkillButton[i] = static_cast<CUI_DynamicButton*>(Find_PartObject({ strSkillButtonPartTag }));
		m_pUIPassiveSkillIcon[i] = static_cast<CUI_DynamicTexture*>(Find_PartObject({ strSkillPartTag }));
	}

	CUI_Description::DESC DescDesc{};
	{
		DescDesc.pOwner = m_pOwner;

		DescDesc.szBoardFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Description/UI_DescBoard.dat";
		DescDesc.szBorderFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Description/UI_DescBorder.dat";
		DescDesc.szLogoFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Description/UI_DescLogo.dat";
		DescDesc.szIconFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Description/UI_DescIcon.dat";
		DescDesc.szLineFileName = L"../Bin/DataFiles/UI_Data/Menu/Status/Description/UI_DescLine.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Description", LEVEL_STATIC, L"Prototype_GameObject_UI_Description", &DescDesc)))
		return E_FAIL;

	m_pUIDesc = static_cast<CUI_Description*>(Find_PartObject({ L"Part_UI_Description" }));

	return S_OK;
}

void CUI_Status::Setting_ShellData()
{
	if (!*m_ppShellStat)
	{
		m_pUIShellDesc->Set_IsActive(false);

		m_pUIShellButton->Set_HoverCallback([this]()->void
			{
				m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);

				m_pUIDesc->Show(false, true);

				m_pUIDesc->Set_Name(L"등딱zl없음");
				m_pUIDesc->Set_Description(L"벌거벗고 있으면 위험하니까, 등딱지를 찾으세요!");
			});
	}
	else
	{
		m_pUIShellDesc->Set_IsActive(true);

		_int iTextureNum = (_int)(*m_ppShellStat)->eShell;
		FShellDescData tData = CFileData_Locator<FShellDescData>::Get_FileData()->Get_Data(0, iTextureNum);

		m_pUIShellButton->Set_HoverCallback([this, tData, iTextureNum]()->void
			{
				m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
				m_pUIDesc->Show(true);
				m_pUIDesc->Change_Texture(L"Prototype_Component_Texture_Shell", iTextureNum);
				m_pUIDesc->Set_Name(tData.szShellName);
				m_pUIDesc->Set_Description(tData.szSkillDescription);
				m_pUIDesc->Change_IconShaderPass((_uint)SHADER_PASS_UI::BLEND);
			});
	}
}

void CUI_Status::Setting_ActiveSkillData()
{
	vector<FActiveSkill> vecSkills = CSkill_DataBase::Get().Get_ActiveSkills();

	for (_int i = 0; i < vecSkills.size(); i++)
	{
		if (vecSkills[i].bUnLocked)
		{
			if (find(m_vecUnlockedSkill.begin(), m_vecUnlockedSkill.end(), vecSkills[i].eActiveSkill) == m_vecUnlockedSkill.end())
			{
				m_vecUnlockedSkill.push_back(vecSkills[i].eActiveSkill);
			}
		}
	}


	for (_int i = 0; i < 3; i++)
	{
		if (i < m_vecUnlockedSkill.size())
		{
			m_pUISkillIcon[i]->Change_TextureNum((_uint)m_vecUnlockedSkill[i]);
			m_pUISkillIcon[i]->Set_IsActive(true);
			m_pUISkillButton[i]->Set_IsActive(true);


			if (m_vecUnlockedSkill[i] == m_pPlayerStat->ePrimaryActiveSkill)
			{
				m_pUISkillBorder[i]->Set_IsActive(true);
				m_pUISkillBorder[i]->Change_SrcColor({ 0.f, 0.f, 0.f, 1.f });
				m_pUISkillButton[i]->Set_Callback([this, i]()->void
					{
						m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
						CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").front());
						pPlayer->Change_PrimaryActiveSkill(ACTIVESKILL::ACTIVESKILL_END);
						m_pUISkillBorder[i]->Set_IsActive(false);
					});
			}
			else if (m_vecUnlockedSkill[i] == m_pPlayerStat->eSecondaryActiveSkill)
			{
				m_pUISkillBorder[i]->Set_IsActive(true);
				m_pUISkillBorder[i]->Change_SrcColor({ 0.5f, 0.5f, 0.5f, 1.f });
				m_pUISkillButton[i]->Set_Callback([this, i]()->void
					{
						m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
						CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").front());
						pPlayer->Change_SecondaryActiveSkill(ACTIVESKILL::ACTIVESKILL_END);
						m_pUISkillBorder[i]->Set_IsActive(false);
					});
			}
			else
			{
				m_pUISkillButton[i]->Set_Callback([this, vecSkills, i]()->void
					{
						m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
						if (m_pPlayerStat->ePrimaryActiveSkill == ACTIVESKILL::ACTIVESKILL_END)
						{
							CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").front());
							pPlayer->Change_PrimaryActiveSkill(m_vecUnlockedSkill[i]);
							m_pUISkillBorder[i]->Change_SrcColor({ 0.f, 0.f, 0.f, 1.f });
							m_pUISkillBorder[i]->Set_IsActive(true);
						}
						else if (m_pPlayerStat->eSecondaryActiveSkill == ACTIVESKILL::ACTIVESKILL_END)
						{
							CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").front());
							pPlayer->Change_SecondaryActiveSkill(m_vecUnlockedSkill[i]);
							m_pUISkillBorder[i]->Change_SrcColor({ 0.5f, 0.5f, 0.5f, 1.f });
							m_pUISkillBorder[i]->Set_IsActive(true);
						}
					});
			}

			m_pUISkillButton[i]->Set_HoverCallback([this, i]()->void
				{
					m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
					FSkillData SkillData = CFileData_Locator<FSkillData>::Get_FileData()->Get_Data(0, (_int)m_vecUnlockedSkill[i]);
					m_pUIDesc->Show(true);
					m_pUIDesc->Change_Texture(m_strSkillIconTextureTag, (_int)m_vecUnlockedSkill[i]);
					m_pUIDesc->Set_Name(SkillData.szSkillName);
					m_pUIDesc->Set_Description(SkillData.szSkillDescription);
					m_pUIDesc->Change_IconShaderPass((_uint)SHADER_PASS_UI::SKILL);
				});
		}

		switch (m_pUISkillIcon[i]->Get_TextureNum())
		{
		case 0:
			m_pUISkillIcon[i]->Change_SrcColor({ 0.8f, 0.f, 1.f, 1.f });
			break;
		case 1:
			m_pUISkillIcon[i]->Change_SrcColor({ 0.f, 0.f, 1.f, 1.f });
			break;
		case 2:
			break;
		}
	}
}

void CUI_Status::Setting_PassiveSkillData()
{
	vector<FActionSkill> vecSkills = CSkill_DataBase::Get().Get_ActionSkills();

	for (_int i = 0; i < vecSkills.size(); i++)
	{
		if (vecSkills[i].bUnLocked)
		{
			if (find(m_vecUnlockedPassiveSkill.begin(), m_vecUnlockedPassiveSkill.end(), vecSkills[i].eActionSkill) == m_vecUnlockedPassiveSkill.end())
			{
				m_vecUnlockedPassiveSkill.push_back(vecSkills[i].eActionSkill);
			}
		}
	}


	for (_int i = 0; i < 4; i++)
	{
		if (i < m_vecUnlockedPassiveSkill.size())
		{
			m_pUIPassiveSkillIcon[i]->Change_TextureNum((_uint)m_vecUnlockedPassiveSkill[i]);
			m_pUIPassiveSkillIcon[i]->Set_IsActive(true);
			m_pUIPassiveSkillButton[i]->Set_IsActive(true);
			m_pUIPassiveSkillButton[i]->Set_HoverCallback([this, i]()->void
				{
					m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
					FSkillData SkillData = CFileData_Locator<FSkillData>::Get_FileData()->Get_Data(1, (_int)m_vecUnlockedPassiveSkill[i]);
					m_pUIDesc->Show(true);
					m_pUIDesc->Change_Texture(m_strPassvieSkillIconTextureTag, (_int)m_vecUnlockedPassiveSkill[i]);
					m_pUIDesc->Set_Name(SkillData.szSkillName);
					m_pUIDesc->Set_Description(SkillData.szSkillDescription);
					m_pUIDesc->Change_IconShaderPass((_uint)SHADER_PASS_UI::BLEND);
				});
		}
	}
}

CUI_Status* CUI_Status::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Status* pInstance = new CUI_Status(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Status::Clone(void* pArg)
{
	CUI_Status* pInstance = new CUI_Status(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Status::Free()
{
    CUIObject::Free();
}

