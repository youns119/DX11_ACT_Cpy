#include "pch.h"
#include "ToolPartObject.h"

#include "GameInstance.h"
#include "Collider.h"

CToolPartObject::CToolPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CToolPartObject::CToolPartObject(const CToolPartObject& Prototype)
	: super(Prototype)
{
}

HRESULT CToolPartObject::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolPartObject::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	if (pDesc == nullptr)
	{
		MSG_BOX("Failed To Create : CWeapon_Player");
		return E_FAIL;
	}
	else
		m_pSocketMatrix = pDesc->pSocketMatrix;

	if (FAILED(super::Initialize(&pDesc->PartObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CToolPartObject::Priority_Update(_float fTimeDelta)
{
}

void CToolPartObject::Update(_float fTimeDelta)
{
	_matrix SocketMatrix{};

	if (m_pSocketMatrix == nullptr)
		SocketMatrix = XMMatrixIdentity();
	else SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CToolPartObject::Late_Update(_float fTimeDelta)
{
	if (m_bShow)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	//m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif
}

HRESULT CToolPartObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = static_cast<_uint>(m_pModelCom->Get_NumMeshes());

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iShaderPass = 0;

		m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE);

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

void CToolPartObject::Set_SocketBone(const _float4x4* _pSocketMatrix)
{
	m_pSocketMatrix = _pSocketMatrix;
}

HRESULT CToolPartObject::Change_Model(const _wstring& _strModelTag, LEVELID _eLevelID)
{
	Delete_Component(L"Model", reinterpret_cast<CComponent**>(&m_pModelCom));
	Safe_Release(m_pModelCom);

	// Model
	CModel::DESC tModelDesc{};
	tModelDesc.pOwner = this;

	if (FAILED(Add_Component
	(
		_eLevelID,
		_strModelTag,
		L"Model",
		reinterpret_cast<CComponent**>(&m_pModelCom),
		&tModelDesc
	)))
		return E_FAIL;

	m_pSocketMatrix = nullptr;

	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
	m_pTransformCom->Rotate(0.f, 0.f, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	return S_OK;
}

HRESULT CToolPartObject::Ready_Components()
{
	// Com_Model
	if (FAILED(Add_Component(LEVEL_TOOL, L"Model_Fork",
		L"Model", reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	// Com_Shader
	if (FAILED(super::Add_Component(LEVEL_STATIC, TEXT("Shader_VtxMesh"),
		TEXT("Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// Com_Collider
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;

		Desc.fRadius = 1.f;
		Desc.vCenter = { 0.f, 1.f, 0.f };

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CToolPartObject::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	return S_OK;
}

CToolPartObject* CToolPartObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolPartObject* pInstance = new CToolPartObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CToolPartObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CToolPartObject::Clone(void* pArg)
{
	CToolPartObject* pInstance = new CToolPartObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CToolPartObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolPartObject::Free()
{
	super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}