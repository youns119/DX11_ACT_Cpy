#include "pch.h"
#include "UI_Stat.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "FPlayerStat.h"
#include "Player.h"

#include "UI_DefaultStretch.h"
#include "UI_Value.h"
#include "UI_Shake.h"
#include "UI_Texture.h"
#include "UI_Button.h"
#include "UI_StatButton.h"
#include "UI_Description.h"

#include <sstream>
#include <locale>

CUI_Stat::CUI_Stat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Stat::CUI_Stat(const CUI_Stat& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_Stat::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Stat::Initialize(void* pArg)
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

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CUI_Stat::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_Stat::Update(_float fTimeDelta)
{
	if (m_pUIManager->Get_CopyTexture_IsActive())
		m_eRenderGroup = CRenderer::RENDERGROUP::UI_ANIM;
	else
		m_eRenderGroup = CRenderer::RENDERGROUP::UI;

	if (!m_isActive)
	{
		m_iMicroplastic = m_pUIManager->Get_Micro_Ref();

		m_pStatButton[0]->Set_Value(&m_pUIManager->Get_PlayerStat()->iVitality);
		m_pStatButton[1]->Set_Value(&m_pUIManager->Get_PlayerStat()->iResistance);
		m_pStatButton[2]->Set_Value(&m_pUIManager->Get_PlayerStat()->iAttackPower);
		m_pStatButton[3]->Set_Value(&m_pUIManager->Get_PlayerStat()->iMSG);
		/*for (_int i = 0; i < 4; i++)
		{
			m_pStatButton[i]->Reset_Value();
		}*/
		return;
	}

	if (m_pGameInstance->Key_Down(KEY::ESCAPE))
	{
		m_pUIManager->Hide_StatUI([this]()->void
			{
				m_pUIManager->Show_MoonSnailUI();
			});
	}

	_int iValueSum = {};
	for (_int i = 0; i < 4; i++)
	{
		iValueSum += m_pStatButton[i]->Get_Value();
	}
	m_iLevel = iValueSum;
	m_iLevelUpCost = m_iLevel * 15;
	m_iLevelDownCost = (m_iLevel - 1) * 15;

	super::Update(fTimeDelta);
}

void CUI_Stat::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);

		super::Late_Update(fTimeDelta);
	}
}

