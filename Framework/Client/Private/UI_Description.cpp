#include "pch.h"
#include "UI_Description.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "FileData_Locator.h"
#include "Item.h"
#include "UI_DefaultStretch.h"
#include "UI_Texture.h"
#include "UI_Line.h"

CUI_Description::CUI_Description(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Description::CUI_Description(const CUI_Description& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_Description::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Description::Initialize(void* pArg)
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
	m_pOwner = pDesc->pOwner;

	if (FAILED(CUIObject::Initialize(&pDesc->UIObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_PartObject(pDesc)))
		return E_FAIL;

	m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	Show(false);

	return S_OK;
}

void CUI_Description::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	super::Priority_Update(fTimeDelta);
}

void CUI_Description::Update(_float fTimeDelta)
{
	if (m_pUIIcon->Get_TextureTag() == L"Prototype_Component_Texture_Skill")
	{
		m_pUIIcon->Set_ShaderPassNum((_uint)SHADER_PASS_UI::SKILL);

		switch (m_pUIIcon->Get_TextureNum())
		{
		case 0:
			m_pUIIcon->Change_SrcColor({ 0.8f, 0.f, 1.f, 1.f });
			break;
		case 1:
			m_pUIIcon->Change_SrcColor({ 0.f, 0.f, 1.f, 1.f });
			break;
		case 2:
			break;
		}
	}
	else
		m_pUIIcon->Set_ShaderPassNum((_uint)SHADER_PASS_UI::BLEND);

	super::Update(fTimeDelta);
}

void CUI_Description::Late_Update(_float fTimeDelta)
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

HRESULT CUI_Description::Render()
{	
	_float3 vLineLocate = m_pUILine->Get_Locate();

	_float2 vTextStartPos = { vLineLocate.x - 20.f, vLineLocate.y + 20.f };
	_float	fOffsetY = 25.f;

	for (_int i = 0; i < m_vecText.size(); i++)
	{
		_float2 vTextPos = { vTextStartPos.x, vTextStartPos.y + fOffsetY * i };

		m_pGameInstance->Render_Font(L"Font_Cafe_15", m_vecText[i].c_str(), vTextPos);
	}
	return S_OK;
}

void CUI_Description::Show(_bool bShow)
{
	m_pUIIcon->Set_IsActive(bShow);
	m_pUILine->Set_IsActive(bShow);
}

void CUI_Description::Show(_bool bIconShow, _bool bLineShow)
{
	m_pUIIcon->Set_IsActive(bIconShow);
	m_pUILine->Set_IsActive(bLineShow);
}

void CUI_Description::Change_Texture(_wstring strTextureTag, _int iTextureNum)
{
	m_pUIIcon->Change_Texture(strTextureTag, iTextureNum);
}

void CUI_Description::Set_Name(const _tchar* szName)
{
	m_pUILine->Set_Text(szName);
}

void CUI_Description::Set_Description(const _tchar* szDescription)
{
	m_vecText.clear();

	_int len = (_int)wcslen(szDescription);
	_int index = 0;

	while (index < len)
	{
		size_t chunkSize = min(15, len - index);
		m_vecText.push_back(_wstring(szDescription + index, chunkSize));
		index += (_int)chunkSize;
	}
}

void CUI_Description::Change_IconShaderPass(_uint iPassNum)
{
	m_pUIIcon->Set_ShaderPassNum(iPassNum);
}

HRESULT CUI_Description::Ready_PartObject(DESC* pDesc)
{
	CUI_DefaultStretch::DESC StretchDesc{};
	{
		StretchDesc.UIDesc.pOwner = m_pOwner;

		StretchDesc.UIDesc.strFileName = pDesc->szBoardFileName;
		StretchDesc.fStretchFactor = 10.f;
		StretchDesc.bMiddle = true;
		StretchDesc.UIDesc.bRightAngle = false;
		StretchDesc.UIDesc.bStretchY = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_DescBoard", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	{
		StretchDesc.UIDesc.pOwner = m_pOwner;

		StretchDesc.UIDesc.strFileName = pDesc->szBorderFileName;
		StretchDesc.fStretchFactor = 7.f;
		StretchDesc.bMiddle = true;
		StretchDesc.UIDesc.bRightAngle = false;
		StretchDesc.UIDesc.bStretchY = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_DescBorder", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	CUI_Texture::DESC TextureDesc{};
	{
		TextureDesc.UIDesc.pOwner = m_pOwner;

		TextureDesc.UIDesc.strFileName = pDesc->szLogoFileName;
	}
	if (FAILED(Add_PartObject(L"Part_UI_DescLogo", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	{
		TextureDesc.UIDesc.pOwner = m_pOwner;

		TextureDesc.UIDesc.strFileName = pDesc->szIconFileName;
	}
	if (FAILED(Add_PartObject(L"Part_UI_DescIcon", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUIIcon = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_DescIcon" }));

	CUI_Line::DESC LineDesc{};
	{
		LineDesc.UIDesc.pOwner = m_pOwner;

		LineDesc.UIDesc.strFileName = pDesc->szLineFileName;
	}
	if (FAILED(Add_PartObject(L"Part_UI_DescLine", LEVEL_STATIC, L"Prototype_GameObject_UI_Line", &LineDesc)))
		return E_FAIL;

	m_pUILine = static_cast<CUI_Line*>(Find_PartObject({ L"Part_UI_DescLine" }));


	return S_OK;
}

CUI_Description* CUI_Description::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Description* pInstance = new CUI_Description(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Description");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Description::Clone(void* pArg)
{
	CUI_Description* pInstance = new CUI_Description(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Description");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Description::Free()
{
    CUIObject::Free();
}

