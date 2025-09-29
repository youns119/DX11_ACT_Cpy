#include "pch.h"
#include "Roland_Wrench_2_R.h"
#include "GameInstance.h"
#include "Collider.h"

#include "Boss_Roland.h"
#include "Player.h"

CRoland_Wrench_2_R::CRoland_Wrench_2_R(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CRoland_Wrench_2_R::CRoland_Wrench_2_R(const CRoland_Wrench_2_R& Prototype)
	: super(Prototype)
{
}

HRESULT CRoland_Wrench_2_R::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRoland_Wrench_2_R::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	if (pDesc == nullptr)
	{
		MSG_BOX("Failed To Create : CRoland_Wrench_2_R");
		return E_FAIL;
	}
	else {
		m_fDamage = pDesc->fDamage;
		m_pOwner = pDesc->pOwner;
	}

	if (FAILED(super::Initialize(&pDesc->PartObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pDesc->pSocketMatrix != nullptr)
		m_pSocketMatrix = pDesc->pSocketMatrix;
	if (pDesc->szFilePath != nullptr)
		Load_From_File(pDesc->szFilePath, pDesc->pModel);	

	m_isActive = false;

	return S_OK;
}

void CRoland_Wrench_2_R::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CRoland_Wrench_2_R::Update(_float fTimeDelta)
{
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

	__super::Update(fTimeDelta);
}

void CRoland_Wrench_2_R::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif // _DEBUG	
}

HRESULT CRoland_Wrench_2_R::Render()
{
	if (!m_isActive)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
	{
		_uint iShaderPass = 0;
		if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE))
		{
			iShaderPass = (_uint)SHADER_PASS_ANIMMESH::DEFAULT;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else
		{
			iShaderPass = (_uint)SHADER_PASS_ANIMMESH::BASE_COLOR;

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

void CRoland_Wrench_2_R::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr)
		pPlayer->Damaged(m_fDamage, IDamaged::DAMAGE::DAMAGE_NORMAL, m_pOwner);
}

void CRoland_Wrench_2_R::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CRoland_Wrench_2_R::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

void CRoland_Wrench_2_R::Parried()
{
	dynamic_cast<IParried*>(m_pOwner)->Parried();
}

HRESULT CRoland_Wrench_2_R::Load_From_File(const _char* szFilePath, CModel* pModel)
{
	_string strLoadFile = _string(szFilePath);
	_wstring strFilePath = CUtils::ConvertToWideChar(strLoadFile);

	_bool bResult{};
	_ulong dwByte{};
	HANDLE hFile{};
	hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed To Load PartObject Data");
		return E_FAIL;
	}

	_char szSocketBone[MAX_PATH]{};

	bResult = ReadFile(hFile, &szSocketBone, sizeof(_char) * MAX_PATH, &dwByte, nullptr);

	_float3 vScale{};
	_float3 vRotation{};
	_float3 vTranslation{};

	bResult = ReadFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
	bResult = ReadFile(hFile, &vRotation, sizeof(_float3), &dwByte, nullptr);
	bResult = ReadFile(hFile, &vTranslation, sizeof(_float3), &dwByte, nullptr);

	CloseHandle(hFile);

	if (!strcmp(szSocketBone, "NoSocket"))
	{
		_float4x4 matSocket{};
		XMStoreFloat4x4(&matSocket, XMMatrixIdentity());

		m_pSocketMatrix = &matSocket;
	}
	else
		m_pSocketMatrix = &pModel->Get_BoneCombinedMatrix_By_Name(szSocketBone);

	m_pTransformCom->Set_Scale(vScale);
	m_pTransformCom->Rotate(vRotation.x, vRotation.y, vRotation.z);
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat3(&vTranslation));

	return S_OK;
}

HRESULT CRoland_Wrench_2_R::Ready_Components()
{
	// Com_Model
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Roland_Wrench_2_R",
		L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	// Com_Shader
	if (FAILED(super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// Com_Collider
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = static_cast<_uint>(COLLISION_GROUP::MONSTER_HIT);

		Desc.fRadius = 1.f;
		Desc.vCenter = { 0.f, 1.5f, 0.f };

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
	}

	return S_OK;
}


HRESULT CRoland_Wrench_2_R::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	return S_OK;
}

CRoland_Wrench_2_R* CRoland_Wrench_2_R::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRoland_Wrench_2_R* pInstance = new CRoland_Wrench_2_R(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CRoland_Wrench_2_R");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CRoland_Wrench_2_R::Clone(void* pArg)
{
	CRoland_Wrench_2_R* pInstance = new CRoland_Wrench_2_R(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CRoland_Wrench_2_R");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Wrench_2_R::Free()
{
	super::Free();	

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
