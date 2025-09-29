#include "pch.h"
#include "UI_Text.h"

#include "GameInstance.h"
#include "UI_Manager.h"

CUI_Text::CUI_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Text::CUI_Text(const CUI_Text& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Text::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Text::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	return S_OK;
}

void CUI_Text::Priority_Update(_float fTimeDelta)
{
}

void CUI_Text::Update(_float fTimeDelta)
{
	if (m_pUIManager->Get_TextTexture_IsActive())
		m_eRenderGroup = CRenderer::RENDERGROUP::UI_ANIM;
	else
		m_isActive = false;
}

void CUI_Text::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
	}
}

HRESULT CUI_Text::Render()
{
	if (!m_pTextureCom)
		return S_OK;

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
		if (FAILED(m_pCoverTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_CoverTexture", m_iCoverTextureNum)))
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

	m_pGameInstance->Render_Font(m_strFontTag, m_szText, vTextPos, XMLoadFloat4(&m_vTextColor), 0.f, { 0.f, 0.f }, m_fTextScale, m_eSorting);

	return S_OK;
}

CUI_Text* CUI_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Text* pInstance = new CUI_Text(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Text");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Text::Clone(void* pArg)
{
	CUI_Text* pInstance = new CUI_Text(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Text");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Text::Free()
{
	CUI::Free();
}