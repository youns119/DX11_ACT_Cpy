#include "VIBuffer_Instancing_Mesh.h"
#include "GameInstance.h"

CVIBuffer_Instancing_Mesh::CVIBuffer_Instancing_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instancing_Mesh::CVIBuffer_Instancing_Mesh(const CVIBuffer_Instancing_Mesh& Prototype)
	: super(Prototype)
	, m_InstanceBufferDesc{ Prototype.m_InstanceBufferDesc }
	, m_InstanceInitialData{ Prototype.m_InstanceInitialData }
	, m_iInstanceVertexStride{ Prototype.m_iInstanceVertexStride }
	, m_iNumIndexPerInstance{ Prototype.m_iNumIndexPerInstance }
	, m_iNumInstance{ Prototype.m_iNumInstance }
	, m_iTotalInstance{ Prototype.m_iTotalInstance }
	, m_pVBInstance{ Prototype.m_pVBInstance }
	, m_InstanceVertices{ Prototype.m_InstanceVertices }
{
	Safe_AddRef(m_pVBInstance);
}
HRESULT CVIBuffer_Instancing_Mesh::Initialize_Prototype(const DESC* pDesc, const _wstring& strPrototypeTag, const _uint iLevelIndex)
{
	if (!pDesc)
		return E_FAIL;

	CModel* pCloneModel = static_cast<CModel*>(m_pGameInstance->Clone_Prototype(iLevelIndex, strPrototypeTag));
	if (nullptr == pCloneModel)
		return E_FAIL;

	_uint iNumMesh = (_uint)pCloneModel->Get_NumMeshes();

	vector<VTXMESH> Vertices; // �̰��� ���ۿ� ä���� ����Ÿ
	vector<_uint> Indices;

	for (_uint i = 0; i < iNumMesh; i++) // ���� ������ �ִ� �޽� ������ŭ �ݺ�
	{
		//���ؽ� ������ ä��� : ��->�޽�����->�޽�������ŭ���ؽ�����->2�߹ݺ������粨����->�������Ϳ� �μ�Ʈ
		const auto& CloneVectexData = pCloneModel->Get_VertexData(i); // i ��° ����<���ؽ�data> 
		_uint iNumCloneVertices = (_uint)CloneVectexData.size(); // i��° ���� ������ 

		vector<VTXMESH> MeshVertices; // �ӽ� ���� ����
		MeshVertices.resize(iNumCloneVertices); // ���� ũ�� ��ŭ �������� ���ش�

		for (_uint i = 0; i < iNumCloneVertices; i++)
		{
			MeshVertices[i].vPosition = CloneVectexData[i].vPosition;
			MeshVertices[i].vNormal = CloneVectexData[i].vNormal;
			MeshVertices[i].vTangent = CloneVectexData[i].vTangent;
			MeshVertices[i].vTexCoord = CloneVectexData[i].vTexCoord;
		}
		Vertices.insert(Vertices.end(), MeshVertices.begin(), MeshVertices.end());

		//===================�ε��� ������ ä���======================//
		const auto& CloneIndexData = pCloneModel->Get_IndexData(i); // i ��° ����<�ε���data> 
		_uint iNumCloneIndices = (_uint)CloneIndexData.size(); // i��° ���� ������ 

		vector<_uint> MeshIndices; // �ӽ� ���� ����
		MeshIndices.resize(iNumCloneIndices); // ���� ũ�� ��ŭ �������� ���ش�

		for (_uint i = 0; i < iNumCloneIndices; i++)
		{
			MeshIndices[i] = CloneIndexData[i];
		}

		Indices.insert(Indices.end(), MeshIndices.begin(), MeshIndices.end());
	}
	Safe_Release(pCloneModel);

	m_iNumVertices = static_cast<_uint>(Vertices.size()); // ���ؽ� ����
	m_iVertexStride = sizeof(VTXMESH);

	m_iTotalInstance = pDesc->iTotalInstance;
	m_iNumInstance = pDesc->iNumInstance;
	m_iInstanceVertexStride = sizeof(VTXINSTANCE);

	m_iNumIndexPerInstance = static_cast<_uint>(Indices.size());//�ε��� ����

	//m_iNumIndices = m_iNumIndexPerInstance * m_iNumInstance;
	m_iNumIndices = m_iNumIndexPerInstance;

	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;

	m_iNumVertexBuffers = 2; //���ؽ� ���� + �ν��Ͻ� ���� 

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	//m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	//m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	//vector<VTXMESH> Vertices;
	//Vertices.resize(m_iNumVertices);
	////���ؽ� �߰�


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

	//vector<_uint> Indices;
	//Indices.resize(m_iNumIndices);
	////�ε���� ä��� 

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = Indices.data();

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
#pragma endregion


#pragma region INSTANCE_BUFFER
	/* ����� ���鶧 ���� Ȱ���ϱ����� �ν��Ͻ� ���� ���۸� �Ҵ��ϱ����� �ʿ��� �ʱ� �����͸� �̸� �� �����س��´�.  */
	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
	m_InstanceBufferDesc.ByteWidth = m_iTotalInstance * m_iInstanceVertexStride; // ��Ż�� �����ؾߴ�! �ڸ��� ����! ��Ƴ��ߴ�!
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_InstanceVertices.resize(m_iTotalInstance);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float4 vTranslation = {};

		vTranslation.x = CUtils::Compute_Random(pDesc->vCenter.x - pDesc->vRange.x * 0.5f, pDesc->vCenter.x + pDesc->vRange.x * 0.5f);
		vTranslation.y = CUtils::Compute_Random(pDesc->vCenter.y - pDesc->vRange.y * 0.5f, pDesc->vCenter.y + pDesc->vRange.y * 0.5f);
		vTranslation.z = CUtils::Compute_Random(pDesc->vCenter.z - pDesc->vRange.z * 0.5f, pDesc->vCenter.z + pDesc->vRange.z * 0.5f);
		vTranslation.w = 1;


		m_InstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		m_InstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		m_InstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		m_InstanceVertices[i].vTranslation = vTranslation;

		_float fRotY = CUtils::Compute_Random(0.0f, 180.f);

		_matrix matRotation = XMMatrixRotationY(XMConvertToRadians(fRotY));
		// ȸ�� ����
		XMStoreFloat4(&m_InstanceVertices[i].vRight, XMVector4Transform(XMLoadFloat4(&m_InstanceVertices[i].vRight), matRotation));
		XMStoreFloat4(&m_InstanceVertices[i].vUp, XMVector4Transform(XMLoadFloat4(&m_InstanceVertices[i].vUp), matRotation));
		XMStoreFloat4(&m_InstanceVertices[i].vLook, XMVector4Transform(XMLoadFloat4(&m_InstanceVertices[i].vLook), matRotation));

		_float fScale = pDesc->vSize.x;
		//_float fScale = CUtils::Compute_Random(vSize.x, vSize.y);

		XMStoreFloat4(&m_InstanceVertices[i].vRight, XMLoadFloat4(&m_InstanceVertices[i].vRight) * fScale);
		XMStoreFloat4(&m_InstanceVertices[i].vUp, XMLoadFloat4(&m_InstanceVertices[i].vUp) * fScale);
		XMStoreFloat4(&m_InstanceVertices[i].vLook, XMLoadFloat4(&m_InstanceVertices[i].vLook) * fScale);
	}

	m_InstanceInitialData.pSysMem = m_InstanceVertices.data();
