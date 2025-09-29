#include "pch.h"
#include "UI_SkillEffect.h"

#include "GameInstance.h"

CUI_SkillEffect::CUI_SkillEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_SkillEffect::CUI_SkillEffect(const CUI_SkillEffect& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_SkillEffect::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SkillEffect::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_isActive = false;

	return S_OK;
}

void CUI_SkillEffect::Priority_Update(_float fTimeDelta)
{
}

void CUI_SkillEffect::Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_fFrame += fTimeDelta;

		if (m_fFrame >= 1.f)
		{
			m_fFrame = 1.f;
			m_isActive = false;
		}
	}
}

void CUI_SkillEffect::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
	}
}

HRESULT CUI_SkillEffect::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &m_fFrame, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_UI::SKILLEFFECT)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_SkillEffect::Show()
{
	m_fFrame = 0.f;
	m_isActive = true;
}

CUI_SkillEffect* CUI_SkillEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_SkillEffect* pInstance = new CUI_SkillEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_SkillEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_SkillEffect::Clone(void* pArg)
{
	CUI_SkillEffect* pInstance = new CUI_SkillEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_SkillEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_SkillEffect::Free()
{
	CUI::Free();
}