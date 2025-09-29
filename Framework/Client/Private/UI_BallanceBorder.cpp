#include "pch.h"
#include "UI_BallanceBorder.h"

#include "GameInstance.h"

CUI_BallanceBorder::CUI_BallanceBorder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_BallanceBorder::CUI_BallanceBorder(const CUI_BallanceBorder& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_BallanceBorder::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_BallanceBorder::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_fStretchFactor = pDesc->fStretchFactor;
	m_bMiddle = pDesc->bMiddle;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	return S_OK;
}

void CUI_BallanceBorder::Priority_Update(_float fTimeDelta)
{
}

void CUI_BallanceBorder::Update(_float fTimeDelta)
{
	/*if (m_pGameInstance->Key_Down(KEY::UP))
		m_fStretchFactor += 1.f * m_fChangeRatio;
	else if (m_pGameInstance->Key_Down(KEY::DOWN))
		m_fStretchFactor -= 1.f * m_fChangeRatio;*/

	m_fFrame += fTimeDelta;

	if (m_bInWorld)
		Change_World(m_fWorldOffsetY);

	if (m_bDisappear)
	{
		m_tParams.vSrcColor.w -= fTimeDelta;
		if (m_tParams.vSrcColor.w < 0.f)
		{
			m_isActive = false;
			m_tParams.vSrcColor.w = 1.f;
		}
	}
	else
	{
		m_tParams.vSrcColor.w = (sin(m_fFrame * XM_PI * 2) + 1.f) / 2.f;
	}
}

void CUI_BallanceBorder::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_BallanceBorder::Render()
{
	if (!m_pTextureCom)
		return S_OK;

	_float fRatio = (m_tParams.vStretchRange.y - m_tParams.vStretchRange.x) * (m_fStretchFactor - 1) + 1;

	m_pTransformCom->Set_Scale(m_vScale.x * fRatio, m_vScale.y, 1.f);

	if (!m_bMiddle)
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

	if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_UI::STRETCH)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_BallanceBorder::Blink()
{
	m_isActive = true;
	m_bDisappear = false;
	m_pTransformCom->Locate(m_vLocate.x - s_fViewportWidth * 0.5f, -m_vLocate.y + s_fViewportHeight * 0.5f, m_vLocate.z);
}

void CUI_BallanceBorder::Disappear()
{
	m_isActive = true;
	m_bDisappear = true;
	m_pTransformCom->Locate(m_vLocate.x - s_fViewportWidth * 0.5f, -m_vLocate.y + s_fViewportHeight * 0.5f, 0.f);
	m_tParams.vSrcColor.w = 1.f;
}

CUI_BallanceBorder* CUI_BallanceBorder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_BallanceBorder* pInstance = new CUI_BallanceBorder(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_BallanceBorder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_BallanceBorder::Clone(void* pArg)
{
	CUI_BallanceBorder* pInstance = new CUI_BallanceBorder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_BallanceBorder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BallanceBorder::Free()
{
	CUI::Free();
}