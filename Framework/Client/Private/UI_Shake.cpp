#include "pch.h"
#include "UI_Shake.h"

#include "GameInstance.h"

CUI_Shake::CUI_Shake(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Shake::CUI_Shake(const CUI_Shake& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Shake::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Shake::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	return S_OK;
}

void CUI_Shake::Priority_Update(_float fTimeDelta)
{
}

void CUI_Shake::Update(_float fTimeDelta)
{
	_float3 vOffset;

	m_fFrame += fTimeDelta;

	if (m_fFrame > 0.3f)
	{
		m_fFrame = 0.f;

		vOffset.x = CUtils::Compute_Random(-1, 1) * m_fShakeAmount;
		vOffset.y = CUtils::Compute_Random(-1, 1) * m_fShakeAmount;

		m_pTransformCom->Locate(m_vLocate.x - s_fViewportWidth * 0.5f + vOffset.x, -m_vLocate.y + s_fViewportHeight * 0.5f + vOffset.y, m_vLocate.z);
	}
}

void CUI_Shake::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
	}
}

HRESULT CUI_Shake::Render()
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

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CUI_Shake* CUI_Shake::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Shake* pInstance = new CUI_Shake(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Shake");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Shake::Clone(void* pArg)
{
	CUI_Shake* pInstance = new CUI_Shake(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Shake");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Shake::Free()
{
	CUI::Free();
}