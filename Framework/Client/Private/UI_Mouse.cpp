#include "pch.h"
#include "UI_Mouse.h"

#include "GameInstance.h"

CUI_Mouse::CUI_Mouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Mouse::CUI_Mouse(const CUI_Mouse& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Mouse::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Mouse::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	return S_OK;
}

void CUI_Mouse::Priority_Update(_float fTimeDelta)
{
}

void CUI_Mouse::Update(_float fTimeDelta)
{
	_float2 vMousePos = m_pGameInstance->Calulate_ScreenMousePos();

	m_pTransformCom->Locate(vMousePos.x - g_iWindowSizeX * 0.5f, -vMousePos.y + g_iWindowSizeY * 0.5f, 0.0f);

	if (m_pGameInstance->Mouse_Press(DIM_LB))
		m_iTextureNum = 1;
	else
		m_iTextureNum = 0;
}

void CUI_Mouse::Late_Update(_float fTimeDelta)
{
	if(m_isActive)
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_Mouse::Render()
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

CUI_Mouse* CUI_Mouse::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Mouse* pInstance = new CUI_Mouse(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Mouse");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Mouse::Clone(void* pArg)
{
	CUI_Mouse* pInstance = new CUI_Mouse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Mouse");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Mouse::Free()
{
    CUI::Free();
}
