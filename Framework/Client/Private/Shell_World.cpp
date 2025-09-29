#include "pch.h"
#include "Shell_World.h"

#include "GameInstance.h"
#include "Collider.h"

#include "Player.h"
#include "UI_Manager.h"
#include "UI_ShellDesc.h"

CShell_World::CShell_World(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CShell_World::CShell_World(const CShell_World& Prototype)
	: super(Prototype)
{
}

HRESULT CShell_World::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShell_World::Initialize(void* pArg)
{
	CShell_Item::DESC * pDesc = static_cast<CShell_Item::DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	return S_OK;
}

void CShell_World::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CShell_World::Update(_float fTimeDelta)
{
	if (m_bGrappled)
	{
		m_fLerp += fTimeDelta * 2.5f;
		if (m_fLerp >= 1.f)
			m_fLerp = 1.f;

		_float fLerp = sinf(XMConvertToRadians(90.f * m_fLerp));
		_matrix matTransform = m_pTransformCom->Get_WorldMatrix();

		_vector vScale{}, vRotation{}, vTranslation{};
		XMMatrixDecompose(&vScale, &vRotation, &vTranslation, matTransform);

		_vector vLerpTranslation = XMVectorLerp(XMLoadFloat3(&m_vSrcPos), XMLoadFloat3(&m_vDstPos), fLerp);
		_matrix matLerp = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vLerpTranslation);

		m_pTransformCom->Locate(_vec3(vLerpTranslation)._float3());
		m_pPhysicsCom->Manipulate_Physics(matLerp);
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	super::Update(fTimeDelta);
}

void CShell_World::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG

	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pPhysicsCom);

#endif

	m_pGameInstance->Simulate_Physics(m_pPhysicsCom);
	super::Late_Update(fTimeDelta);
}

void CShell_World::Final_Update(_float fTimeDelta)
{
	//XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	super::Final_Update(fTimeDelta);
}

HRESULT CShell_World::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrix_Ref())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	_uint iNumMeshes = static_cast<_uint>(m_pModelCom->Get_NumMeshes());

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iShaderPass = 0;
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

HRESULT CShell_World::Render_Shadow()
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

_vec3 CShell_World::Get_GrapplePos()
{
	_vec3 vPos = m_pColliderCom->Get_BoundingCenter();

	return vPos;
}

_uint CShell_World::Get_Type()
{
	_uint iType = m_pColliderCom->Get_CollisionGroup();

	return iType;
}

void CShell_World::Shell_Launch(_vec3 vSrc, _vec3 vDst)
{
	m_bStop = false;
	m_bGrappled = true;
	m_fLerp = 0.f;
	XMStoreFloat3(&m_vSrcPos, vSrc._vector());
	XMStoreFloat3(&m_vDstPos, vDst._vector());

	m_pPhysicsCom->Set_Kinematic(false);
}

void CShell_World::UnLaunch()
{
	m_bStop = false;
	m_bGrappled = false;
	m_fLerp = 0.f;
	m_vSrcPos = _float3(0.f, 0.f, 0.f);
	m_vDstPos = _float3(0.f, 0.f, 0.f);

	m_pPhysicsCom->Set_Kinematic(false);
}

void CShell_World::Stop()
{
	m_bStop = true;

	m_pPhysicsCom->Set_Kinematic(true);
}

void CShell_World::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	switch (iGroup)
	{
	case (_uint)COLLISION_GROUP::PLAYER :
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(pOther);

		if (!pPlayer->Get_PlayerStat()->bGrappling)
		{
			CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

			if (pPlayer->Get_PlayerStat()->bHasShell)
				pUIManager->Show_Interaction(L"µî²®Áú ±³Ã¼");
			else pUIManager->Show_Interaction(L"µî²®Áú Âø¿ë");
		}
		else
		{

		}
	}
	break;
	default :
		break;
	}

	m_pUIShellDesc->Set_IsActive(true);
}

void CShell_World::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CShell_World::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
	pUIManager->Hide_Interaction();

	m_pUIShellDesc->Set_IsActive(false);
}

HRESULT CShell_World::Ready_Components()
{
	if(FAILED(super::Ready_Components()))
		return E_FAIL;

	// Com_Collider
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = static_cast<_uint>(COLLISION_GROUP::SHELL);

		Desc.fRadius = m_tShellData.fColliderRadius;
		Desc.vCenter = m_tShellData.vColliderCenter;

		if (FAILED(Add_Component
		(
			LEVEL_STATIC,
			TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider",
			reinterpret_cast<CComponent**>(&m_pColliderCom), 
			&Desc
		)))
			return E_FAIL;
	}

	// Com_Physics
	{
		CPhysics_Convex::DESC Desc {};
		Desc.isInChunk = false;
		Desc.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		Desc.PhysicsDesc.fRestitution = 0.f;

		Desc.PhysicsDesc.isStatic = false;
		Desc.PhysicsDesc.iFilter = static_cast<_uint>(PHYSICS_FILTER::SHELL);
		Desc.PhysicsDesc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());
		Desc.PhysicsDesc.fDynamicFriction = 0.8f;
		Desc.PhysicsDesc.fStaticFriction = 0.9f;
		Desc.PhysicsDesc.fMass = 5.5f;
		Desc.PhysicsDesc.InitialForce = _vec3 {0.f, 0.f, -10.f}; // dlatl zhem skwnddp tnwjd dPwjd

		size_t iPrefixSize = wcslen(L"Prototype_Component_Model_");
		_wstring strPrototypeTag = L"Prototype_Component_Physics_Convex_" + m_tShellData.strModel.substr(iPrefixSize);

		if (FAILED(Add_Component
		(
			LEVEL_STATIC,
			strPrototypeTag,
			L"Com_Physics",
			reinterpret_cast<CComponent**>(&m_pPhysicsCom),
			&Desc
		)))
			return E_FAIL;

		//m_pPhysicsCom->Set_Kinematic(true);
	}

	return S_OK;
}

HRESULT CShell_World::Ready_PartObject()
{
	CUI_ShellDesc::DESC ShellDesc{};
	{
		ShellDesc.pShell = this;
		ShellDesc.pShellStat = &m_tShellStat;
	}
	if (FAILED(Add_PartObject(L"Part_UI_ShellDesc", LEVEL_STATIC, L"Prototype_GameObject_UI_ShellDesc", &ShellDesc)))
		return E_FAIL;

	m_pUIShellDesc = static_cast<CUI_ShellDesc*>(Find_PartObject({ L"Part_UI_ShellDesc" }));

	return S_OK;
}

CShell_World* CShell_World::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShell_World* pInstance = new CShell_World(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CShell_World");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CShell_World::Clone(void* pArg)
{
	CShell_World* pInstance = new CShell_World(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CShell_World");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShell_World::Free()
{
	super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pPhysicsCom);
}