#pragma endregion


	return S_OK;
}

HRESULT CVIBuffer_Instancing_Mesh::Initialize_Prototype(const DESC* pDesc, const _wstring& strPrototypeTag, const _uint iLevelIndex, const _wstring& strInstancingFilePath)
{
	CModel* pCloneModel = static_cast<CModel*>(m_pGameInstance->Clone_Prototype(iLevelIndex, strPrototypeTag));
	if (nullptr == pCloneModel)
		return E_FAIL;

	_uint iNumMesh = (_uint)pCloneModel->Get_NumMeshes();

	vector<VTXMESH> Vertices; // �̰��� ���ۿ� ä���� ����Ÿ
	vector<_uint> Indices;

	for (_uint i = 0; i < iNumMesh; i++) // ���� ������ �ִ� �޽� ������ŭ �ݺ�
	{
		//���ؽ� ������ ä��� : ��->�޽�����->�޽�������ŭ���ؽ�����->2�߹ݺ������粨����->�������Ϳ� �μ�Ʈ
		const auto& CloneVectexData = pCloneModel->Get_VertexData(i); // i ��° ����<���ؽ�data> 
		_uint iNumCloneVertices = (_uint)CloneVectexData.size(); // i��° ���� ������ 

		vector<VTXMESH> MeshVertices; // �ӽ� ���� ����
		MeshVertices.resize(iNumCloneVertices); // ���� ũ�� ��ŭ �������� ���ش�

		for (_uint i = 0; i < iNumCloneVertices; i++)
		{
			MeshVertices[i].vPosition = CloneVectexData[i].vPosition;
			MeshVertices[i].vNormal = CloneVectexData[i].vNormal;
			MeshVertices[i].vTangent = CloneVectexData[i].vTangent;
			MeshVertices[i].vTexCoord = CloneVectexData[i].vTexCoord;
		}
		Vertices.insert(Vertices.end(), MeshVertices.begin(), MeshVertices.end());

		//===================�ε��� ������ ä���======================//
		const auto& CloneIndexData = pCloneModel->Get_IndexData(i); // i ��° ����<�ε���data> 
		_uint iNumCloneIndices = (_uint)CloneIndexData.size(); // i��° ���� ������ 

		vector<_uint> MeshIndices; // �ӽ� ���� ����
		MeshIndices.resize(iNumCloneIndices); // ���� ũ�� ��ŭ �������� ���ش�

		for (_uint i = 0; i < iNumCloneIndices; i++)
		{
			MeshIndices[i] = CloneIndexData[i];
		}

		Indices.insert(Indices.end(), MeshIndices.begin(), MeshIndices.end());
	}
	Safe_Release(pCloneModel);



	m_iNumVertices = static_cast<_uint>(Vertices.size()); // ���ؽ� ����
	m_iVertexStride = sizeof(VTXMESH);

	//m_iTotalInstance = pDesc->iTotalInstance;
	//m_iNumInstance = pDesc->iNumInstance;
	m_iInstanceVertexStride = sizeof(VTXINSTANCE);

	m_iNumIndexPerInstance = static_cast<_uint>(Indices.size());//�ε��� ����

	//m_iNumIndices = m_iNumIndexPerInstance * m_iNumInstance;
	m_iNumIndices = m_iNumIndexPerInstance;

	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;

	m_iNumVertexBuffers = 2; //���ؽ� ���� + �ν��Ͻ� ���� 

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	//m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	//m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	//vector<VTXMESH> Vertices;
	//Vertices.resize(m_iNumVertices);
	////���ؽ� �߰�


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

	//vector<_uint> Indices;
	//Indices.resize(m_iNumIndices);
	////�ε���� ä��� 

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = Indices.data();

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
#pragma endregion


#pragma region INSTANCE_BUFFER

	// �ν��Ͻ� �������� �ε��ϱ� //
	HRESULT hr{};
	DWORD dwByte{};
	HANDLE hFile = CreateFile(strInstancingFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	_bool bRead = false;

	//bRead = ReadFile(hFile, &m_iTotalInstance, sizeof(_uint), &dwByte, nullptr);// �Ϥ��߿� ���� �ּ� Ǯ�� !!! 1
	//m_iNumInstance = m_iTotalInstance;// �Ϥ��߿� ���� �ּ� Ǯ�� !!!2
	m_iTotalInstance = 10000;
	bRead = ReadFile(hFile, &m_iNumInstance, sizeof(_uint), &dwByte, nullptr);

	// ���� �ε� 1�� //

	/* ����� ���鶧 ���� Ȱ���ϱ����� �ν��Ͻ� ���� ���۸� �Ҵ��ϱ����� �ʿ��� �ʱ� �����͸� �̸� �� �����س��´�.  */
	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
	m_InstanceBufferDesc.ByteWidth = m_iTotalInstance * m_iInstanceVertexStride; // ��Ż�� �����ؾߴ�! �ڸ��� ����! ��Ƴ��ߴ�!
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_InstanceVertices.resize(m_iTotalInstance);

	//���Ϸε������� 
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		VTXINSTANCE ReadInstance = {};

		bRead = ReadFile(hFile, &ReadInstance, sizeof(VTXINSTANCE), &dwByte, nullptr);
		//�̰� ��ġ�� ������ �ӽ��ڵ� 
		//_float fScale = CUtils::Compute_Random(1.f, 2.f);

		//XMStoreFloat4(&ReadInstance.vRight, XMLoadFloat4(&ReadInstance.vRight) * fScale);
		//XMStoreFloat4(&ReadInstance.vUp, XMLoadFloat4(&ReadInstance.vUp) * fScale);
		//XMStoreFloat4(&ReadInstance.vLook, XMLoadFloat4(&ReadInstance.vLook) * fScale);//�̷��ù��̰Ŷ����̾�����������=��

		m_InstanceVertices[i] = ReadInstance;
	}

	CloseHandle(hFile);

	m_InstanceInitialData.pSysMem = m_InstanceVertices.data();
#pragma endregion


	return S_OK;
}

