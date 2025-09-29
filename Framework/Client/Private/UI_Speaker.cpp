#include "pch.h"
#include "UI_Speaker.h"

#include "GameInstance.h"

CUI_Speaker::CUI_Speaker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Speaker::CUI_Speaker(const CUI_Speaker& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Speaker::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Speaker::Initialize(void* pArg)
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

void CUI_Speaker::Priority_Update(_float fTimeDelta)
{
}

void CUI_Speaker::Update(_float fTimeDelta)
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

	if (m_bQuiet)
	{
		if (m_fScaleRatio > 0.9f)
			m_fScaleRatio -= 0.02f;
		else
			m_fScaleRatio = 0.9f;
	}
	else
	{
		if (m_fScaleRatio < 1.f)
			m_fScaleRatio += 0.02f;
		else
			m_fScaleRatio = 1.f;
	}

	m_pTransformCom->Set_Scale(m_vScale.x * m_fScaleRatio, m_vScale.y * m_fScaleRatio, 1.f);
}

void CUI_Speaker::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
	}
}

HRESULT CUI_Speaker::Render()
{
	if (!m_pTextureCom)
		return S_OK;

	_int iShaderPass = {};

	if (m_bQuiet)
		iShaderPass = (_int)SHADER_PASS_UI::COLOR_BLEND;
	else
		iShaderPass = m_iShaderPass;

	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", s_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", s_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_Texture", m_iTextureNum)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UIParams", &m_tParams, sizeof(UI_PARAMS))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bHovered", &m_bHover, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bRightAngle", &m_bRightAngle, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bStretchY", &m_bStretchY, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CUI_Speaker* CUI_Speaker::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Speaker* pInstance = new CUI_Speaker(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Speaker");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Speaker::Clone(void* pArg)
{
	CUI_Speaker* pInstance = new CUI_Speaker(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Speaker");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Speaker::Free()
{
	CUI::Free();
}