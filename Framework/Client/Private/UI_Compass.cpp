#include "pch.h"
#include "UI_Compass.h"

#include "GameInstance.h"
#include "FPlayerStat.h"
#include "UI_CompassArrow.h"
#include "UI_Texture.h"

CUI_Compass::CUI_Compass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Compass::CUI_Compass(const CUI_Compass& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Compass::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Compass::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_pPlayerStat = pDesc->pPlayerStat;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	return S_OK;
}

void CUI_Compass::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_Compass::Update(_float fTimeDelta)
{
	if (m_bBroken)
	{
		m_pTransformCom->Turn(m_vAxis, fTimeDelta * 10.f);
	}
	else
	{
		_matrix matXZtoXY = XMMatrixIdentity();

		matXZtoXY.r[1] = XMVectorSet(0, 0, 1, 0);
		matXZtoXY.r[2] = XMVectorSet(0, 1, 0, 0); // m_vLocate.z¸¦ Áà¾ßµÇ³ª?

		_mat matViewInv = m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORM::VIEW);
		_vector vCamLook = matViewInv.look._vector();

		vCamLook = XMVector3Normalize(XMVectorSetY(vCamLook, 0.f));

		_vector vUILook = XMVector3TransformNormal(vCamLook, matXZtoXY);

		_float fUIRotation = atan2f(XMVectorGetX(vUILook), XMVectorGetY(vUILook));

		m_pTransformCom->Rotate(m_vAxis, fUIRotation);
	}

	super::Update(fTimeDelta);
}

void CUI_Compass::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);

	super::Late_Update(fTimeDelta);
}

HRESULT CUI_Compass::Render()
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

	_vector vUpDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::UP));
	_vector vRightDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::RIGHT));

	m_vTextPosition[DIR_EAST] = { m_vLocate.x + XMVectorGetX(vRightDir) * 45.f, m_vLocate.y - XMVectorGetY(vRightDir) * 45.f };
	m_vTextPosition[DIR_WEST] = { m_vLocate.x - XMVectorGetX(vRightDir) * 45.f, m_vLocate.y + XMVectorGetY(vRightDir) * 45.f };
	m_vTextPosition[DIR_SOUTH] = { m_vLocate.x - XMVectorGetX(vUpDir) * 45.f, m_vLocate.y + XMVectorGetY(vUpDir) * 45.f };
	m_vTextPosition[DIR_NORTH] = { m_vLocate.x + XMVectorGetX(vUpDir) * 45.f, m_vLocate.y - XMVectorGetY(vUpDir) * 45.f };

	for (_int i = 0; i < DIR_END; i++)
	{
		m_pGameInstance->Render_Font(L"Font_Cafe_10", m_szDirText[i], m_vTextPosition[i], XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 1.f, SORTING::MIDDLE);
	}

    return S_OK;
}

HRESULT CUI_Compass::Ready_PartObject()
{
	/* CompassArrow */
	CUI_CompassArrow::DESC CompassArrowDesc{};
	{
		CompassArrowDesc.UIDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		CompassArrowDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_CompassArrow.dat";
		CompassArrowDesc.pBroken = &m_bBroken;
	}
	if (FAILED(Add_PartObject(L"Part_UI_CompassArrow", LEVEL_STATIC, L"Prototype_GameObject_UI_CompassArrow", &CompassArrowDesc)))
		return E_FAIL;

	/* Texture */
	CUI_Texture::DESC TextureDesc{};
	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_CompassArrow2.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_CompassArrow2", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_Compass* CUI_Compass::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Compass* pInstance = new CUI_Compass(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Compass");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Compass::Clone(void* pArg)
{
	CUI_Compass* pInstance = new CUI_Compass(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Compass");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Compass::Free()
{
    CUI::Free();
}