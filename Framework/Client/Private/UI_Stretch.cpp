#include "pch.h"
#include "UI_Stretch.h"

#include "GameInstance.h"

CUI_Stretch::CUI_Stretch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Stretch::CUI_Stretch(const CUI_Stretch& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Stretch::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Stretch::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_fLengthStandard = pDesc->fLengthStandard;
	m_fLengthDelta = pDesc->fLengthDelta;

	m_fStartFactor = pDesc->fStartFactor;
	m_fChangeRatio = pDesc->fChangeRatio;

	m_bMiddle = pDesc->bMiddle;

	if (pDesc->pCur && pDesc->pMax)
		Set_BarValue(pDesc->pCur, pDesc->pMax, pDesc->pTemp);

	return S_OK;
}

void CUI_Stretch::Priority_Update(_float fTimeDelta)
{
}

void CUI_Stretch::Update(_float fTimeDelta)
{
	if (m_bInWorld)
		Change_World(m_fWorldOffsetY);

	if (m_pTemp)
		m_fStretchFactor = m_fStartFactor + (*m_pTemp - m_fLengthStandard) / m_fLengthDelta * m_fChangeRatio;
}

void CUI_Stretch::Late_Update(_float fTimeDelta)
{
	if (m_isActive && m_fStretchFactor >= 1.f)
	{
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
	}
}

HRESULT CUI_Stretch::Render()
{
	if (!m_pTextureCom)
		return S_OK;

	_float fRatio = (m_tParams.vStretchRange.y - m_tParams.vStretchRange.x) * (m_fStretchFactor - 1) + 1;

	m_pTransformCom->Set_Scale(m_vScale.x * fRatio, m_vScale.y * m_fScalingRatio, 1.f);

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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fStretchFactor", &fRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bHovered", &m_bHover, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bRightAngle", &m_bRightAngle, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bStretchY", &m_bStretchY, sizeof(_bool))))
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

void CUI_Stretch::Set_BarValue(_float* pCur, _float* pMax, _float* pTemp)
{
	m_pCur = pCur;
	m_pMax = pMax;

	if (pTemp)
		m_pTemp = pTemp;
	else
		m_pTemp = m_pMax;
}

CUI_Stretch* CUI_Stretch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Stretch* pInstance = new CUI_Stretch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Stretch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Stretch::Clone(void* pArg)
{
	CUI_Stretch* pInstance = new CUI_Stretch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Stretch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Stretch::Free()
{
	CUI::Free();
}