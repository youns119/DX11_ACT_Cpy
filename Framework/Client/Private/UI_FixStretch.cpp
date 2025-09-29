#include "pch.h"
#include "UI_FixStretch.h"

#include "GameInstance.h"

CUI_FixStretch::CUI_FixStretch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_FixStretch::CUI_FixStretch(const CUI_FixStretch& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_FixStretch::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_FixStretch::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_fStartFactor = pDesc->fStartFactor;
	m_fEndFactor = pDesc->fEndFactor;
	m_fStretchFactor = m_fStartFactor;

	m_fChangeRatio = pDesc->fChangeRatio;
	m_fStretchSpeed = pDesc->fStretchSpeed;

	m_bMiddle = pDesc->bMiddle;

	m_pCur = pDesc->pCur;
	m_pMax = pDesc->pMax;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	return S_OK;
}

void CUI_FixStretch::Priority_Update(_float fTimeDelta)
{
}

void CUI_FixStretch::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_fFrame += 5.f * fTimeDelta;

	if (m_bInWorld)
		Change_World(m_fWorldOffsetY);

	if (m_fStretchFactor < m_fEndFactor)
	{
		m_fStretchFactor = m_fFrame * m_fChangeRatio * m_fStretchSpeed;
		m_bStretched = false;
	}
	else
	{
		m_fStretchFactor = m_fEndFactor;
		m_bStretched = true;
	}
}

void CUI_FixStretch::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
	}
}

HRESULT CUI_FixStretch::Render()
{
	if (!m_pTextureCom)
		return S_OK;

	_float fRatio = (m_tParams.vStretchRange.y - m_tParams.vStretchRange.x) * (m_fStretchFactor - 1) + 1;

	m_pTransformCom->Set_Scale(m_vScale.x * fRatio, m_vScale.y, 1.f);

	if(!m_bMiddle)
		m_pTransformCom->Locate(m_vLocate.x + m_vScale.x * (fRatio - 1) / 2.f - s_fViewportWidth * 0.5f, -m_vLocate.y + s_fViewportHeight * 0.5f, m_vLocate.z);

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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fStretchFactor", &fRatio, sizeof(_float))))
		return E_FAIL;

	if (m_pCur)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fCur", m_pCur, sizeof(_float))))
			return E_FAIL;
	}
	
	if (m_pMax)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fMax", m_pMax, sizeof(_float))))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_FixStretch::Reset()
{
	m_isActive = true;
	m_fStretchFactor = m_fStartFactor;
	m_fFrame = 0;
}

void CUI_FixStretch::Change_EndFactor(_float fEndFactor)
{ 
	if (m_fStretchFactor > fEndFactor)
		m_fStretchFactor = m_fStartFactor;

	m_fEndFactor = fEndFactor;
}

CUI_FixStretch* CUI_FixStretch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_FixStretch* pInstance = new CUI_FixStretch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_FixStretch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_FixStretch::Clone(void* pArg)
{
	CUI_FixStretch* pInstance = new CUI_FixStretch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_FixStretch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_FixStretch::Free()
{
	CUI::Free();
}