HRESULT CVIBuffer_Instancing_Mesh::Initialize(void* pArg)
{
	return m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialData, &m_pVBInstance);
}

HRESULT CVIBuffer_Instancing_Mesh::Bind_InputAssembler()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffer[] =
	{
		m_pVB,
		m_pVBInstance,
	};

	_uint				iVertexStrides[] =
	{
		m_iVertexStride,
		m_iInstanceVertexStride,
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

HRESULT CVIBuffer_Instancing_Mesh::Render()
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	{
		memcpy(SubResource.pData, m_InstanceVertices.data(), sizeof(VTXINSTANCE) * m_iNumInstance);
	}
	m_pContext->Unmap(m_pVBInstance, 0);



	if (nullptr == m_pContext)
		return E_FAIL;

	m_pContext->DrawIndexedInstanced(m_iNumIndexPerInstance, m_iNumInstance, 0, 0, 0);
	//m_pContext->DrawIndexedInstanced( m_iNumIndexPerInstance, 2, 0, 0, 0 ); // �׸� �ν��Ͻ��� ������ �ٿ��� �ǰٴ�.



	return S_OK;
}

void CVIBuffer_Instancing_Mesh::Input_InstancingInfo(const _int& iNumInstancing, const _float3& vCenter, const _float3& vRange, const _float2& vSize)
{
	_uint iCntLoop = iNumInstancing + m_iNumInstance;
	if (m_iNumInstance == m_iTotalInstance)
		return;
	else if (m_iNumInstance + iNumInstancing > m_iTotalInstance)
		iCntLoop = (m_iTotalInstance - m_iNumInstance) + m_iNumInstance;

	for (size_t i = m_iNumInstance; i < m_iNumInstance + iNumInstancing; i++)
	{
		_float4 vTranslation = {};
		VTXINSTANCE InstanceData = {};

		vTranslation.x = CUtils::Compute_Random(vCenter.x - vRange.x * 0.5f, vCenter.x + vRange.x * 0.5f);
		vTranslation.y = CUtils::Compute_Random(vCenter.y - vRange.y * 0.5f, vCenter.y + vRange.y * 0.5f);
		vTranslation.z = CUtils::Compute_Random(vCenter.z - vRange.z * 0.5f, vCenter.z + vRange.z * 0.5f);
		vTranslation.w = 1;

		InstanceData.vRight = _float4(1.f, 0.f, 0.f, 0.f);
		InstanceData.vUp = _float4(0.f, 1.f, 0.f, 0.f);
		InstanceData.vLook = _float4(0.f, 0.f, 1.f, 0.f);
		InstanceData.vTranslation = vTranslation;

		_float fRotY = CUtils::Compute_Random(0.0f, 180.f);

		_matrix matRotation = XMMatrixRotationY(XMConvertToRadians(fRotY));
		// ȸ�� ����
		XMStoreFloat4(&InstanceData.vRight, XMVector4Transform(XMLoadFloat4(&InstanceData.vRight), matRotation));
		XMStoreFloat4(&InstanceData.vUp, XMVector4Transform(XMLoadFloat4(&InstanceData.vUp), matRotation));
		XMStoreFloat4(&InstanceData.vLook, XMVector4Transform(XMLoadFloat4(&InstanceData.vLook), matRotation));

		_float fScale = CUtils::Compute_Random(vSize.x, vSize.y);

		XMStoreFloat4(&InstanceData.vRight, XMLoadFloat4(&InstanceData.vRight) * fScale);
		XMStoreFloat4(&InstanceData.vUp, XMLoadFloat4(&InstanceData.vUp) * fScale);
		XMStoreFloat4(&InstanceData.vLook, XMLoadFloat4(&InstanceData.vLook) * fScale);

		m_InstanceVertices[i] = InstanceData;
	}
	m_iNumInstance += iNumInstancing;

}

