#include "VIBuffer_Instancing_Mesh_SRT.h"
#include "GameInstance.h"
#include "Shader_Compute.h"

CVIBuffer_Instancing_Mesh_SRT::CVIBuffer_Instancing_Mesh_SRT(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instancing_Mesh_SRT::CVIBuffer_Instancing_Mesh_SRT(const CVIBuffer_Instancing_Mesh_SRT& Prototype)
	: super(Prototype)
	, m_InstanceBufferDesc{ Prototype.m_InstanceBufferDesc }
	, m_InstanceInitialData{ Prototype.m_InstanceInitialData }
	, m_iInstancingDataStride{ Prototype.m_iInstancingDataStride }
	, m_iNumIndexPerInstance{ Prototype.m_iNumIndexPerInstance }
	, m_iNumInstance{ Prototype.m_iNumInstance }
	, m_pVBInstance{ Prototype.m_pVBInstance }
	, m_InstanceData{ Prototype.m_InstanceData }
{
	Safe_AddRef(m_pVBInstance);
}
HRESULT CVIBuffer_Instancing_Mesh_SRT::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing_Mesh_SRT::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	CModel* pCloneModel = static_cast<CModel*>(m_pGameInstance->Clone_Prototype(pDesc->eLevelID, pDesc->strModelTag));
	if (nullptr == pCloneModel)
		return E_FAIL;

	_uint iNumMesh = (_uint)pCloneModel->Get_NumMeshes();

	vector<VTXMESH> Vertices;
	vector<_uint> Indices;

	for (_uint i = 0; i < iNumMesh; i++)
	{
		const auto& CloneVectexData = pCloneModel->Get_VertexData(i);
		_uint iNumCloneVertices = (_uint)CloneVectexData.size();

		vector<VTXMESH> MeshVertices;
		MeshVertices.resize(iNumCloneVertices);

		for (_uint i = 0; i < iNumCloneVertices; i++)
		{
			MeshVertices[i].vPosition = CloneVectexData[i].vPosition;
			MeshVertices[i].vNormal = CloneVectexData[i].vNormal;
			MeshVertices[i].vTangent = CloneVectexData[i].vTangent;
			MeshVertices[i].vTexCoord = CloneVectexData[i].vTexCoord;
		}
		Vertices.insert(Vertices.end(), MeshVertices.begin(), MeshVertices.end());

		//===================인덱스 데이터 채우기======================//
		const auto& CloneIndexData = pCloneModel->Get_IndexData(i);
		_uint iNumCloneIndices = (_uint)CloneIndexData.size();

		vector<_uint> MeshIndices;
		MeshIndices.resize(iNumCloneIndices);

		for (_uint i = 0; i < iNumCloneIndices; i++)
		{
			MeshIndices[i] = CloneIndexData[i];
		}

		Indices.insert(Indices.end(), MeshIndices.begin(), MeshIndices.end());
	}
	Safe_Release(pCloneModel);

	m_bIsLoop = pDesc->bIsLoop;
	m_iPattern = pDesc->iPattern;
	m_vPivot = pDesc->vPivot;

	m_iNumVertices = static_cast<_uint>(Vertices.size());
	m_iVertexStride = sizeof(VTXMESH);

	m_iNumInstance = pDesc->iNumInstance;
	m_iInstancingDataStride = sizeof(VTXPARTICLE_MESH);

	m_iNumIndexPerInstance = static_cast<_uint>(Indices.size());
	m_iNumIndices = m_iNumIndexPerInstance;

	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;

	m_iNumVertexBuffers = 2;

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = Vertices.data();

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
#pragma endregion


#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iIndexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = Indices.data();

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
#pragma endregion


#pragma region INSTANCE_BUFFER
	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
	m_InstanceBufferDesc.ByteWidth = m_iNumInstance * m_iInstancingDataStride;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.StructureByteStride = m_iInstancingDataStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_InstanceData.resize(m_iNumInstance);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float4 vTranslation = {};

		vTranslation.x = CUtils::Compute_Random(pDesc->vCenter.x - pDesc->vRange.x * 0.5f, pDesc->vCenter.x + pDesc->vRange.x * 0.5f);
		vTranslation.y = CUtils::Compute_Random(pDesc->vCenter.y - pDesc->vRange.y * 0.5f, pDesc->vCenter.y + pDesc->vRange.y * 0.5f);
		vTranslation.z = CUtils::Compute_Random(pDesc->vCenter.z - pDesc->vRange.z * 0.5f, pDesc->vCenter.z + pDesc->vRange.z * 0.5f);
		vTranslation.w = 1;
		
		m_InstanceData[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		m_InstanceData[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		m_InstanceData[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		m_InstanceData[i].vTranslation = vTranslation;

		m_InstanceData[i].vLifeTime.x = CUtils::Compute_Random(pDesc->vLifeTime.x, pDesc->vLifeTime.y);
		m_InstanceData[i].vLifeTime.y = 0.f;
		m_InstanceData[i].fSpeed = CUtils::Compute_Random(pDesc->vSpeed.x, pDesc->vSpeed.y);


		// 회전 적용
		_vector vRotationAxis{};
		switch (pDesc->iRotationAxis)
		{
		case 0:
			vRotationAxis = XMLoadFloat4(&m_InstanceData[i].vRight);
			break;
		case 1:
			vRotationAxis = XMLoadFloat4(&m_InstanceData[i].vUp);
			break;
		case 2:
			vRotationAxis = XMLoadFloat4(&m_InstanceData[i].vLook);
			break;
		default:
			break;
		}
		_float fRotationRadians = XMConvertToRadians(CUtils::Compute_Random(pDesc->fRotationSpeed, pDesc->fRotationFinalSpeed));
		_vector vRotation = XMQuaternionRotationAxis(vRotationAxis, fRotationRadians);
		XMStoreFloat4(&m_InstanceData[i].vRight, XMVectorSetW(XMVector3Rotate(XMLoadFloat4(&m_InstanceData[i].vRight), vRotation), 0.f));
		XMStoreFloat4(&m_InstanceData[i].vUp, XMVectorSetW(XMVector3Rotate(XMLoadFloat4(&m_InstanceData[i].vUp), vRotation), 0.f));
		XMStoreFloat4(&m_InstanceData[i].vLook, XMVectorSetW(XMVector3Rotate(XMLoadFloat4(&m_InstanceData[i].vLook), vRotation), 0.f));
		m_InstanceData[i].vQuaternion = m_vQuaternionRotation;
		XMStoreFloat4(&m_vQuaternionRotation, vRotation);

		// 크기 적용
		_float fRandScale = CUtils::Compute_Random(pDesc->vScalingSize.x, pDesc->vScalingSize.y);
		m_vScalingSize = _float3(fRandScale, fRandScale, fRandScale);
		m_fFinalSize = pDesc->fFinalSize;
		XMStoreFloat4(&m_InstanceData[i].vRight, XMLoadFloat4(&m_InstanceData[i].vRight) * fRandScale);
		XMStoreFloat4(&m_InstanceData[i].vUp, XMLoadFloat4(&m_InstanceData[i].vUp) * fRandScale);
		XMStoreFloat4(&m_InstanceData[i].vLook, XMLoadFloat4(&m_InstanceData[i].vLook) * fRandScale);
	}
	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
	m_InstanceInitialData.pSysMem = m_InstanceData.data();
#pragma endregion

	Create_CS(m_iPattern);
	return m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialData, &m_pVBInstance);
}

HRESULT CVIBuffer_Instancing_Mesh_SRT::Bind_InputAssembler()
{
	if (nullptr == m_pContext || false == m_bIsRenderable)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffer[] =
	{
		m_pVB,
		m_pVBInstance,
	};

	_uint				iVertexStrides[] =
	{
		m_iVertexStride,
		m_iInstancingDataStride,
	};

	_uint				iOffsets[] =
	{
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffer, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

HRESULT CVIBuffer_Instancing_Mesh_SRT::Render()
{
	if (nullptr == m_pContext || false == m_bIsRenderable)
		return E_FAIL;

	m_pContext->DrawIndexedInstanced(m_iNumIndexPerInstance, m_iNumInstance, 0, 0, 0);
	return S_OK;
}

_bool CVIBuffer_Instancing_Mesh_SRT::Progress(_float fTimeDelta)
{
	CONST_BUFFER Desc{};
	Desc.iNumInstance = m_iNumInstance;
	Desc.fTimeDelta = fTimeDelta;
	Desc.bIsLoop = m_bIsLoop;
	Desc.vPivot = _float4(m_vPivot.x, m_vPivot.y, m_vPivot.z, 1.f);

	Desc.vOriginSize = m_vScalingSize;
	Desc.fFinalSize = m_fFinalSize;

	m_bIsRenderable = true;
	if (FAILED(m_pComputeShader->Compute(0, 1, &Desc, sizeof(CONST_BUFFER), m_pVBInstance)))
		m_bIsRenderable = false;

	return true;
}

void CVIBuffer_Instancing_Mesh_SRT::Change_Pattern(_uint iPattern)
{
	if (m_iPattern != iPattern)
	{
		m_iPattern = iPattern;
		switch (m_iPattern)
		{
		case FRACTION:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_MeshPtcl.hlsl", "CSMesh_Fraction");
			break;
		case EXPL_SLOWDOWN:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_MeshPtcl.hlsl", "CSMesh_ExplSlowDown");
			break;
		case AROUND:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_MeshPtcl.hlsl", "CSMesh_Around");
			break;
		case RISE:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_MeshPtcl.hlsl", "CSMesh_Rise");
			break;
		case HOLD:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_MeshPtcl.hlsl", "CSMesh_Hold");
			break;
		default:
			break;
		}
	}
}

void CVIBuffer_Instancing_Mesh_SRT::Reset_Life()
{
	CONST_BUFFER Desc{};
	Desc.iNumInstance = m_iNumInstance;
	Desc.fTimeDelta = 0.f;
	Desc.vPivot = _float4(m_vPivot.x, m_vPivot.y, m_vPivot.z, 0.f);
	Desc.vOriginSize = m_vScalingSize;
	Desc.fFinalSize = 1.f;

	m_pComputeShader->Init_Data(0, 1, &Desc, sizeof(CONST_BUFFER), m_pVBInstance);
}

void CVIBuffer_Instancing_Mesh_SRT::Create_CS(_uint iPattern)
{
	switch (iPattern)
	{
	case FRACTION:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_MeshPtcl.hlsl", "CSMesh_Fraction", "CSMesh_Clear", sizeof(VTXPARTICLE_MESH), m_iNumInstance, sizeof(CONST_BUFFER), m_InstanceData.data());
		break;	
	case EXPL_SLOWDOWN:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_MeshPtcl.hlsl", "CSMesh_ExplSlowDown", "CSMesh_Clear", sizeof(VTXPARTICLE_MESH), m_iNumInstance, sizeof(CONST_BUFFER), m_InstanceData.data());
		break;
	case AROUND:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_MeshPtcl.hlsl", "CSMesh_Around", "CSMesh_Clear", sizeof(VTXPARTICLE_MESH), m_iNumInstance, sizeof(CONST_BUFFER), m_InstanceData.data());
		break;
	case RISE:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_MeshPtcl.hlsl", "CSMesh_Rise", "CSMesh_Clear", sizeof(VTXPARTICLE_MESH), m_iNumInstance, sizeof(CONST_BUFFER), m_InstanceData.data());
		break;
	case HOLD:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_MeshPtcl.hlsl", "CSMesh_Hold", "CSMesh_Clear", sizeof(VTXPARTICLE_MESH), m_iNumInstance, sizeof(CONST_BUFFER), m_InstanceData.data());
		break;
	default:
		break;
	}
}

CVIBuffer_Instancing_Mesh_SRT* CVIBuffer_Instancing_Mesh_SRT::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Instancing_Mesh_SRT* pInstance = new CVIBuffer_Instancing_Mesh_SRT(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CVIBuffer_Instancing_Mesh_SRT");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Instancing_Mesh_SRT::Clone(void* pArg)
{
	CVIBuffer_Instancing_Mesh_SRT* pInstance = new CVIBuffer_Instancing_Mesh_SRT(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CVIBuffer_Instancing_Mesh_SRT");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Instancing_Mesh_SRT::Free()
{
	super::Free();

	Safe_Release(m_pComputeShader);
	Safe_Release(m_pVBInstance);
}
