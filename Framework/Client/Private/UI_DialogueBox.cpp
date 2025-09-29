#include "pch.h"
#include "UI_DialogueBox.h"

#include "GameInstance.h"

CUI_DialogueBox::CUI_DialogueBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_DialogueBox::CUI_DialogueBox(const CUI_DialogueBox& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_DialogueBox::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_DialogueBox::Initialize(void* pArg)
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

void CUI_DialogueBox::Priority_Update(_float fTimeDelta)
{
}

void CUI_DialogueBox::Update(_float fTimeDelta)
{
}

void CUI_DialogueBox::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
	}
}

HRESULT CUI_DialogueBox::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fStretchFactor", &m_fStretchFactor, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	if (m_vecText.size() == 1)
	{
		_float2 vTextPos = { m_vLocate.x + m_vTextOffset.x, m_vLocate.y + m_vTextOffset.y };

		m_pGameInstance->Render_Font(m_strFontTag, m_vecText[0].c_str(), vTextPos, XMLoadFloat4(&m_vTextColor), 0.f, {0.f, 0.f}, m_fTextScale, m_eSorting);
	}
	else
	{
		_float2 vTextPos = { m_vLocate.x + m_vTextOffset.x, m_vLocate.y + m_vTextOffset.y - 13.f};
		m_pGameInstance->Render_Font(m_strFontTag, m_vecText[0].c_str(), vTextPos, XMLoadFloat4(&m_vTextColor), 0.f, { 0.f, 0.f }, m_fTextScale, m_eSorting);

		vTextPos = { m_vLocate.x + m_vTextOffset.x, m_vLocate.y + m_vTextOffset.y + 13.f };
		m_pGameInstance->Render_Font(m_strFontTag, m_vecText[1].c_str(), vTextPos, XMLoadFloat4(&m_vTextColor), 0.f, { 0.f, 0.f }, m_fTextScale, m_eSorting);
	}

	return S_OK;
}

void CUI_DialogueBox::Set_DialogueText(const _tchar* szDescription)
{
	m_vecText.clear();

	_int len = (_int)wcslen(szDescription);
	_int index = 0;

	while (index < len)
	{
		size_t chunkSize = min(20, len - index);
		m_vecText.push_back(_wstring(szDescription + index, chunkSize));
		index += (_int)chunkSize;
	}
}

CUI_DialogueBox* CUI_DialogueBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_DialogueBox* pInstance = new CUI_DialogueBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Count");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_DialogueBox::Clone(void* pArg)
{
	CUI_DialogueBox* pInstance = new CUI_DialogueBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Count");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_DialogueBox::Free()
{
	CUI::Free();
}