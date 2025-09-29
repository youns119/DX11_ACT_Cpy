#include "pch.h"
#include "UI_Fade.h"

#include "GameInstance.h"

CUI_Fade::CUI_Fade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Fade::CUI_Fade(const CUI_Fade& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Fade::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Fade::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	return S_OK;
}

void CUI_Fade::Priority_Update(_float fTimeDelta)
{
}

void CUI_Fade::Update(_float fTimeDelta)
{
	if (m_bFadeIn)
	{
		m_fAlpha -= m_fFadeSpeed * fTimeDelta;

		if (m_fAlpha <= 0.0f)
		{
			m_fAlpha = 0.0f;
			m_bFadeInEnd = true;
			m_bFadeOutEnd = false;
		}
	}
	else if(m_bFadeOut)
	{
		m_fAlpha += m_fFadeSpeed * fTimeDelta;

		if (m_fAlpha >= 1.0f)
		{
			m_fAlpha = 1.0f;
			m_bFadeOutEnd = true;
			m_bFadeInEnd = false;
		}
	}
}

void CUI_Fade::Late_Update(_float fTimeDelta)
{
	if(m_fAlpha > 0.f)
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_Fade::Render()
{
	if (!m_pTextureCom)
		return S_OK;

	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", s_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", s_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UIParams", &m_tParams, sizeof(UI_PARAMS))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bHovered", &m_bHover, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bRightAngle", &m_bRightAngle, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bStretchY", &m_bStretchY, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_UI::FADE)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_Fade::FadeIn()
{
	if(m_bFadeIn == false)
	{
		m_bFadeIn = true; 
		m_bFadeOut = false;
		m_fAlpha = 1.f;
		m_bFadeInEnd = false;
	}
}

void CUI_Fade::FadeOut()
{
	if (m_bFadeOut == false)
	{
		m_bFadeIn = false;
		m_bFadeOut = true;
		m_fAlpha = 0.f;
		m_bFadeOutEnd = false;
	}
}

CUI_Fade* CUI_Fade::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Fade* pInstance = new CUI_Fade(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Fade");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Fade::Clone(void* pArg)
{
	CUI_Fade* pInstance = new CUI_Fade(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Fade");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Fade::Free()
{
	CUI::Free();
}