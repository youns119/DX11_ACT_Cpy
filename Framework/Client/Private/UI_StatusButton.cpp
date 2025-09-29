#include "pch.h"
#include "UI_StatusButton.h"

#include "GameInstance.h"
#include "UI_Value.h"

CUI_StatusButton::CUI_StatusButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_StatusButton::CUI_StatusButton(const CUI_StatusButton& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_StatusButton::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_StatusButton::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Locate(m_vLocate.x - s_fViewportWidth * 0.5f, -m_vLocate.y + s_fViewportHeight * 0.5f, m_vLocate.z);

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));
	
	m_fStretchFactor = pDesc->fStretchFactor;

	if (FAILED(Ready_PartObject(pDesc)))
		return E_FAIL;

	m_fRatio = (m_tParams.vStretchRange.y - m_tParams.vStretchRange.x) * (m_fStretchFactor - 1) + 1;

	if (m_bStretchY)
		m_vScale.y *= m_fRatio;

	else
		m_vScale.x *= m_fRatio;

	m_pTransformCom->Set_Scale(m_vScale.x, m_vScale.y, 1.f);

	m_HoverCallback = pDesc->HoverCallback;

	return S_OK;
}

void CUI_StatusButton::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_StatusButton::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	_float2 fMousePos = m_pGameInstance->Calulate_ScreenMousePos();

	_float2 vMin = { m_vLocate.x - m_vScale.x * 0.5f, m_vLocate.y - m_vScale.y * 0.5f };
	_float2 vMax = { m_vLocate.x + m_vScale.x * 0.5f, m_vLocate.y + m_vScale.y * 0.5f };

	m_bHover = (fMousePos.x >= vMin.x && fMousePos.x <= vMax.x && fMousePos.y >= vMin.y && fMousePos.y <= vMax.y);

	m_fFrame += fTimeDelta;

	if (m_bHover)
	{
		if (m_HoverCallback)
		{
			if (!m_bHovered)
			{
				m_HoverCallback();
				m_bHovered = true;
			}
		}

		if (m_pGameInstance->Mouse_Down(DIM_LB) && m_Callback)
			m_Callback();
	}
	else
		m_bHovered = false;

	switch (m_iShaderPass)
	{
	case (_uint)SHADER_PASS_UI::OPTION_BUTTON:
		if (m_fFrame >= m_fInterval)
		{
			if (m_tParams.fThreshold > 0.1f)
				m_tParams.fThreshold -= 0.1f;
			else
				m_tParams.fThreshold = 0.1f;

			m_tParams.vSpriteIndex.y++;

			if (m_tParams.vSpriteIndex.y >= m_tParams.vSpriteSize.y)
			{
				m_tParams.vSpriteIndex.y = 0;
				m_tParams.vSpriteIndex.x++;

				if (m_tParams.vSpriteIndex.x >= m_tParams.vSpriteSize.x)
					m_tParams.vSpriteIndex = {};
			}
			m_fFrame = 0.f;
		}
		break;
	default:
		if (m_fFrame >= m_fInterval)
		{
			m_tParams.vSpriteIndex.y++;

			if (m_tParams.vSpriteIndex.y >= m_tParams.vSpriteSize.y)
			{
				m_tParams.vSpriteIndex.y = 0;
				m_tParams.vSpriteIndex.x++;

				if (m_tParams.vSpriteIndex.x >= m_tParams.vSpriteSize.x)
					m_tParams.vSpriteIndex = {};
			}

			m_fFrame = 0.f;
		}
		break;
	}

	super::Update(fTimeDelta);
}

void CUI_StatusButton::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
		
		super::Late_Update(fTimeDelta);
	}
}

HRESULT CUI_StatusButton::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fStretchFactor", &m_fRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_StatusButton::Ready_PartObject(DESC* pDesc)
{
	CUI_Value::DESC ValueDesc{};
	{
		ValueDesc.UIDesc.pOwner = m_pOwner;
		ValueDesc.UIDesc.strFileName = pDesc->strIconFileName;
		ValueDesc.pCount = pDesc->pCount;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Icon", LEVEL_STATIC, L"Prototype_GameObject_UI_Value", &ValueDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_StatusButton* CUI_StatusButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_StatusButton* pInstance = new CUI_StatusButton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_StatusButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_StatusButton::Clone(void* pArg)
{
	CUI_StatusButton* pInstance = new CUI_StatusButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_StatusButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_StatusButton::Free()
{
	CUI::Free();
}