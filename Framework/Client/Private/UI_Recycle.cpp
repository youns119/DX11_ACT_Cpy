#include "pch.h"
#include "UI_Recycle.h"

#include "GameInstance.h"

CUI_Recycle::CUI_Recycle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Recycle::CUI_Recycle(const CUI_Recycle& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Recycle::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Recycle::Initialize(void* pArg)
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

void CUI_Recycle::Priority_Update(_float fTimeDelta)
{
}

void CUI_Recycle::Update(_float fTimeDelta)
{
	m_fFrame += fTimeDelta * 15.f;

	if (m_fFrame >= 1.f)
	{
 		m_iTextureNum++;
		m_fFrame = 0.f;
	}
	m_iTextureNum %= 10;
}

void CUI_Recycle::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_Recycle::Render()
{
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

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

    return S_OK;
}

CUI_Recycle* CUI_Recycle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Recycle* pInstance = new CUI_Recycle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Recycle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Recycle::Clone(void* pArg)
{
	CUI_Recycle* pInstance = new CUI_Recycle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Recycle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Recycle::Free()
{
    CUI::Free();
}
