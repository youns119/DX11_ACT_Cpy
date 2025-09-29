#include "pch.h"
#include "UI_BallanceBar.h"

#include "GameInstance.h"

CUI_BallanceBar::CUI_BallanceBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_BallanceBar::CUI_BallanceBar(const CUI_BallanceBar& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_BallanceBar::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_BallanceBar::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_fChangeRatio = pDesc->fChangeRatio;
	m_fEndFactor = pDesc->fEndFactor;

	m_pCur = pDesc->pCur;
	m_pMax = pDesc->pMax;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	return S_OK;
}

void CUI_BallanceBar::Priority_Update(_float fTimeDelta)
{
}

void CUI_BallanceBar::Update(_float fTimeDelta)
{
	/*if (m_pGameInstance->Key_Down(KEY::UP))
		m_fStretchFactor += 1.f * m_fChangeRatio;
	else if (m_pGameInstance->Key_Down(KEY::DOWN))
		m_fStretchFactor -= 1.f * m_fChangeRatio;*/

	m_fFrame += fTimeDelta;

	if (m_bInWorld)
		Change_World(m_fWorldOffsetY);

	m_fStretchFactor = m_fEndFactor / (*m_pMax) * (*m_pCur) * m_fChangeRatio;
}

void CUI_BallanceBar::Late_Update(_float fTimeDelta)
{
	if (m_fStretchFactor == 0.f)
		return;

	if(m_isActive)
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_BallanceBar::Render()
{
	if (!m_pTextureCom)
		return S_OK;

	_float fRatio = (m_tParams.vStretchRange.y - m_tParams.vStretchRange.x) * (m_fStretchFactor - 1) + 1;

	m_pTransformCom->Set_Scale(m_vScale.x * fRatio, m_vScale.y, 1.f);

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

	if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_UI::STRETCH)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CUI_BallanceBar* CUI_BallanceBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_BallanceBar* pInstance = new CUI_BallanceBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_BallanceBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_BallanceBar::Clone(void* pArg)
{
	CUI_BallanceBar* pInstance = new CUI_BallanceBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_BallanceBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BallanceBar::Free()
{
	CUI::Free();
}