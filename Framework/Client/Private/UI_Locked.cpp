#include "pch.h"
#include "UI_Locked.h"

#include "GameInstance.h"

CUI_Locked::CUI_Locked(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Locked::CUI_Locked(const CUI_Locked& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Locked::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Locked::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	m_pLocked = pDesc->pLocked;

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));


	return S_OK;
}

void CUI_Locked::Priority_Update(_float fTimeDelta)
{
}

void CUI_Locked::Update(_float fTimeDelta)
{
	if (m_bInWorld)
		Change_World(m_fWorldOffsetY);
}

void CUI_Locked::Late_Update(_float fTimeDelta)
{
	if (*m_pLocked)
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_Locked::Render()
{
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

CUI_Locked* CUI_Locked::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Locked* pInstance = new CUI_Locked(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Locked");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Locked::Clone(void* pArg)
{
	CUI_Locked* pInstance = new CUI_Locked(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Locked");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Locked::Free()
{
	CUI::Free();
}