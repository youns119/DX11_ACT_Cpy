#include "pch.h"
#include "UI_Interaction.h"

#include "GameInstance.h"
#include "UI_Texture.h"

CUI_Interaction::CUI_Interaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Interaction::CUI_Interaction(const CUI_Interaction& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Interaction::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Interaction::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	lstrcpy(m_szText, pDesc->szText);

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_isActive = false;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	return S_OK;
}

void CUI_Interaction::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_Interaction::Update(_float fTimeDelta)
{	
	if (m_fFrame < 1.f)
		m_fFrame += 5.f * fTimeDelta;
	else
		m_fFrame = 1.f;

	// 텍스트 길이가 2일때가 기준이라서
	m_fStretchFactor = m_fChangeRatio * lstrlen(m_szText) - 7;

	super::Update(fTimeDelta);
}

void CUI_Interaction::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);

		if(m_fFrame >= 0.9f)
			super::Late_Update(fTimeDelta);
	}
}

HRESULT CUI_Interaction::Render()
{
	if (!m_pTextureCom)
		return S_OK;

	_float fRatio = (m_tParams.vStretchRange.y - m_tParams.vStretchRange.x) * (m_fStretchFactor - 1) + 1;

	m_pTransformCom->Set_Scale(m_vScale.x * fRatio * m_fFrame, m_vScale.y * m_fFrame, 1.f);
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

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (m_fFrame >= 0.9f)
	{
		_float2 vTextPos = { m_vLocate.x + m_vTextOffset.x, m_vLocate.y + m_vTextOffset.y };

		m_pGameInstance->Render_Font(m_strFontTag, m_szText, vTextPos, XMLoadFloat4(&m_vTextColor), 0.f, { 0.f, 0.f }, m_fTextScale);
	}
	
	return S_OK;
}

void CUI_Interaction::Show()
{
	m_fFrame = 0;
	m_isActive = true;
	m_iShowCount++;
}

void CUI_Interaction::Hide()
{
	m_iShowCount--;
	if (m_iShowCount <= 0)
		m_isActive = false;
	
}

HRESULT CUI_Interaction::Ready_PartObject()
{
	/* E */
	CUI_Texture::DESC TextureDesc{};
	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_E.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_E", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_Interaction* CUI_Interaction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Interaction* pInstance = new CUI_Interaction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Interaction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Interaction::Clone(void* pArg)
{
	CUI_Interaction* pInstance = new CUI_Interaction(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Interaction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Interaction::Free()
{
	CUI::Free();
}