void CVIBuffer_Instancing_Mesh::Delete_PrevInstancing(const _int& iDeleteNumInstancing)
{
	_uint iNumInstancingVertices = static_cast<_uint>(m_InstanceVertices.size());

	if (iDeleteNumInstancing <= 0 || iNumInstancingVertices == 0)
	{
		return;
	}

	if (static_cast<_uint>(iDeleteNumInstancing) >= iNumInstancingVertices)
	{
		m_InstanceVertices.clear(); // ���� �Է��� ���� �� �������� ��ü ���� 
	}
	else
	{
		m_InstanceVertices.erase(m_InstanceVertices.begin(),
			m_InstanceVertices.begin() + iDeleteNumInstancing);
	}
}

void CVIBuffer_Instancing_Mesh::Save_InstancingFile(HANDLE hFile)
{
	DWORD dwByte{};
	WriteFile(hFile, &m_iNumInstance, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		WriteFile(hFile, &m_InstanceVertices[i], sizeof(VTXINSTANCE), &dwByte, nullptr);
	}
}

_bool CVIBuffer_Instancing_Mesh::Find_PickedInstance_WorldMatrix(const _float3& vPickedPos, _float4x4& OutMatrix)
{
	if (m_iNumInstance <= 0)
		return false;

	_int iClosestIndex = -1;
	_float fMinDistance = 50.f;

	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		// �ν��Ͻ��� Ʈ�������̼�(��ġ) ��������
		_float4 instancePos = m_InstanceVertices[i].vTranslation;

		// ��ŷ�� ��ǥ�� �Ÿ� ���
		_vector vInstance = XMLoadFloat4(&instancePos);
		_vector vPicked = XMLoadFloat3(&vPickedPos);
		_float fDistance = XMVectorGetX(XMVector3Length(vInstance - vPicked));

		// �ּ� �Ÿ� �ν��Ͻ� ã��
		if (fDistance < fMinDistance)
		{
			fMinDistance = fDistance;
			iClosestIndex = i;
		}
	}
	m_iPickedInstanceIdx = iClosestIndex;

	if (0 <= m_iPickedInstanceIdx)
	{
		OutMatrix = CombineToWorldMatrix(m_InstanceVertices[m_iPickedInstanceIdx].vRight, m_InstanceVertices[m_iPickedInstanceIdx].vUp,
			m_InstanceVertices[m_iPickedInstanceIdx].vLook, m_InstanceVertices[m_iPickedInstanceIdx].vTranslation);
		return true;
	}
	return false;
}

