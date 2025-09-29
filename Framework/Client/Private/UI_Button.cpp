#include "pch.h"
#include "UI_Button.h"

#include "GameInstance.h"

CUI_Button::CUI_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Button::CUI_Button(const CUI_Button& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Button::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Button::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	lstrcpy(m_szText, pDesc->szText);

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_HoverCallback = pDesc->HoverCallback;
	m_ClickCallback = pDesc->ClickCallback;

	return S_OK;
}

void CUI_Button::Priority_Update(_float fTimeDelta)
{
}

void CUI_Button::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	_float2 fMousePos = m_pGameInstance->Calulate_ScreenMousePos();

	_float2 vMin = { m_vLocate.x - m_vScale.x * 0.5f, m_vLocate.y - m_vScale.y * 0.5f };
	_float2 vMax = { m_vLocate.x + m_vScale.x * 0.5f, m_vLocate.y + m_vScale.y * 0.5f };

	m_bHover = (fMousePos.x >= vMin.x && fMousePos.x <= vMax.x && fMousePos.y >= vMin.y && fMousePos.y <= vMax.y);

	if (m_bHover)
	{
		if (m_HoverCallback)
		{
			if (!m_bHovered)
			{
				m_HoverCallback();
				m_bHovered = true;
			}
		}

		if (m_pGameInstance->Mouse_Down(DIM_LB) && m_ClickCallback)
		{
			m_ClickCallback();
		}
	}
	else
		m_bHovered = false;

	m_fFrame += fTimeDelta;

	switch (m_iShaderPass)
	{
	case (_uint)SHADER_PASS_UI::OPTION_BUTTON:
		if (m_fFrame >= m_fInterval)
		{
			if (m_tParams.fThreshold > 0.1f)
				m_tParams.fThreshold -= 0.1f;
			else
				m_tParams.fThreshold = 0.1f;

			m_tParams.vSpriteIndex.y++;

			if (m_tParams.vSpriteIndex.y >= m_tParams.vSpriteSize.y)
			{
				m_tParams.vSpriteIndex.y = 0;
				m_tParams.vSpriteIndex.x++;

				if (m_tParams.vSpriteIndex.x >= m_tParams.vSpriteSize.x)
					m_tParams.vSpriteIndex = {};
			}
			m_fFrame = 0.f;
		}
		break;
	case (_uint)SHADER_PASS_UI::MENU_BUTTON:
	case (_uint)SHADER_PASS_UI::BUTTON:
		if (m_fFrame >= m_fInterval)
		{
			m_tParams.vSpriteIndex.y++;

			if (m_tParams.vSpriteIndex.y >= m_tParams.vSpriteSize.y)
			{
				m_tParams.vSpriteIndex.y = 0;
				m_tParams.vSpriteIndex.x++;

				if (m_tParams.vSpriteIndex.x >= m_tParams.vSpriteSize.x)
					m_tParams.vSpriteIndex = {};
			}

			m_fFrame = 0.f;
		}
		break;
	default:
		break;
	}
}

void CUI_Button::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
	}
}

HRESULT CUI_Button::Render()
{
	if (!m_pTextureCom)
		return S_OK;

	_float4 vCurTextColor = {};

	if (m_bHover)
		vCurTextColor = m_vHoverTextColor;
	else
		vCurTextColor = m_vTextColor;

	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", s_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", s_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (m_pMaskTextureCom)
	{
		if (FAILED(m_pMaskTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_MaskTexture", 0)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UIParams", &m_tParams, sizeof(UI_PARAMS))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bHovered", &m_bHover, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bRightAngle", &m_bRightAngle, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bStretchY", &m_bStretchY, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	_float2 vTextPos = { m_vLocate.x + m_vTextOffset.x, m_vLocate.y + m_vTextOffset.y };

	vCurTextColor.w = 1.f;

	m_pGameInstance->Render_Font(m_strFontTag, m_szText, vTextPos, XMLoadFloat4(&vCurTextColor), 0.f, { 0.f, 0.f }, m_fTextScale, m_eSorting, SpriteEffects_None, m_vLocate.z);

	return S_OK;
}

CUI_Button* CUI_Button::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Button* pInstance = new CUI_Button(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Button");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Button::Clone(void* pArg)
{
	CUI_Button* pInstance = new CUI_Button(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Button");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Button::Free()
{
	CUI::Free();
}