#include "pch.h"
#include "UI_DynamicButton.h"

#include "GameInstance.h"

CUI_DynamicButton::CUI_DynamicButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_DynamicButton::CUI_DynamicButton(const CUI_DynamicButton& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_DynamicButton::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_DynamicButton::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	lstrcpy(m_szText, pDesc->szText);

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_vLocate = { pDesc->vLocate.x, pDesc->vLocate.y, m_vLocate.z };
	m_HoverCallback = pDesc->HoverCallback;
	m_Callback = pDesc->Callback;
	m_pTransformCom->Locate(m_vLocate.x - s_fViewportWidth * 0.5f, -m_vLocate.y + s_fViewportHeight * 0.5f, m_vLocate.z);

	return S_OK;
}

void CUI_DynamicButton::Priority_Update(_float fTimeDelta)
{
}

void CUI_DynamicButton::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	_float2 fMousePos = m_pGameInstance->Calulate_ScreenMousePos();

	_float2 vMin = { m_vLocate.x - m_vScale.x * 0.5f, m_vLocate.y - m_vScale.y * 0.5f };
	_float2 vMax = { m_vLocate.x + m_vScale.x * 0.5f, m_vLocate.y + m_vScale.y * 0.5f };

	m_bHover = (fMousePos.x >= vMin.x && fMousePos.x <= vMax.x && fMousePos.y >= vMin.y && fMousePos.y <= vMax.y);

	m_fFrame += fTimeDelta;

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

		if (m_pGameInstance->Mouse_Down(DIM_LB) && m_Callback)
			m_Callback();
	}
	else
		m_bHovered = false;

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
	default:
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
	}
}

void CUI_DynamicButton::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
		
	}
}

HRESULT CUI_DynamicButton::Render()
{
	if (!m_pTextureCom)
		return S_OK;

	_float4 vCurTextColor;

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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fStretchFactor", &m_fStretchFactor, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	_float2 vTextPos = { m_vLocate.x + m_vTextOffset.x, m_vLocate.y + m_vTextOffset.y };
	vCurTextColor.w = 0.9f;
	m_pGameInstance->Render_Font(m_strFontTag, m_szText, vTextPos, XMLoadFloat4(&vCurTextColor), 0.f, { 0.f, 0.f }, m_fTextScale, m_eSorting, SpriteEffects_None, m_vLocate.z);

	return S_OK;
}

CUI_DynamicButton* CUI_DynamicButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_DynamicButton* pInstance = new CUI_DynamicButton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_DynamicButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_DynamicButton::Clone(void* pArg)
{
	CUI_DynamicButton* pInstance = new CUI_DynamicButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_DynamicButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_DynamicButton::Free()
{
    CUI::Free();
}
