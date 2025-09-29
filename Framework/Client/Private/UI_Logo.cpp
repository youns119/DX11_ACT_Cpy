#include "pch.h"
#include "UI_Logo.h"

#include "GameInstance.h"

CUI_Logo::CUI_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Logo::CUI_Logo(const CUI_Logo& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Logo::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Logo::Initialize(void* pArg)
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

void CUI_Logo::Priority_Update(_float fTimeDelta)
{
}

void CUI_Logo::Update(_float fTimeDelta)
{
	// µÕ½Ç°Å¸®±â
	m_fFrame += 2.0f * fTimeDelta;

	_vector Origin = (m_pTransformCom->Get_State(CTransform::STATE::POSITION));
	Origin = XMVectorSetY(Origin, XMVectorGetY(Origin) + sin(m_fFrame) / 8.f);

	m_pTransformCom->Set_State(CTransform::STATE::POSITION, Origin);
}

void CUI_Logo::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_Logo::Render()
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

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

    return S_OK;
}

CUI_Logo* CUI_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Logo* pInstance = new CUI_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Logo::Clone(void* pArg)
{
	CUI_Logo* pInstance = new CUI_Logo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Logo::Free()
{
    CUI::Free();
}
