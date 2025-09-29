#include "pch.h"
#include "UI_CompassArrow.h"

#include "GameInstance.h"

CUI_CompassArrow::CUI_CompassArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_CompassArrow::CUI_CompassArrow(const CUI_CompassArrow& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_CompassArrow::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CompassArrow::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_pBroken = pDesc->pBroken;

	return S_OK;
}

void CUI_CompassArrow::Priority_Update(_float fTimeDelta)
{
}

void CUI_CompassArrow::Update(_float fTimeDelta)
{
	if (*m_pBroken)
	{
		m_pTransformCom->Turn(m_vAxis, fTimeDelta * 15.f);
	}
	else
	{
		_matrix matParentWorld = XMLoadFloat4x4(m_pParentWorldMatrix);
		_matrix matWorld = m_pTransformCom->Get_WorldMatrix();

		_vector vParentScale{}, vParentRotation{}, vParentTranslation{};
		_vector vScale{}, vRotation{}, vTranslation{};

		XMMatrixDecompose(&vParentScale, &vParentRotation, &vParentTranslation, matParentWorld);
		XMMatrixDecompose(&vScale, &vRotation, &vTranslation, matWorld);

		_vector vLerpRotation = XMQuaternionSlerp(vRotation, vParentRotation, m_fRotationSpeed * fTimeDelta);

		_matrix matLerp =
			XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vLerpRotation, vTranslation);

		m_pTransformCom->Set_State(CTransform::STATE::RIGHT, matLerp.r[0]);
		m_pTransformCom->Set_State(CTransform::STATE::UP, matLerp.r[1]);
		m_pTransformCom->Set_State(CTransform::STATE::LOOK, matLerp.r[2]);
	}
}

void CUI_CompassArrow::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_CompassArrow::Render()
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

CUI_CompassArrow* CUI_CompassArrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CompassArrow* pInstance = new CUI_CompassArrow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CompassArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_CompassArrow::Clone(void* pArg)
{
	CUI_CompassArrow* pInstance = new CUI_CompassArrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CompassArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_CompassArrow::Free()
{
	CUI::Free();
}