_float4x4 CVIBuffer_Instancing_Mesh::CombineToWorldMatrix(const _float4& vRight, const _float4& vUp, const _float4& vLook, const _float4& vTranslation)
{
	_matrix WorldMatrix = _matrix(XMLoadFloat4(&vRight), XMLoadFloat4(&vUp), XMLoadFloat4(&vLook), XMLoadFloat4(&vTranslation));
	_float4x4 WorldMat = {};

	XMStoreFloat4x4(&WorldMat, WorldMatrix);

	return WorldMat;
}

_bool CVIBuffer_Instancing_Mesh::Get_PickInstanceMesh_WorldMatrix(_float4x4& OutMatrix)
{
	if (0 <= m_iPickedInstanceIdx)
	{
		OutMatrix = CombineToWorldMatrix(m_InstanceVertices[m_iPickedInstanceIdx].vRight, m_InstanceVertices[m_iPickedInstanceIdx].vUp,
			m_InstanceVertices[m_iPickedInstanceIdx].vLook, m_InstanceVertices[m_iPickedInstanceIdx].vTranslation);
		return true;
	}
	return false;
}

_bool CVIBuffer_Instancing_Mesh::Set_PickInstanceMesh_WorldMatrix(_float4x4& InMatrix)
{
	if (0 <= m_iPickedInstanceIdx)
	{
		m_InstanceVertices[m_iPickedInstanceIdx].vRight = _float4(InMatrix.m[0]);
		m_InstanceVertices[m_iPickedInstanceIdx].vUp = _float4(InMatrix.m[1]);
		m_InstanceVertices[m_iPickedInstanceIdx].vLook = _float4(InMatrix.m[2]);
		m_InstanceVertices[m_iPickedInstanceIdx].vTranslation = _float4(InMatrix.m[3]);
		return true;
	}
	return false;
}

CVIBuffer_Instancing_Mesh* CVIBuffer_Instancing_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const DESC* pDesc, const _wstring& strPrototypeTag, const _uint iLevelIndex)
{
	CVIBuffer_Instancing_Mesh* pInstance = new CVIBuffer_Instancing_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pDesc, strPrototypeTag, iLevelIndex)))
	{
		MSG_BOX("Failed To Create : CVIBuffer_Instancing_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Instancing_Mesh* CVIBuffer_Instancing_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const DESC* pDesc, const _wstring& strPrototypeTag, const _uint iLevelIndex, const _wstring& strInstancingFilePath)
{
	CVIBuffer_Instancing_Mesh* pInstance = new CVIBuffer_Instancing_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pDesc, strPrototypeTag, iLevelIndex, strInstancingFilePath)))
	{
		MSG_BOX("Failed To Create : CVIBuffer_Instancing_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CComponent* CVIBuffer_Instancing_Mesh::Clone(void* pArg)
{
	CVIBuffer_Instancing_Mesh* pInstance = new CVIBuffer_Instancing_Mesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CVIBuffer_Instancing_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Instancing_Mesh::Free()
{
	super::Free();

	Safe_Release(m_pVBInstance);
}
