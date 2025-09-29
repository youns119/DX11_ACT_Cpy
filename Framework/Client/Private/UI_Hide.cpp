#include "pch.h"
#include "UI_Hurt.h"

#include "GameInstance.h"
#include "FPlayerStat.h"
#include "UI_Hide.h"

CUI_Hide::CUI_Hide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Hide::CUI_Hide(const CUI_Hide& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Hide::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Hide::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pPlayerStat = pDesc->pPlayerStat;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_isActive = false;

	return S_OK;
}

void CUI_Hide::Priority_Update(_float fTimeDelta)
{
	if (m_pPlayerStat->bHasShell && m_pPlayerStat->bGuard)
	{
		if (!m_bIsPlayerHide)
		{
			m_bIsPlayerHide = true;
			Show();
		}
	}
	else
	{
		if (m_bIsPlayerHide)
		{
			m_bIsPlayerHide = false;
			Hide();
		}
	}
}

void CUI_Hide::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_fFrame += fTimeDelta;

	if (m_bIsPlayerHide)
	{
		if (!m_bEnd)
		{
			m_tParams.vSrcColor.w += m_fFrame;

			if (m_tParams.vSrcColor.w >= 0.8f)
				m_bEnd = true;
		}
	}
	else
	{
		if (!m_bEnd)
		{
			m_tParams.vSrcColor.w -= m_fFrame;

			if (m_tParams.vSrcColor.w <= 0.f)
			{
				m_bEnd = true;
				m_isActive = false;
			}
		}
		
	}
}

void CUI_Hide::Late_Update(_float fTimeDelta)
{
	if(m_isActive)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
}

HRESULT CUI_Hide::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UIParams", &m_tParams, sizeof(UI_PARAMS))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bHovered", &m_bHover, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bRightAngle", &m_bRightAngle, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bStretchY", &m_bStretchY, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &m_fFrame, sizeof(_float))))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_Hide::Show()
{
	m_isActive = true;

	m_tParams.vSrcColor.w = 0.f;
	m_fFrame = 0.f;
	m_bIsPlayerHide = true;
	m_bEnd = false;
}

void CUI_Hide::Hide()
{
	m_tParams.vSrcColor.w = 0.8f;
	m_fFrame = 0.f;
	m_bIsPlayerHide = false;
	m_bEnd = false;
}

CUI_Hide* CUI_Hide::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Hide* pInstance = new CUI_Hide(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Hide");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Hide::Clone(void* pArg)
{
	CUI_Hide* pInstance = new CUI_Hide(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Hide");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Hide::Free()
{
	CUI::Free();
}