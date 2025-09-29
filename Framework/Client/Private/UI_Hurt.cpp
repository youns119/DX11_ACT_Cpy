#include "pch.h"
#include "UI_Hurt.h"

#include "GameInstance.h"

CUI_Hurt::CUI_Hurt(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Hurt::CUI_Hurt(const CUI_Hurt& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Hurt::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Hurt::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pCurHp = pDesc->pCurHp;
	m_fLastHp = *m_pCurHp;

	m_pMaxHp = pDesc->pMaxHp;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_isActive = false;

	return S_OK;
}

void CUI_Hurt::Priority_Update(_float fTimeDelta)
{
	if (m_fLastHp <= *m_pMaxHp * 0.3f)
	{
		m_isActive = true;
	}

	if (m_fLastHp != *m_pCurHp)
	{
		if (m_fLastHp > *m_pCurHp)
			m_isActive = true;

		m_fLastHp = *m_pCurHp;
	}
}

void CUI_Hurt::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_fFrame += fTimeDelta;
	m_tParams.vSrcColor.w = (0.8f - m_fFrame);

	if (m_fFrame > 0.8f)
	{
		m_fFrame = 0.f;
		m_isActive = false;
	}

}

void CUI_Hurt::Late_Update(_float fTimeDelta)
{
	if(m_isActive)
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_Hurt::Render()
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

CUI_Hurt* CUI_Hurt::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Hurt* pInstance = new CUI_Hurt(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Hurt");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Hurt::Clone(void* pArg)
{
	CUI_Hurt* pInstance = new CUI_Hurt(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Hurt");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Hurt::Free()
{
	CUI::Free();
}