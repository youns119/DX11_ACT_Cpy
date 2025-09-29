#include "pch.h"
#include "UI_TextTexture.h"

#include "GameInstance.h"
#include "UI_Manager.h"

CUI_TextTexture::CUI_TextTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_TextTexture::CUI_TextTexture(const CUI_TextTexture& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_TextTexture::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_TextTexture::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	pDesc->UIObjectDesc.fSizeX = g_iWindowSizeX;
	pDesc->UIObjectDesc.fSizeY = 120.f;
	pDesc->UIObjectDesc.fX = g_iWindowSizeX * 0.5f;
	pDesc->UIObjectDesc.fY = g_iWindowSizeY * 0.5f;

	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(CUIObject::Initialize(&pDesc->UIObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CUI_TextTexture::Priority_Update(_float fTimeDelta)
{
}

void CUI_TextTexture::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (m_isActive)
	{
		if(m_bShowEffect)
			m_fTime += fTimeDelta;

		if (m_fFrame < 5.f)
		{
			m_fFrame += fTimeDelta;

			if (m_fFrame < 1.5f)
				m_fTextAlpha += fTimeDelta;
			else if (m_fFrame < 3.5f)
				m_fTextAlpha = 1.f;
			else if (m_fFrame < 5.f)
				m_fTextAlpha -= fTimeDelta;
		}
		else
			m_isActive = false;
	}
}

void CUI_TextTexture::Late_Update(_float fTimeDelta)
{
	if(m_isActive)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
}

HRESULT CUI_TextTexture::Render()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", s_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", s_ProjMatrix)))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_UI", m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pMaskTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTextAlpha", &m_fTextAlpha, sizeof(_float))))
		return E_FAIL;

	if(m_bShowEffect)
		m_pShaderCom->Begin((_uint)SHADER_PASS_UI::TEXT_TEXTURE);
	else 
		m_pShaderCom->Begin((_uint)SHADER_PASS_UI::COLOR_BLEND);

	m_pVIBufferCom->Bind_InputAssembler();
	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_TextTexture::Show(_bool bShowEffect)
{
	m_isActive = true;
	m_bShowEffect = bShowEffect;
	m_fTextAlpha = 0.f;
	m_fFrame = 0.f;
}

HRESULT CUI_TextTexture::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(CUIObject::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(CUIObject::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(CUIObject::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wave"),
		TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		return E_FAIL;
	

	return S_OK;
}

CUI_TextTexture* CUI_TextTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_TextTexture* pInstance = new CUI_TextTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_TextTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_TextTexture::Clone(void* pArg)
{
	CUI_TextTexture* pInstance = new CUI_TextTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_TextTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_TextTexture::Free()
{
    CUIObject::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pVIBufferCom);
}

