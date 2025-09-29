#include "pch.h"
#include "UI_Ghost.h"

#include "GameInstance.h"

CUI_Ghost::CUI_Ghost(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Ghost::CUI_Ghost(const CUI_Ghost& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Ghost::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Ghost::Initialize(void* pArg)
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

void CUI_Ghost::Priority_Update(_float fTimeDelta)
{
}

void CUI_Ghost::Update(_float fTimeDelta)
{
	m_fFrame += 2.0f * fTimeDelta;
	
	_float fY = m_fWorldOffsetY + sin(m_fFrame) / 4.f;

	if (m_bInWorld)
		Change_World(fY + 2.f);
}

void CUI_Ghost::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_Ghost::Render()
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

CUI_Ghost* CUI_Ghost::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Ghost* pInstance = new CUI_Ghost(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Ghost");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Ghost::Clone(void* pArg)
{
	CUI_Ghost* pInstance = new CUI_Ghost(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Ghost");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Ghost::Free()
{
    CUI::Free();
}
