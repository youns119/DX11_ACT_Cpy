#include "pch.h"
#include "UI_ChatBubble.h"

#include "GameInstance.h"

CUI_ChatBubble::CUI_ChatBubble(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_ChatBubble::CUI_ChatBubble(const CUI_ChatBubble& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_ChatBubble::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ChatBubble::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	m_pRender = pDesc->pRender;

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	return S_OK;
}

void CUI_ChatBubble::Priority_Update(_float fTimeDelta)
{
}

void CUI_ChatBubble::Update(_float fTimeDelta)
{
	if (m_bInWorld)
		Change_World(m_fWorldOffsetY);
}

void CUI_ChatBubble::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (*m_pRender)
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_ChatBubble::Render()
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

CUI_ChatBubble* CUI_ChatBubble::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ChatBubble* pInstance = new CUI_ChatBubble(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_ChatBubble");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_ChatBubble::Clone(void* pArg)
{
	CUI_ChatBubble* pInstance = new CUI_ChatBubble(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_ChatBubble");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_ChatBubble::Free()
{
	CUI::Free();
}