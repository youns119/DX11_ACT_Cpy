#include "pch.h"
#include "UI_DynamicTexture.h"

#include "GameInstance.h"

CUI_DynamicTexture::CUI_DynamicTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_DynamicTexture::CUI_DynamicTexture(const CUI_DynamicTexture& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_DynamicTexture::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_DynamicTexture::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_vLocate = { pDesc->vLocate.x, pDesc->vLocate.y, m_vLocate.z };
	m_pTransformCom->Locate(m_vLocate.x - s_fViewportWidth * 0.5f, -m_vLocate.y + s_fViewportHeight * 0.5f, m_vLocate.z);

	return S_OK;
}

void CUI_DynamicTexture::Priority_Update(_float fTimeDelta)
{
}

void CUI_DynamicTexture::Update(_float fTimeDelta)
{
}

void CUI_DynamicTexture::Late_Update(_float fTimeDelta)
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

HRESULT CUI_DynamicTexture::Render()
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

	if (FAILED(m_pTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_Texture", m_iTextureNum)))
		return E_FAIL;

	if (m_pMaskTextureCom)
	{
		if (FAILED(m_pMaskTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_MaskTexture", 0)))
			return E_FAIL;
	}

	_uint iShaderPass = m_iShaderPass;

	if (m_pCoverTextureCom)
	{
		if (FAILED(m_pCoverTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_CoverTexture", 0)))
			return E_FAIL;

		iShaderPass = (_uint)SHADER_PASS_UI::COVER;
	}
	else
	{
		if (FAILED(m_pTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_CoverTexture", 0)))
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

	if (FAILED(m_pShaderCom->Begin(iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	_float2 vTextPos = { m_vLocate.x + m_vTextOffset.x, m_vLocate.y + m_vTextOffset.y };

	m_pGameInstance->Render_Font(m_strFontTag, m_szText, vTextPos, XMLoadFloat4(&vCurTextColor), 0.f, { 0.f, 0.f }, m_fTextScale);

	return S_OK;
}

CUI_DynamicTexture* CUI_DynamicTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_DynamicTexture* pInstance = new CUI_DynamicTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_DynamicTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_DynamicTexture::Clone(void* pArg)
{
	CUI_DynamicTexture* pInstance = new CUI_DynamicTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_DynamicTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_DynamicTexture::Free()
{
    CUI::Free();
}
