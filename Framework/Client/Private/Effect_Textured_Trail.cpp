#include "pch.h"
#include "Effect_Textured_Trail.h"

#include "GameInstance.h"


CEffect_Textured_Trail::CEffect_Textured_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CEffect_Textured_Trail::CEffect_Textured_Trail(const CEffect_Textured_Trail& Prototype)
	: super(Prototype)
{
}

HRESULT CEffect_Textured_Trail::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Textured_Trail::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)) || FAILED(Ready_Components()))
		return E_FAIL;

	_vector vLocalPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, vLocalPos);

	m_fWidth = 0.175f;

	return S_OK;
}

void CEffect_Textured_Trail::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Textured_Trail::Update(_float fTimeDelta)
{
	
}

void CEffect_Textured_Trail::Late_Update(_float fTimeDelta)
{
}

void CEffect_Textured_Trail::Final_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMMatrixTranslation(m_pParentWorldMatrix->_41, m_pParentWorldMatrix->_42, m_pParentWorldMatrix->_43));
	
	_vector vCamPos = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());
	_vector vWorldPos = _mat(m_CombinedWorldMatrix).position._vector();
	m_fCamDistance = XMVectorGetX(XMVector3Length(vCamPos - vWorldPos));

	m_pBufferCom->Progress(fTimeDelta);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::EFFECTBLEND, this);
}

HRESULT CEffect_Textured_Trail::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pBufferCom->Bind_InputAssembler();
	m_pBufferCom->Render();
	return S_OK;
}

HRESULT CEffect_Textured_Trail::Clear_Info()
{
	m_pBufferCom->Reset_Life();
	return S_OK;
}

HRESULT CEffect_Textured_Trail::Ready_Components()
{
	// Com_Shader
	if (FAILED(super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTrail"),
		TEXT("Com_Shader"),	reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CVIBuffer_Trail::DESC Desc{};
	Desc.pWorldMatrix = &m_CombinedWorldMatrix;
	Desc.vLifeTime = _float2(0.5f, 0.f);
	Desc.fSize = 0.f;
	Desc.fInterval = 0.25f;
	Desc.iMaxVtxNum = 128;

	/* Com_Texture */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Tether"), 
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_ColorTexture */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TetherColor"), 
		TEXT("Com_ColorTexture"), reinterpret_cast<CComponent**>(&m_pColorTextureCom))))
		return E_FAIL;

	// Com_VIBuffer
	if (FAILED(super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pBufferCom), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Textured_Trail::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;	
	
	if (FAILED(m_pColorTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_ColorTexture", 0)))
		return E_FAIL;

	// 카메라 포지션 바인딩
	m_vCamPosition = m_pGameInstance->Get_CamPosition();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_vCamPosition, sizeof(_float4))))
		return E_FAIL;

	// 너비 바인딩
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fWidth", &m_fWidth, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CEffect_Textured_Trail* CEffect_Textured_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Textured_Trail* pInstance = new CEffect_Textured_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CEffect_Textured_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CEffect_Textured_Trail::Clone(void* pArg)
{
	CEffect_Textured_Trail* pInstance = new CEffect_Textured_Trail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CEffect_Textured_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Textured_Trail::Free()
{
	super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColorTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);
}