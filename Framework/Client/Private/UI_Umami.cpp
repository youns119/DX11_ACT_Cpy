#include "pch.h"
#include "UI_Umami.h"

#include "GameInstance.h"

CUI_Umami::CUI_Umami(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Umami::CUI_Umami(const CUI_Umami& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Umami::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Umami::Initialize(void* pArg)
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

	return S_OK;
}

void CUI_Umami::Priority_Update(_float fTimeDelta)
{
}

void CUI_Umami::Update(_float fTimeDelta)
{
	if (m_iIndex + 1 > (_int)*m_pUmamiPoint)
	{
		m_fFrame += fTimeDelta;

		_float fScale = max(0.8f, 1.f - m_fFrame * 2.f);

		m_pTransformCom->Set_Scale(m_vScale.x * fScale, m_vScale.y * 0.8f, fScale);
		m_iTextureNum = 0;
	}
	else
	{
		m_fFrame = 0.f;

		m_iTextureNum = 1;
		m_pTransformCom->Set_Scale(m_vScale.x, m_vScale.y, 1.f);
	}
}

void CUI_Umami::Late_Update(_float fTimeDelta)
{
	if (m_pOwner)
		m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
	else
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_Umami::Render()
{
	if (!m_pTextureCom)
		return S_OK;

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

CUI_Umami* CUI_Umami::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Umami* pInstance = new CUI_Umami(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Umami");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Umami::Clone(void* pArg)
{
	CUI_Umami* pInstance = new CUI_Umami(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Umami");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Umami::Free()
{
	CUI::Free();
}