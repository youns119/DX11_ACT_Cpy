#include "pch.h"
#include "UI_PlayerLeft.h"

#include "GameInstance.h"
#include "Player.h"
#include "FileData_Locator.h"
#include "FData.h"

#include "UI_Shell.h"
#include "UI_Passenger.h"
#include "UI_Umami.h"
#include "UI_UmamiEffect.h"
#include "UI_Texture.h"
#include "UI_Value.h"
#include "UI_Stretch.h"
#include "UI_StretchBar.h"
#include "UI_StretchBarEffect.h"

CUI_PlayerLeft::CUI_PlayerLeft(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_PlayerLeft::CUI_PlayerLeft(const CUI_PlayerLeft& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_PlayerLeft::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_PlayerLeft::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	pDesc->UIObjectDesc.fSizeX = g_iWindowSizeX;
	pDesc->UIObjectDesc.fSizeY = g_iWindowSizeY;
	pDesc->UIObjectDesc.fX = g_iWindowSizeX * 0.5f;
	pDesc->UIObjectDesc.fY = g_iWindowSizeY * 0.5f;
	pDesc->UIObjectDesc.fZ = 0.01f;

	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	m_pPlayerStat = pDesc->pPlayerStat;
	m_ppShellStat = pDesc->ppShellStat;

	if (FAILED(CUIObject::Initialize(&pDesc->UIObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	return S_OK;
}

void CUI_PlayerLeft::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_PlayerLeft::Update(_float fTimeDelta)
{
	m_pUIHpBar->Move(fTimeDelta);
	m_pUIHpBorder->Move(fTimeDelta);
	m_pUIHpEffectBar->Move(fTimeDelta);

	m_pUIShellBorder->ScalingY(fTimeDelta * 5.f);
	m_pUIShellBar->ScalingY(fTimeDelta * 5.f);

	for (_int i = 0; i < (_int)m_pPlayerStat->fMaxSkillPoint; i++)
	{
		m_pUIUmami[i]->Move(fTimeDelta);
		m_pUIUmamiEffect[i]->Move(fTimeDelta);
	}

	// µî²®Áú Âø¿ë
	if (m_bHasShell != m_pPlayerStat->bHasShell)
	{
		m_bHasShell = m_pPlayerStat->bHasShell;

		if (m_bHasShell)
		{
			m_pUIHpBar->Set_Move({ 1,0 }, 15.f, 60.f);
			m_pUIHpBorder->Set_Move({ 1,0 }, 15.f, 60.f);
			m_pUIHpEffectBar->Set_Move({ 1,0 }, 15.f, 60.f);

			m_pUIShellBorder->Start_ScalingY(0.f, 1.f);
			m_pUIShellBar->Start_ScalingY(0.f, 1.f);

			for (_int i = 0; i < (_int)m_pPlayerStat->fMaxSkillPoint; i++)
			{
				m_pUIUmami[i]->Set_Move({ 1, -0.3f }, 15.f, 80.f);
				m_pUIUmamiEffect[i]->Set_Move({ 1,-0.3f }, 15.f, 80.f);
			}
		}
		else
		{
			m_pUIHpBar->Set_Move({-1,0}, 15.f, 60.f);
			m_pUIHpBorder->Set_Move({ -1,0 }, 15.f, 60.f);
			m_pUIHpEffectBar->Set_Move({ -1,0 }, 15.f, 60.f);

			m_pUIShellBorder->Start_ScalingY(1.f, 0.f);
			m_pUIShellBar->Start_ScalingY(1.f, 0.f);

			for (_int i = 0; i < (_int)m_pPlayerStat->fMaxSkillPoint; i++)
			{
				m_pUIUmami[i]->Set_Move({ -1, 0.3f }, 15.f, 80.f);
				m_pUIUmamiEffect[i]->Set_Move({ -1,0.3f }, 15.f, 80.f);
			}

			m_pLastShellStat = nullptr;
		}
	}

	if (m_bHasShell)
	{
		// µî²®Áú ±³Ã¼
		if (m_pLastShellStat != *m_ppShellStat)
		{
			m_pLastShellStat = *m_ppShellStat;

			m_pUIShell->Change_TextureNum((_uint)m_pLastShellStat->eShell);

			FShellDescData tData = CFileData_Locator<FShellDescData>::Get_FileData()->Get_Data(0, (_int)m_pLastShellStat->eShell);

			m_pUIShell->Set_Skill(tData.szSkillName);
			m_pUIShell->Set_ShellDef((*m_ppShellStat)->fShellDefense);
			m_pUIShell->Show();

			m_pUIShellBorder->Set_BarValue(&(*m_ppShellStat)->fShellCurrHP, &(*m_ppShellStat)->fShellMaxHP);
			m_pUIShellBorder->Set_IsActive(true);

			m_pUIShellBar->Set_BarValue(&(*m_ppShellStat)->fShellCurrHP, &(*m_ppShellStat)->fShellMaxHP);
			m_pUIShellBar->Set_IsActive(true);

			m_pUIShellEffectBar->Set_BarValue(&(*m_ppShellStat)->fShellCurrHP, &(*m_ppShellStat)->fShellMaxHP);
			m_pUIShellEffectBar->Set_IsActive(true);
		}
	}
	else
	{
		m_pUIShellBorder->Set_IsActive(false);
		m_pUIShellBar->Set_IsActive(false);
		m_pUIShellEffectBar->Set_IsActive(false);
		m_pUIShell->Set_IsActive(false);
	}

	super::Update(fTimeDelta);
}

void CUI_PlayerLeft::Late_Update(_float fTimeDelta)
{
	//if (!m_pPlayerStat->bTalking)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
		super::Late_Update(fTimeDelta);
	}
}

HRESULT CUI_PlayerLeft::Render()
{
    return S_OK;
}

HRESULT CUI_PlayerLeft::Ready_PartObject()
{
	/* Shell */
	CUI_Shell ::DESC ShellDesc{};
	{
		ShellDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerLeft/UI_Shell.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Shell", LEVEL_STATIC, L"Prototype_GameObject_UI_Shell", &ShellDesc)))
		return E_FAIL;

	m_pUIShell = static_cast<CUI_Shell*>(Find_PartObject({ L"Part_UI_Shell" }));

	/* HealthBorder */
	CUI_Stretch::DESC StretchDesc{};
	{
		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerLeft/UI_HealthBorder.dat";

		StretchDesc.fChangeRatio = 1.f;
		StretchDesc.fStartFactor = 9.f;
		StretchDesc.pCur = &m_pPlayerStat->fCurrHP;
		StretchDesc.pMax = &m_pPlayerStat->fMaxHP;
	}
	if (FAILED(Add_PartObject(L"Part_UI_HealthBorder", LEVEL_STATIC, L"Prototype_GameObject_UI_Stretch", &StretchDesc)))
		return E_FAIL;

	m_pUIHpBorder = static_cast<CUI_Stretch*>(Find_PartObject({ L"Part_UI_HealthBorder" }));

	/* HealthBar */
	CUI_StretchBar::DESC StretchBarDesc{};
	{
		StretchBarDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerLeft/UI_HealthBar.dat";

		StretchBarDesc.fChangeRatio = 2.f;
		StretchBarDesc.fStartFactor = 17.f;

		StretchBarDesc.pCur = &m_pPlayerStat->fCurrHP;
		StretchBarDesc.pMax = &m_pPlayerStat->fMaxHP;
	}
	if (FAILED(Add_PartObject(L"Part_UI_HealthBar", LEVEL_STATIC, L"Prototype_GameObject_UI_StretchBar", &StretchBarDesc)))
		return E_FAIL;

	m_pUIHpBar = static_cast<CUI_StretchBar*>(Find_PartObject({ L"Part_UI_HealthBar" }));

	/* HealthEffectBar */
	CUI_StretchBarEffect::DESC EffectBarDesc{};
	{
		EffectBarDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerLeft/UI_HealthBar_Effect.dat";

		EffectBarDesc.fChangeRatio = 2.f;
		EffectBarDesc.fStartFactor = 17.f;

		EffectBarDesc.pCur = &m_pPlayerStat->fCurrHP;
		EffectBarDesc.pMax = &m_pPlayerStat->fMaxHP;
	}
	if (FAILED(Add_PartObject(L"Part_UI_HealthEffectBar", LEVEL_STATIC, L"Prototype_GameObject_UI_StretchBarEffect", &EffectBarDesc)))
		return E_FAIL;

	m_pUIHpEffectBar = static_cast<CUI_StretchBarEffect*>(Find_PartObject({ L"Part_UI_HealthEffectBar" }));

	/* ShellBorder */
	{
		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerLeft/UI_ArmorBorder.dat";

		StretchDesc.fChangeRatio = 1.f;
		StretchDesc.fStartFactor = 13.5f;
		StretchDesc.fLengthStandard = 50.f;
		StretchDesc.fLengthDelta = 5.f;
	}
	if (FAILED(Add_PartObject(L"Part_UI_ShellBorder", LEVEL_STATIC, L"Prototype_GameObject_UI_Stretch", &StretchDesc)))
		return E_FAIL;

	m_pUIShellBorder = static_cast<CUI_Stretch*>(Find_PartObject({ L"Part_UI_ShellBorder" }));

	/* ShellBar */
	{
		StretchBarDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerLeft/UI_ArmorBar.dat";

		StretchBarDesc.fChangeRatio = 2.f;
		StretchBarDesc.fStartFactor = 26.f;
		StretchBarDesc.fLengthStandard = 50.f;
		StretchBarDesc.fLengthDelta = 5.f;
	}
	if (FAILED(Add_PartObject(L"Part_UI_ShellBar", LEVEL_STATIC, L"Prototype_GameObject_UI_StretchBar", &StretchBarDesc)))
		return E_FAIL;

	m_pUIShellBar = static_cast<CUI_StretchBar*>(Find_PartObject({ L"Part_UI_ShellBar" }));

	/* ShellEffectBar */
	{
		EffectBarDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerLeft/UI_ArmorBar_Effect.dat";

		EffectBarDesc.fChangeRatio = 2.f;
		EffectBarDesc.fStartFactor = 26.f;
		EffectBarDesc.fLengthStandard = 50.f;
		EffectBarDesc.fLengthDelta = 5.f;
	}
	if (FAILED(Add_PartObject(L"Part_UI_ShellEffectBar", LEVEL_STATIC, L"Prototype_GameObject_UI_StretchBarEffect", &EffectBarDesc)))
		return E_FAIL;

	m_pUIShellEffectBar = static_cast<CUI_StretchBarEffect*>(Find_PartObject({ L"Part_UI_ShellEffectBar" }));

	_float2 vStart1 = { 110.f, 660.f };
	_float2 vStart2 = { 135.f, 657.f };

	_float fOffsetX = { 53.f };

	for (_int i = 0; i < (_int)m_pPlayerStat->fMaxSkillPoint; i++)
	{
		wstring strUmamiTag = L"Part_UI_Umami" + to_wstring(i);
		wstring strEffectTag = L"Part_UI_UmamiEffect" + to_wstring(i);

		_float2 vLocate = {};

		if (i % 2 == 0)
			vLocate = { vStart1.x + fOffsetX * (i / 2), vStart1.y };
		else
			vLocate = { vStart2.x + fOffsetX * (i / 2), vStart2.y };
		
		CUI_Umami::DESC UmamiDesc{};
		{
			UmamiDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerLeft/UI_Umami.dat";

			UmamiDesc.vLocate = vLocate;
			UmamiDesc.iIndex = i;
			UmamiDesc.pUmamiPoint = &m_pPlayerStat->fCurrSkillPoint;
		}
		if (FAILED(Add_PartObject(strUmamiTag, LEVEL_STATIC, L"Prototype_GameObject_UI_Umami", &UmamiDesc)))
			return E_FAIL;

		m_pUIUmami[i] = static_cast<CUI_Umami*>(Find_PartObject({ strUmamiTag }));

		CUI_UmamiEffect::DESC UmamiEffectDesc{};
		{
			UmamiEffectDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerLeft/UI_UmamiEffect.dat";
		
			UmamiEffectDesc.vLocate = vLocate;
			UmamiEffectDesc.iIndex = i;
			UmamiEffectDesc.pUmamiPoint = &m_pPlayerStat->fCurrSkillPoint;
		}
		if (FAILED(Add_PartObject(strEffectTag, LEVEL_STATIC, L"Prototype_GameObject_UI_UmamiEffect", &UmamiEffectDesc)))
			return E_FAIL;

		m_pUIUmamiEffect[i] = static_cast<CUI_UmamiEffect*>(Find_PartObject({ strEffectTag }));
	}
	
	return S_OK;
}

CUI_PlayerLeft* CUI_PlayerLeft::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_PlayerLeft* pInstance = new CUI_PlayerLeft(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_PlayerLeft");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_PlayerLeft::Clone(void* pArg)
{
	CUI_PlayerLeft* pInstance = new CUI_PlayerLeft(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_PlayerLeft");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PlayerLeft::Free()
{
    CUIObject::Free();
}

