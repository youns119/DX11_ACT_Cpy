#include "pch.h"
#include "UI_UmamiEffect.h"

#include "GameInstance.h"

CUI_UmamiEffect::CUI_UmamiEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_UmamiEffect::CUI_UmamiEffect(const CUI_UmamiEffect& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_UmamiEffect::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UmamiEffect::Initialize(void* pArg)
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

	m_iIndex = pDesc->iIndex;
	m_pUmamiPoint = pDesc->pUmamiPoint;
	m_iCurUmamiPoint = (_int)*m_pUmamiPoint;

	m_isActive = false;

	return S_OK;
}

void CUI_UmamiEffect::Priority_Update(_float fTimeDelta)
{
	if (m_iCurUmamiPoint != (_int)*m_pUmamiPoint)
	{
		m_iCurUmamiPoint = (_int)*m_pUmamiPoint;

		if (m_iIndex == m_iCurUmamiPoint)
		{
			m_isActive = true;
			m_fFrame = 0;
			m_tParams.vSrcColor.w = 1.f;
		}
	}
}

void CUI_UmamiEffect::Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_fFrame += fTimeDelta * 5.f;

		_float fScale = min(4.f, 1.f + m_fFrame * 10.f);
		_float fLocateZ = max(0.1f, m_vLocate.z - m_fFrame);

		m_pTransformCom->Set_Scale(m_vScale.x * fScale, m_vScale.y * fScale, 1.f);
		m_pTransformCom->Locate(m_vLocate.x - s_fViewportWidth * 0.5f, -m_vLocate.y + s_fViewportHeight * 0.5f, fLocateZ);

		m_tParams.vSrcColor.w -= m_fFrame;

		if (m_tParams.vSrcColor.w <= 0.0f)
		{
			m_isActive = false;
		}
	}
}

void CUI_UmamiEffect::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
	}
}

HRESULT CUI_UmamiEffect::Render()
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

CUI_UmamiEffect* CUI_UmamiEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_UmamiEffect* pInstance = new CUI_UmamiEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_UmamiEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_UmamiEffect::Clone(void* pArg)
{
	CUI_UmamiEffect* pInstance = new CUI_UmamiEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_UmamiEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_UmamiEffect::Free()
{
	CUI::Free();
}