HRESULT CUI_Stat::Render()
{
	_wstring strLevel = {};
	_wstring strMicro = {};
	_wstring strCost = Format_With_Comma(m_iLevelUpCost);
	if (m_iMicroplastic != m_pUIManager->Get_Micro_Ref())
	{
		strLevel = to_wstring(m_pUIManager->Get_PlayerStat()->iLevel) + L"   ->   " + to_wstring(m_iLevel);
		strMicro = Format_With_Comma(m_pUIManager->Get_Micro_Ref()) +	L"   ->   " + Format_With_Comma(m_iMicroplastic);
	}
	else
	{
		strLevel = to_wstring(m_iLevel);
		strMicro = Format_With_Comma(m_iMicroplastic);
	}

	m_pGameInstance->Render_Font(L"Font_Main_40", L"레벨 업", { 480.f, 100.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 0.7f, SORTING::LEFT);

	m_pGameInstance->Render_Font(L"Font_Cafe_15", L"레벨", { 480.f, 140.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.0f, SORTING::LEFT);
	m_pGameInstance->Render_Font(L"Font_Cafe_15", strLevel.c_str(), { 800.f, 140.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.0f, SORTING::RIGHT);

	m_pGameInstance->Render_Font(L"Font_Cafe_15", L"미세 플라스틱", { 480.f, 190.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.0f, SORTING::LEFT);
	m_pGameInstance->Render_Font(L"Font_Cafe_15", strMicro.c_str(), { 800.f, 190.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.0f, SORTING::RIGHT);

	m_pGameInstance->Render_Font(L"Font_Cafe_15", L"다음 레벨 비용", { 480.f, 220.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.0f, SORTING::LEFT);
	m_pGameInstance->Render_Font(L"Font_Cafe_15", strCost.c_str(), { 800.f, 220.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.0f, SORTING::RIGHT);

    return S_OK;
}

HRESULT CUI_Stat::Ready_PartObject()
{
	CUI_DefaultStretch::DESC StretchDesc{};
	{
		StretchDesc.UIDesc.pOwner = this;

		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Stat_TopBoard.dat";
		StretchDesc.fStretchFactor = 20.f;
		StretchDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_TopBoard", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	{
		StretchDesc.UIDesc.pOwner = this;

		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Stat_TopBorder.dat";
		StretchDesc.fStretchFactor = 24.f;
		StretchDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_TopBorder", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	{
		StretchDesc.UIDesc.pOwner = this;

		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Stat_TopLine.dat";
		StretchDesc.fStretchFactor = 260.f;
		StretchDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_TopLine", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	{
		StretchDesc.UIDesc.pOwner = this;

		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Stat_Board1.dat";
		StretchDesc.fStretchFactor = 30.f;
		StretchDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Board1", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	{
		StretchDesc.UIDesc.pOwner = this;

		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Stat_Board2.dat";
		StretchDesc.fStretchFactor = 30.f;
		StretchDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Board2", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	{
		StretchDesc.UIDesc.pOwner = this;

		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Stat_Border.dat";
		StretchDesc.fStretchFactor = 4.f;
		StretchDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Border", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	CUI_StatButton::DESC StatButtonDesc{};
	{
		StatButtonDesc.pMicroplastic = &m_iMicroplastic;
		StatButtonDesc.pLevelUpCost = &m_iLevelUpCost;
		StatButtonDesc.pLevelDownCost = &m_iLevelDownCost;

		StatButtonDesc.fStretchFactor = 230.f;

		StatButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Stat_Button1.dat";
		StatButtonDesc.strIconFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Button_Icon1.dat";
		StatButtonDesc.pValue = &m_pUIManager->Get_PlayerStat()->iVitality;

		lstrcpy(StatButtonDesc.szIconText, L"생명력");

		StatButtonDesc.HoverCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
				m_pUIDesc->Change_Texture(m_strStatIconTextureTag, 0);
				m_pUIDesc->Set_Name(L"생명력");
				m_pUIDesc->Set_Description(L"내 체력의 양을 증가시킵니다.");

				m_pUIDesc->Show(true);
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_StatButton1", LEVEL_STATIC, L"Prototype_GameObject_UI_StatButton", &StatButtonDesc)))
		return E_FAIL;

	m_pStatButton[0] = static_cast<CUI_StatButton*>(Find_PartObject({ L"Part_UI_StatButton1" }));

	{
		StatButtonDesc.pMicroplastic = &m_iMicroplastic;
		StatButtonDesc.pLevelUpCost = &m_iLevelUpCost;

		StatButtonDesc.fStretchFactor = 230.f;

		StatButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Stat_Button2.dat";
		StatButtonDesc.strIconFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Button_Icon2.dat";
		StatButtonDesc.pValue = &m_pUIManager->Get_PlayerStat()->iResistance;

		lstrcpy(StatButtonDesc.szIconText, L"저항력");

		StatButtonDesc.HoverCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
				m_pUIDesc->Change_Texture(m_strStatIconTextureTag, 1);
				m_pUIDesc->Set_Name(L"저항력");
				m_pUIDesc->Set_Description(L"등딱지가 받는 대미지를 감소시키고, 상태 효과 저항력을 증가시킵니다.");

				m_pUIDesc->Show(true);
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_StatButton2", LEVEL_STATIC, L"Prototype_GameObject_UI_StatButton", &StatButtonDesc)))
		return E_FAIL;

	m_pStatButton[1] = static_cast<CUI_StatButton*>(Find_PartObject({ L"Part_UI_StatButton2" }));

	{
		StatButtonDesc.pMicroplastic = &m_iMicroplastic;
		StatButtonDesc.pLevelUpCost = &m_iLevelUpCost;

		StatButtonDesc.fStretchFactor = 230.f;

		StatButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Stat_Button3.dat";
		StatButtonDesc.strIconFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Button_Icon3.dat";
		StatButtonDesc.pValue = &m_pUIManager->Get_PlayerStat()->iAttackPower;

		lstrcpy(StatButtonDesc.szIconText, L"공격력");

		StatButtonDesc.HoverCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
				m_pUIDesc->Change_Texture(m_strStatIconTextureTag, 2);
				m_pUIDesc->Set_Name(L"공격력");
				m_pUIDesc->Set_Description(L"물리적인 공격의 대미지를 강화합니다.");

				m_pUIDesc->Show(true);
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_StatButton3", LEVEL_STATIC, L"Prototype_GameObject_UI_StatButton", &StatButtonDesc)))
		return E_FAIL;

	m_pStatButton[2] = static_cast<CUI_StatButton*>(Find_PartObject({ L"Part_UI_StatButton3" }));

	{
		StatButtonDesc.pMicroplastic = &m_iMicroplastic;
		StatButtonDesc.pLevelUpCost = &m_iLevelUpCost;

		StatButtonDesc.fStretchFactor = 230.f;

		StatButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Stat_Button4.dat";
		StatButtonDesc.strIconFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Button_Icon4.dat";
		StatButtonDesc.pValue = &m_pUIManager->Get_PlayerStat()->iMSG;

		lstrcpy(StatButtonDesc.szIconText, L"MSG");
	
		StatButtonDesc.HoverCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
				m_pUIDesc->Change_Texture(m_strStatIconTextureTag, 3);
				m_pUIDesc->Set_Name(L"MSG");
				m_pUIDesc->Set_Description(L"감칠맛 기반 공격의 대미지를 강화합니다.");

				m_pUIDesc->Show(true);
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_StatButton4", LEVEL_STATIC, L"Prototype_GameObject_UI_StatButton", &StatButtonDesc)))
		return E_FAIL;

	m_pStatButton[3] = static_cast<CUI_StatButton*>(Find_PartObject({ L"Part_UI_StatButton4" }));

	CUI_Shake::DESC ShakeDesc{};
	{
		ShakeDesc.UIDesc.pOwner = this;
		ShakeDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Stat_Deco1.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Deco1", LEVEL_STATIC, L"Prototype_GameObject_UI_Shake", &ShakeDesc)))
		return E_FAIL;

	{
		ShakeDesc.UIDesc.pOwner = this;
		ShakeDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Stat_Deco2.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Deco2", LEVEL_STATIC, L"Prototype_GameObject_UI_Shake", &ShakeDesc)))
		return E_FAIL;

	/* ESC_Button */
	CUI_Button::DESC ButtonDesc{};
	{
		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_ESC_Button.dat";
		lstrcpy(ButtonDesc.szText, TEXT("뒤로"));
		ButtonDesc.ClickCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
				m_pUIManager->Hide_StatUI([this]()->void
					{
						m_pUIManager->Show_MoonSnailUI();
					});
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_ESC_Button", LEVEL_STATIC, L"Prototype_GameObject_UI_Button", &ButtonDesc)))
		return E_FAIL;

	{
		ButtonDesc.UIDesc.eSorting = SORTING::MIDDLE;
		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_OK_Button.dat";
		lstrcpy(ButtonDesc.szText, TEXT("확정"));
		ButtonDesc.ClickCallback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);

				m_pUIManager->Set_Micro(m_iMicroplastic);
				CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").front());
				
				pPlayer->Get_PlayerStat()->iVitality = m_pStatButton[0]->Get_Value();
				pPlayer->Get_PlayerStat()->iResistance = m_pStatButton[1]->Get_Value();
				pPlayer->Get_PlayerStat()->iAttackPower = m_pStatButton[2]->Get_Value();
				pPlayer->Get_PlayerStat()->iMSG = m_pStatButton[3]->Get_Value();
				pPlayer->Get_PlayerStat()->iLevel = m_iLevel;

				pPlayer->Calculate_Stat();
			};
	}
	if (FAILED(Add_PartObject(L"Part_UI_OK_Button", LEVEL_STATIC, L"Prototype_GameObject_UI_Button", &ButtonDesc)))
		return E_FAIL;

	/*{
		StretchDesc.UIDesc.pOwner = this;

		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Stat_SmallBoard.dat";
		StretchDesc.fStretchFactor = 20.f;
		StretchDesc.bMiddle = true;
		StretchDesc.UIDesc.bStretchY = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_SmallBoard", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;*/

	/*{
		StretchDesc.UIDesc.pOwner = this;

		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Stat_SmallBorder.dat";
		StretchDesc.fStretchFactor = 15.f;
		StretchDesc.bMiddle = true;
		StretchDesc.UIDesc.bStretchY = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_SmallBorder", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;*/

	CUI_Description::DESC DescDesc{};
	{
		DescDesc.pOwner = this;

		DescDesc.szBoardFileName	= L"../Bin/DataFiles/UI_Data/Stat/Description/UI_DescBoard.dat";
		DescDesc.szBorderFileName	= L"../Bin/DataFiles/UI_Data/Stat/Description/UI_DescBorder.dat";
		DescDesc.szLogoFileName		= L"../Bin/DataFiles/UI_Data/Stat/Description/UI_DescLogo.dat";
		DescDesc.szIconFileName		= L"../Bin/DataFiles/UI_Data/Stat/Description/UI_DescIcon.dat";
		DescDesc.szLineFileName		= L"../Bin/DataFiles/UI_Data/Stat/Description/UI_DescLine.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Description", LEVEL_STATIC, L"Prototype_GameObject_UI_Description", &DescDesc)))
		return E_FAIL;

	m_pUIDesc = static_cast<CUI_Description*>(Find_PartObject({ L"Part_UI_Description" }));

	return S_OK;
}

_wstring CUI_Stat::Format_With_Comma(_float fValue)
{
	wstringstream ws;
	ws.imbue(locale("en_US.UTF-8"));  // 미국 영어 로케일
	ws << fValue;

	return ws.str();
}

CUI_Stat* CUI_Stat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Stat* pInstance = new CUI_Stat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Stat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Stat::Clone(void* pArg)
{
	CUI_Stat* pInstance = new CUI_Stat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Stat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Stat::Free()
{
    CUIObject::Free();
}

