#include "pch.h"
#include "Dummy_NonAnim.h"

#include "GameInstance.h"

CDummy_NonAnim::CDummy_NonAnim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CDummy_NonAnim::CDummy_NonAnim(const CDummy_NonAnim& Prototype)
	: super(Prototype)
{
}

HRESULT CDummy_NonAnim::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummy_NonAnim::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;

	if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc->strModelTag, pDesc->eLevelID)))
		return E_FAIL;

	return S_OK;
}

void CDummy_NonAnim::Priority_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(KEY::Z))
	{
		m_pPhysicsCom->Set_Kinematic(true);
		//m_pPhysicsCom->Remove_From_Scene();
	}
	if (m_pGameInstance->Key_Down(KEY::X))
	{
		m_pPhysicsCom->Set_Kinematic(false);
		//m_pPhysicsCom->Add_To_Scene();
	}
}

void CDummy_NonAnim::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(KEY::C))
		m_pPhysicsCom->Add_Torque(_vec3(0.f, 0.f, -100.f));

	if (m_pGameInstance->Key_Down(KEY::O))
		m_pPhysicsCom->Add_To_Scene();
	if (m_pGameInstance->Key_Down(KEY::P))
		m_pPhysicsCom->Remove_From_Scene();

	/* 꼭 조작 안해도 괜찮아 */
	m_pPhysicsCom->Manipulate_Physics(m_pTransformCom->Get_WorldMatrix());

	//m_pGameInstance->Add_DebugRender_Box(_mat(), {1.f, 0.f, 1.f, 1.f});
}

void CDummy_NonAnim::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);
#ifdef _DEBUG
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pPhysicsCom);
#endif

	m_pGameInstance->Simulate_Physics(m_pPhysicsCom);
}

HRESULT CDummy_NonAnim::Render()
{
	if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID")))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
	{
		_uint iMaterialIndex = m_pModelCom->Get_MaterialIndex_In_Mesh(i);
		_uint iShaderPass = m_pModelCom->Get_ShaderPass(iMaterialIndex);

		if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE))
		{
			iShaderPass = (_uint)SHADER_PASS_MESH::DEFAULT;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else
		{
			iShaderPass = (_uint)SHADER_PASS_MESH::BASE_COLOR;

			if (FAILED(m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor")))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::NORMAL, "g_NormalTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::METALNESS, "g_MetalicTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::SHININESS, "g_SmoothnessTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::EMISSIVE, "g_EmissiveMaskTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_EmissionColor(m_pShaderCom, i, "g_vEmissionColor")))
			return E_FAIL;

		m_pShaderCom->Begin(iShaderPass);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CDummy_NonAnim::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix_Ref())))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_ShadowMatrices_To_Shader(m_pShaderCom, "g_LightViewMatrices", "g_LightProjMatrices")))
		return E_FAIL;

	for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
	{
		m_pShaderCom->Begin((_uint)SHADER_PASS_MESH::SHADOW);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CDummy_NonAnim::Ready_Components(const _wstring& strModelTag, LEVELID eLevelID)
{
	/* Com_Shader */
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxMesh",
		L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(Add_Component(eLevelID, strModelTag,
		L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Physics */
	//if (FAILED(Ready_PhysicsSphere()))
	//	return E_FAIL;
	//if (FAILED(Ready_PhysicsMesh()))
	//	return E_FAIL;
	if (FAILED(Ready_PhysicsConvex()))
		return E_FAIL;
	return S_OK;
}

HRESULT CDummy_NonAnim::Ready_PhysicsMesh()
{
	CPhysics_Mesh::DESC Desc {};
	Desc.PhysicsDesc.fStaticFriction = 0.8f;
	Desc.PhysicsDesc.fDynamicFriction = 0.7f;
	Desc.PhysicsDesc.fRestitution = 0.2f;
	Desc.PhysicsDesc.isStatic = true;
	Desc.PhysicsDesc.iFilter = (_uint)PHYSICS_FILTER::DEFAULT;
	Desc.PhysicsDesc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());

	Desc.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	Desc.isInChunk = true;

	const size_t iPrefixLen = wcslen(L"Prototype_Component_Model_");
	const _wstring& strPhysicsPrototypeTag = L"Prototype_Component_Physics_Mesh_" + m_strName.substr(iPrefixLen);

	if (FAILED(Add_Component(LEVEL_STATIC, strPhysicsPrototypeTag,
		L"Com_Physics", reinterpret_cast<CComponent**>(&m_pPhysicsCom), &Desc)))
		return E_FAIL;

    return S_OK;
}

HRESULT CDummy_NonAnim::Ready_PhysicsSphere()
{
	CPhysics_Sphere::DESC Desc {};

	Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	Desc.PhysicsDesc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());
	Desc.PhysicsDesc.isStatic = false;
	Desc.PhysicsDesc.iFilter = (_uint)PHYSICS_FILTER::DEFAULT;
	Desc.fRadius = 0.5f;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Physics_Sphere",
		L"Com_Physics", reinterpret_cast<CComponent**>(&m_pPhysicsCom), &Desc)))
		return E_FAIL;

	m_pPhysicsCom->Remove_From_Scene();
	return S_OK;
}

HRESULT CDummy_NonAnim::Ready_PhysicsConvex()
{
	CPhysics_Convex::DESC Desc {};
	// 좋은 방법 있으면 알려주기
	Desc.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	Desc.PhysicsDesc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());
	Desc.PhysicsDesc.isStatic = false;
	Desc.PhysicsDesc.iFilter = (_uint)PHYSICS_FILTER::SHELL;
	Desc.PhysicsDesc.fDynamicFriction = 0.7f;
	Desc.PhysicsDesc.fStaticFriction = 0.8f;
	Desc.PhysicsDesc.fMass = 0.5f;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Physics_Convex_Shell_MirrorBall",
		L"Com_Physics", reinterpret_cast<CComponent**>(&m_pPhysicsCom), &Desc)))
		return E_FAIL;

	// initailize dptj tlrksdmf aksgdl Tmrl Eoansdp durltj gkdtkd ekdlskalrdl dnawlrdlwl dksgehfhr godigogksek.
	m_pPhysicsCom->Set_Kinematic(true);
	return S_OK;
}

CDummy_NonAnim* CDummy_NonAnim::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_NonAnim* pInstance = new CDummy_NonAnim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CDummy_NonAnim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CDummy_NonAnim::Clone(void* pArg)
{
	CDummy_NonAnim* pInstance = new CDummy_NonAnim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CDummy_NonAnim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy_NonAnim::Free()
{
	super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pPhysicsCom);
}
