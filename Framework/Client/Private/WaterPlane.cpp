#include "pch.h"
#include "WaterPlane.h"

#include "GameInstance.h"

CWaterPlane::CWaterPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CWaterPlane::CWaterPlane(const CWaterPlane& Prototype)
	: super(Prototype)
{
}

HRESULT CWaterPlane::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWaterPlane::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;

	if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_vec3(200.f, 200.f, 1.f)._float3());
	m_pTransformCom->Rotate(90.f, 0.f, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, _vec4(0.f, 1.f, 0.f, 1.f)._vector());

	return S_OK;
}

void CWaterPlane::Priority_Update(_float fTimeDelta)
{
}

void CWaterPlane::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	_float fRotation = CUtils::Lerp_Float(90.f, 90.3f, sinf(m_fTimeAcc * XM_2PI * 0.2f));

	m_pTransformCom->Rotate(fRotation, 0.f, 0.f);

	_mat WorldMatrixInv = m_pTransformCom->Get_WorldMatrix_Inversed();

	_vec3 vCamPos = _vec3(m_pGameInstance->Get_CamPosition());
	_vec3 vCamPosInWaterSpace = vCamPos.TransformCoord(WorldMatrixInv);

	if (vCamPosInWaterSpace.z >= 0.f)
	{
		FOG_PARAMS FogParams = m_pGameInstance->Get_FogParams();
		FogParams.vRange = _vec2(0.f, 10.f);
		FogParams.vFogColor = _vec4(70.f, 138.f, 200.f, 255.f) / 255.f;
		m_pGameInstance->Set_FogParams(FogParams);
	}
	else
	{
		FOG_PARAMS FogParams = m_pGameInstance->Get_FogParams();
		FogParams.vRange = _vec2(1000.f, 1000.f);
		m_pGameInstance->Set_FogParams(FogParams);
	}

#ifdef _DEBUG
	ImGui::Begin("Water Plane");

	ImGui::DragFloat2("UV Tile size", (_float*)& m_DebugParams.vTileSize, 0.05f, 0.1f, 50.f);
	ImGui::DragFloat2("Panning Speed", (_float*)& m_DebugParams.vPanndingSpeed, 0.001f, 0.0f, 1.f);
	ImGui::DragFloat("Distortion", &m_DebugParams.fDistortionIntensity, 0.0001f, 0.f);
	ImGui::DragFloat("Light Power", &m_DebugParams.fLightPower, 0.005f, 0.f);
	ImGui::DragFloat("Light Intensity", &m_DebugParams.fLightIntensity, 0.001f, 0.f);

	ImGui::End();
#endif // _DEBUG
}

void CWaterPlane::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::BLEND, this);
}

HRESULT CWaterPlane::Render()
{
	//if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID")))
	//    return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_Depth", m_pShaderCom, "g_DepthTexture")))
		return E_FAIL;

	/* Texture */
	if (FAILED(m_pTextureCom_Sky->Bind_Texture_To_Shader(m_pShaderCom, "g_SkyTexture")))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_Texture")))
		return E_FAIL;
	if (FAILED(m_pTextureCom_Normal->Bind_Texture_To_Shader(m_pShaderCom, "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pTextureCom_Distortion->Bind_Texture_To_Shader(m_pShaderCom, "g_DistortionTexture")))
		return E_FAIL;

	/* Matrices*/
	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->
		Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->
		Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->
		Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->
		Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	/* Cam Position */
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_DebugParams", &m_DebugParams, sizeof(DEBUG_PARAMS))))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_InputAssembler();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CWaterPlane::Ready_Component()
{
	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Rect",
		L"Com_VIBuffer", reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Texture_WaterShadow",
		L"Com_Texture", reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	/* Com_Texture_Sky */
	if (FAILED(super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
		TEXT("Com_Texture_Sky"), reinterpret_cast<CComponent**>(&m_pTextureCom_Sky))))
		return E_FAIL;

	/* Com_Texture_Normal */
	if (FAILED(Add_Component(LEVEL_BEACH, L"Prototype_Component_Texture_WaterNormal",
		L"Com_Texture_Normal", reinterpret_cast<CComponent**>(&m_pTextureCom_Normal))))
		return E_FAIL;

	/* Com_Texture_Distortion */
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Texture_WaterCausticsDistortion",
		L"Com_Texture_Distortion", reinterpret_cast<CComponent**>(&m_pTextureCom_Distortion))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(Add_Component(LEVEL_BEACH, L"Prototype_Component_Shader_WaterPlane",
		L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

CWaterPlane* CWaterPlane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWaterPlane* pInstance = new CWaterPlane(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Craete : CWaterPlane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CWaterPlane::Clone(void* pArg)
{
	CWaterPlane* pInstance = new CWaterPlane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CWaterPlane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaterPlane::Free()
{
	super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCom_Sky);
	Safe_Release(m_pTextureCom_Distortion);
	Safe_Release(m_pTextureCom_Normal);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
