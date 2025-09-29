#include "pch.h"
#include "UI_CopyTexture.h"

#include "GameInstance.h"
#include "UI_Manager.h"

CUI_CopyTexture::CUI_CopyTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_CopyTexture::CUI_CopyTexture(const CUI_CopyTexture& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_CopyTexture::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_CopyTexture::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	pDesc->UIObjectDesc.fSizeX = g_iWindowSizeX;
	pDesc->UIObjectDesc.fSizeY = g_iWindowSizeY;
	pDesc->UIObjectDesc.fX = g_iWindowSizeX * 0.5f;
	pDesc->UIObjectDesc.fY = g_iWindowSizeY * 0.5f;

	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(CUIObject::Initialize(&pDesc->UIObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_CopyTexture::Priority_Update(_float fTimeDelta)
{
}

void CUI_CopyTexture::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (m_bShow)
	{
		m_fThreshold -= fTimeDelta * 8.f;

		if (m_fThreshold <= 0.f)
		{
			m_fThreshold = 0.f;

			if(m_ShowCallback)
				m_ShowCallback();

			m_isActive = false;
		}

		m_pTransformCom->Set_Scale(g_iWindowSizeX * (1.f - m_fThreshold), g_iWindowSizeY * (1.f - m_fThreshold), 1.f);
	}
	else
	{
		m_fThreshold += fTimeDelta * 6.f;

		if (m_fThreshold >= 1.f)
		{
			m_fThreshold = 1.f;

			if (m_HideCallback)
				m_HideCallback();

			m_isActive = false;
		}

		m_pTransformCom->Set_Scale(g_iWindowSizeX * (1.f + m_fThreshold * 0.1f), g_iWindowSizeY * (1.f + m_fThreshold * 0.1f), 1.f);
	}
}

void CUI_CopyTexture::Late_Update(_float fTimeDelta)
{
	if(m_isActive)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
}

HRESULT CUI_CopyTexture::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fThreshold", &m_fThreshold, sizeof(_float))))
		return E_FAIL;

	m_pShaderCom->Begin((_uint)SHADER_PASS_UI::COPY_TEXTURE);

	m_pVIBufferCom->Bind_InputAssembler();
	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_CopyTexture::Show()
{
	m_bShow = true;
	m_isActive = true;
	m_fThreshold = 1.f;
}

void CUI_CopyTexture::Hide()
{
	m_bShow = false;
	m_isActive = true;
	m_fThreshold = 0.f;
}

HRESULT CUI_CopyTexture::Ready_Components()
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
	if (FAILED(CUIObject::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"),
		TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		return E_FAIL;
	

	return S_OK;
}

CUI_CopyTexture* CUI_CopyTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CopyTexture* pInstance = new CUI_CopyTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CopyTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_CopyTexture::Clone(void* pArg)
{
	CUI_CopyTexture* pInstance = new CUI_CopyTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CopyTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_CopyTexture::Free()
{
    CUIObject::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pVIBufferCom);
}

