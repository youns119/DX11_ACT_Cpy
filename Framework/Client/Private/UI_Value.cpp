#include "pch.h"
#include "UI_Value.h"

#include "GameInstance.h"

#include <sstream>
#include <locale>

CUI_Value::CUI_Value(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Value::CUI_Value(const CUI_Value& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Value::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Value::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	
	m_pCount = pDesc->pCount;

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	return S_OK;
}

void CUI_Value::Priority_Update(_float fTimeDelta)
{
}

void CUI_Value::Update(_float fTimeDelta)
{
}

void CUI_Value::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
	}
}

HRESULT CUI_Value::Render()
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

	if (nullptr != m_pCount)
		Format_With_Comma();

	_float2 vTextPos = { m_vLocate.x + m_vTextOffset.x, m_vLocate.y + m_vTextOffset.y };
	
	m_pGameInstance->Render_Font(m_strFontTag, m_szText, vTextPos, XMLoadFloat4(&m_vTextColor), 0.f, {0.f, 0.f}, m_fTextScale, SORTING::RIGHT);

	return S_OK;
}

void CUI_Value::Format_With_Comma()
{
	wstringstream ws;
	ws.imbue(locale("en_US.UTF-8"));  // 미국 영어 로케일
	ws << *m_pCount;

	wstring result = ws.str();

	wcscpy_s(m_szText, result.c_str());
}

CUI_Value* CUI_Value::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Value* pInstance = new CUI_Value(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Value");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Value::Clone(void* pArg)
{
	CUI_Value* pInstance = new CUI_Value(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Value");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Value::Free()
{
	CUI::Free();
}