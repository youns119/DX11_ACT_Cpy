#include "pch.h"
#include "UI_Loading.h"

#include "GameInstance.h"

CUI_Loading::CUI_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Loading::CUI_Loading(const CUI_Loading& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Loading::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loading::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_fSpeed = pDesc->fSpeed;
	m_pNextLevelID = pDesc->pNextLevelID;

	return S_OK;
}

void CUI_Loading::Priority_Update(_float fTimeDelta)
{
	switch (*m_pNextLevelID)
	{
	case LEVEL_BEACH:
		m_iTextureNum = 1;
		m_bGamma = true;
		break;
	case LEVEL_VILLAGE:
		m_iTextureNum = 2;
		m_bGamma = true;
		break;
	case LEVEL_FIELD:
		m_iTextureNum = 3;
		m_bGamma = true;
		break;
	case LEVEL_PINBALL:
		m_iTextureNum = 4;
		m_bGamma = true;
		break;
	case LEVEL_CLUB:
		m_iTextureNum = 5;
		m_bGamma = true;
		break;
	case LEVEL_CRABTOMB:
		m_iTextureNum = 6;
		m_bGamma = true;
		break;
	default:
		m_iTextureNum = 0;
		m_bGamma = false;
		break;
	}
}

void CUI_Loading::Update(_float fTimeDelta)
{
	//if (m_iTextureNum == 0)
	//{
		if (m_fFrame <= 125.f)
			m_fFrame += m_fSpeed * fTimeDelta;

		m_pTransformCom->Set_Scale(m_vScale.x, m_vScale.y, 1.f);
		m_pTransformCom->Locate(m_vLocate.x - s_fViewportWidth * 0.5f, -m_vLocate.y + s_fViewportHeight * 0.5f + m_fFrame, m_vLocate.z);
	//}
	//else
	//{
	//	m_pTransformCom->Set_Scale(g_iWindowSizeX, g_iWindowSizeY, 1.f);
	//	m_pTransformCom->Locate(0.f, 0.f, m_vLocate.z);
	//}
}

void CUI_Loading::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_Loading::Render()
{
	if (!m_pTextureCom)
		return S_OK;

	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", s_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", s_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bGamma", &m_bGamma, sizeof(_bool))))
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

	m_bGamma = false;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bGamma", &m_bGamma, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CUI_Loading* CUI_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Loading* pInstance = new CUI_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Loading::Clone(void* pArg)
{
	CUI_Loading* pInstance = new CUI_Loading(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Loading::Free()
{
	CUI::Free();
}

