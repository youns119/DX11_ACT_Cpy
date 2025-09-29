#include "pch.h"
#include "UI_Lock.h"

#include "GameInstance.h"

CUI_Lock::CUI_Lock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Lock::CUI_Lock(const CUI_Lock& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Lock::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Lock::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	m_pLockOn = pDesc->pLockOn;

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;
	
	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_isActive = false;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));


	return S_OK;
}

void CUI_Lock::Priority_Update(_float fTimeDelta)
{
}

void CUI_Lock::Update(_float fTimeDelta)
{
	if (*m_pLockOn)
	{
		m_isActive = true;
		if(m_fFrame <= 0.15f)
			m_fFrame += fTimeDelta;
	}
	else
	{
		if(m_fFrame > 0.f)
			m_fFrame -= 1.5f * fTimeDelta;
	}

	if (m_fFrame < 0.f)
	{
		m_isActive = false;
		m_fFrame = 0.f;
	}
}

void CUI_Lock::Late_Update(_float fTimeDelta)
{
	if(m_isActive)
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_Lock::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fFrame, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_UI::LOCK)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CUI_Lock* CUI_Lock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Lock* pInstance = new CUI_Lock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Lock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Lock::Clone(void* pArg)
{
	CUI_Lock* pInstance = new CUI_Lock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Lock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Lock::Free()
{
	CUI::Free();
}