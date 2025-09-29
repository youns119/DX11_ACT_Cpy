#include "pch.h"
#include "UI_DefaultStretch.h"

#include "GameInstance.h"

CUI_DefaultStretch::CUI_DefaultStretch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_DefaultStretch::CUI_DefaultStretch(const CUI_DefaultStretch& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_DefaultStretch::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_DefaultStretch::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_fStretchFactor = pDesc->fStretchFactor;
	m_bMiddle = pDesc->bMiddle;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_fRatio = (m_tParams.vStretchRange.y - m_tParams.vStretchRange.x) * (m_fStretchFactor - 1) + 1;

	if (m_bStretchY)
		m_pTransformCom->Set_Scale(m_vScale.x, m_vScale.y * m_fRatio, 1.f);
	else
		m_pTransformCom->Set_Scale(m_vScale.x * m_fRatio, m_vScale.y, 1.f);

	if (!m_bMiddle)
		m_pTransformCom->Locate(m_vLocate.x + m_vScale.x * (m_fRatio - 1) / 2.f - s_fViewportWidth * 0.5f, -m_vLocate.y + s_fViewportHeight * 0.5f, m_vLocate.z);

	return S_OK;
}

void CUI_DefaultStretch::Priority_Update(_float fTimeDelta)
{
}

void CUI_DefaultStretch::Update(_float fTimeDelta)
{
	if (m_bInWorld)
		Change_World(m_fWorldOffsetY);
}

void CUI_DefaultStretch::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
	}
}

HRESULT CUI_DefaultStretch::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fStretchFactor", &m_fRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	_float2 vTextPos = { m_vLocate.x + m_vTextOffset.x, m_vLocate.y + m_vTextOffset.y };

	m_pGameInstance->Render_Font(m_strFontTag, m_szText, vTextPos, XMLoadFloat4(&m_vTextColor), 0.f, { 0.f, 0.f }, m_fTextScale, m_eSorting);

	return S_OK;
}

CUI_DefaultStretch* CUI_DefaultStretch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_DefaultStretch* pInstance = new CUI_DefaultStretch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_DefaultStretch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_DefaultStretch::Clone(void* pArg)
{
	CUI_DefaultStretch* pInstance = new CUI_DefaultStretch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_DefaultStretch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_DefaultStretch::Free()
{
	CUI::Free();
}