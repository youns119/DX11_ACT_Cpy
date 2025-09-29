#include "pch.h"
#include "Terrain.h"

#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTerrain::CTerrain(const CTerrain& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC desc{};
	desc.TransformDesc.fSpeedPerSec = 5.f;

	if (FAILED(super::Initialize(&desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Locate(-64.f, 0.f, -64.f);

	return S_OK;
}

void CTerrain::Priority_Update(_float fTimeDelta)
{
	//m_pNavigationCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CTerrain::Update(_float fTimeDelta)
{
}

void CTerrain::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::BLEND, m_pNavigationCom);
#endif
}

HRESULT CTerrain::Render()
{
	HRESULT hr {};

	hr = Bind_ShaderResources();
	if (FAILED(hr))
		return E_FAIL;

	hr = m_pShaderCom->Begin(1); // 0 : default , 1 : brush
	if (FAILED(hr))
		return E_FAIL;

	hr = m_pVIBufferCom->Bind_InputAssembler();
	if (FAILED(hr))
		return E_FAIL;

	hr = m_pVIBufferCom->Render();
	if (FAILED(hr))
		return E_FAIL;



	return S_OK;
}

HRESULT CTerrain::Ready_Components()
{
	/* Com_Texture_Diffuse */
	if (FAILED(super::Add_Component(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture_Diffuse"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::DIFFUSE]))))
		return E_FAIL;

	/* Com_Texture_Mask*/
	if (FAILED(super::Add_Component(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Texture_Mask"),
		TEXT("Com_Texture_Mask"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::MASK]))))
		return E_FAIL;

	/* Com_Texture_Brush*/
	if (FAILED(super::Add_Component(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Texture_Brush"),
		TEXT("Com_Texture_Brush"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::BRUSH]))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(super::Add_Component(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(super::Add_Component(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	///* Com_Navigation */
	////if (FAILED(super::Add_Component(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Navigation"),
	//if (FAILED(super::Add_Component(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Navigation_TriangluationTest"),
	//	TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",
		m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",
		m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	if (FAILED(m_TextureComs[(_uint)TEXTURE_TYPE::DIFFUSE]->Bind_Textures_To_Shader(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_TextureComs[(_uint)TEXTURE_TYPE::MASK]->Bind_Texture_To_Shader(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;
	if (FAILED(m_TextureComs[(_uint)TEXTURE_TYPE::BRUSH]->Bind_Texture_To_Shader(m_pShaderCom, "g_BrushTexture")))
		return E_FAIL;

	//// forward shading
	//const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//	return E_FAIL;


	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//	return E_FAIL;

	////_float4 vCamPosition = m_pGameInstance->Get_CamPosition();
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;



	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);
	
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CTerrain::Free()
{
	super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pNavigationCom);

	for (auto& pTextureCom : m_TextureComs)
		Safe_Release(pTextureCom);
}
