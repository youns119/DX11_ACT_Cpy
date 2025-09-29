#include "pch.h"
#include "Item_HeartKelp.h"

#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"

#include "Player.h"

CItem_HeartKelp::CItem_HeartKelp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super{ pDevice, pContext }
{
}

CItem_HeartKelp::CItem_HeartKelp(const CItem_HeartKelp& Prototype)
    : super(Prototype)
{
}

HRESULT CItem_HeartKelp::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CItem_HeartKelp::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	if (FAILED(super::Initialize(&pDesc->ItemDesc)) ||
		FAILED(Ready_Components()))
		return E_FAIL;

    m_pOwner = pDesc->pOwner;

	return S_OK;
}

void CItem_HeartKelp::Priority_Update(_float fTimeDelta)
{
}

void CItem_HeartKelp::Update(_float fTimeDelta)
{
}

void CItem_HeartKelp::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
}

void CItem_HeartKelp::Final_Update(_float fTimeDelta)
{
    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (size_t i = 0; i < 3; ++i)
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));
}

HRESULT CItem_HeartKelp::Render()
{
	if (FAILED(Bind_ShaderResources()))
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

void CItem_HeartKelp::MoveToSocket(SOCKET eSocket)
{
	m_pSocketMatrix = m_pSocketMatrices[static_cast<_uint>(eSocket)];

	_float3 vScale = m_vTransforms[static_cast<_uint>(eSocket)][0];
	_float3 vRotation = m_vTransforms[static_cast<_uint>(eSocket)][1];
	_float3 vPosition = m_vTransforms[static_cast<_uint>(eSocket)][2];

	m_pTransformCom->Set_Scale(vScale);
	m_pTransformCom->Rotate(vRotation.x, vRotation.y, vRotation.z);
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat3(&vPosition));
}

void CItem_HeartKelp::Add_Socket(SOCKET eSocket, const _char* szFilePath)
{
	CModel* pModel = static_cast<CModel*>(m_pOwner->Find_Component(L"Com_Model"));

	Load_From_File(eSocket, szFilePath, pModel);
}

HRESULT CItem_HeartKelp::Load_From_File(SOCKET eSocket, const _char* szFilePath, CModel* pModel)
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

	_float3 vScale{}, vRotation{}, vPosition{};

	bResult = ReadFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
	bResult = ReadFile(hFile, &vRotation, sizeof(_float3), &dwByte, nullptr);
	bResult = ReadFile(hFile, &vPosition, sizeof(_float3), &dwByte, nullptr);

	m_vTransforms[static_cast<_uint>(eSocket)][0] = vScale;
	m_vTransforms[static_cast<_uint>(eSocket)][1] = vRotation;
	m_vTransforms[static_cast<_uint>(eSocket)][2] = vPosition;

	CloseHandle(hFile);

	if (!strcmp(szSocketBone, "NoSocket"))
	{
		_float4x4 matSocket{};
		XMStoreFloat4x4(&matSocket, XMMatrixIdentity());

		m_pSocketMatrices[static_cast<_uint>(eSocket)] = &matSocket;
	}
	else
		m_pSocketMatrices[static_cast<_uint>(eSocket)] = &pModel->Get_BoneCombinedMatrix_By_Name(szSocketBone);

	return S_OK;
}

HRESULT CItem_HeartKelp::Ready_Components()
{
	// Com_Model
	if (FAILED(Add_Component
	(
		LEVEL_STATIC,
		L"Prototype_Component_Model_Item_HeartKelp",
		L"Com_Model",
		reinterpret_cast<CComponent**>(&m_pModelCom)
	)))
		return E_FAIL;

	// Com_Shader
	if (FAILED(super::Add_Component
	(
		LEVEL_STATIC,
		TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"),
		reinterpret_cast<CComponent**>(&m_pShaderCom)
	)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_HeartKelp::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	return S_OK;
}

CItem_HeartKelp* CItem_HeartKelp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_HeartKelp* pInstance = new CItem_HeartKelp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CItem_HeartKelp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CItem_HeartKelp::Clone(void* pArg)
{
	CItem_HeartKelp* pInstance = new CItem_HeartKelp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CItem_HeartKelp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_HeartKelp::Free()
{
	super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}