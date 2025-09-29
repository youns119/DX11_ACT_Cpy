#include "pch.h"
#include "UI_Dummy.h"

#include "GameInstance.h"

CUI_Dummy::CUI_Dummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Dummy::CUI_Dummy(const CUI_Dummy& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_Dummy::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Dummy::Initialize(void* pArg)
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

void CUI_Dummy::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));
}

void CUI_Dummy::Update(_float fTimeDelta)
{
	_float2 fMousePos = m_pGameInstance->Calulate_ScreenMousePos();

	_float2 vMin = { m_vLocate.x - m_vScale.x * 0.5f, m_vLocate.y - m_vScale.y * 0.5f };
	_float2 vMax = { m_vLocate.x + m_vScale.x * 0.5f, m_vLocate.y + m_vScale.y * 0.5f };

	m_bHovered = (fMousePos.x >= vMin.x && fMousePos.x <= vMax.x && fMousePos.y >= vMin.y && fMousePos.y <= vMax.y);

	m_fFrame += fTimeDelta;

	switch (m_iShaderPass)
	{
	case (_uint)SHADER_PASS_UI::OPTION_BUTTON:
		if (m_fFrame >= m_fInterval)
		{
			if (m_fThreshold > 0.1f)
				m_fThreshold -= 0.1f;
			else
				m_fThreshold = 0.1f;

			m_vSpriteIndex.y++;

			if (m_vSpriteIndex.y >= m_vSpriteSize.y)
			{
				m_vSpriteIndex.y = 0;
				m_vSpriteIndex.x++;

				if (m_vSpriteIndex.x >= m_vSpriteSize.x)
					m_vSpriteIndex = {};
			}
			m_fFrame = 0.f;
		}
		break;
	case (_uint)SHADER_PASS_UI::MENU_BUTTON:
	case (_uint)SHADER_PASS_UI::BUTTON:
	case (_uint)SHADER_PASS_UI::NOISE:
		if (m_fFrame >= m_fInterval)
		{
			m_vSpriteIndex.y++;

			if (m_vSpriteIndex.y >= m_vSpriteSize.y)
			{
				m_vSpriteIndex.y = 0;
				m_vSpriteIndex.x++;

				if (m_vSpriteIndex.x >= m_vSpriteSize.x)
					m_vSpriteIndex = {};
			}

			m_fFrame = 0.f;
		}
		break;
	}
}

void CUI_Dummy::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
}

HRESULT CUI_Dummy::Render()
{
	if (!m_pTextureCom)
		return S_OK;

	_float4 vCurTextColor;

	if (m_bHovered)
		vCurTextColor = m_vHoverTextColor;
	else
		vCurTextColor = m_vTextColor;

	_float fRatio = (m_vStretchRange.y - m_vStretchRange.x) * (m_fStretchFactor - 1) + 1;

	if (m_bStretchY)
		m_pTransformCom->Set_Scale(m_vScale.x, m_vScale.y * fRatio, 1.f);
	else
		m_pTransformCom->Set_Scale(m_vScale.x * fRatio, m_vScale.y, 1.f);

	//m_pTransformCom->Locate(m_vLocate.x + m_vScale.x * (fRatio - 1) / 2.f - s_fViewportWidth * 0.5f, -m_vLocate.y + s_fViewportHeight * 0.5f, m_vLocate.z);

	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", s_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", s_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bHovered", &m_bHovered, sizeof(_bool))))
		return E_FAIL;

	if (m_pMaskTextureCom)
	{
		if (FAILED(m_pMaskTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_MaskTexture", 0)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bRightAngle", &m_bRightAngle, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bStretchY", &m_bStretchY, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vSpriteSize", &m_vSpriteSize, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vSpriteIndex", &m_vSpriteIndex, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fStretchFactor", &fRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fThreshold", &m_fThreshold, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vSrcColor", &m_vSrcColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDstColor", &m_vDstColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vStretchRange", &m_vStretchRange, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	_float2 vTextPos = { m_vLocate.x + m_vTextOffset.x, m_vLocate.y + m_vTextOffset.y };

	m_pGameInstance->Render_Font(m_strFontTag, m_szText, vTextPos, XMLoadFloat4(&vCurTextColor), 0.f, { 0.f, 0.f }, m_fTextScale, m_eSorting);

	return S_OK;
}

void CUI_Dummy::Clone_Texture(_uint iLevelIndex, const _wstring& strTextureTag)
{
	if (nullptr !=  m_pTextureCom)
	{
		CUIObject::Delete_Component(TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom));
		Safe_Release(m_pTextureCom);
	}

	CUIObject::Add_Component((LEVELID)iLevelIndex, strTextureTag, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom));

	m_strTextureTag = strTextureTag;
}

void CUI_Dummy::Clone_MaskTexture(_uint iLevelIndex, const _wstring& strMaskTextureTag)
{
	CUIObject::Delete_Component(TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom));
	Safe_Release(m_pMaskTextureCom);

	CUIObject::Add_Component((LEVELID)iLevelIndex, strMaskTextureTag, TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom));

	m_strMaskTextureTag = strMaskTextureTag;
}

void CUI_Dummy::Set_Scale(_float2 vScale)
{
	m_pTransformCom->Set_Scale(vScale.x, vScale.y, 0.f);
	m_vScale = vScale;
}

void CUI_Dummy::Set_Locate(_float3 vLocate)
{
	m_pTransformCom->Locate(vLocate.x - g_iWindowSizeX * 0.5f, -vLocate.y + g_iWindowSizeY * 0.5f, vLocate.z);
	m_vLocate = vLocate;
}

HRESULT CUI_Dummy::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(CUIObject::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(CUIObject::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_Dummy* CUI_Dummy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Dummy* pInstance = new CUI_Dummy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Dummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Dummy::Clone(void* pArg)
{
	CUI_Dummy* pInstance = new CUI_Dummy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Dummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Dummy::Free()
{
	CUIObject::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pMaskTextureCom);
}
