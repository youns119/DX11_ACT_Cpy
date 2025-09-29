#include "Mesh.h"

#include "Model.h"
#include "Shader.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer(Prototype)
{
}

HRESULT CMesh::Initialize_Prototype(CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<CBone*>& Bones)
{
	pAIMesh->mColors;

	strcpy_s(m_szName, pAIMesh->mName.C_Str());

	m_iMaterialIndex = pAIMesh->mMaterialIndex;

	m_iNumVertices = pAIMesh->mNumVertices;

	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iIndexStride = 4;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iNumVertexBuffers = 1;

	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;



#pragma region VERTEX_BUFFER
	HRESULT hr {};
	switch (pModel->Get_Type())
	{
	case CModel::TYPE::PRE_TRANSFORM:
		m_iVertexStride = sizeof(VTXMESH);
		hr = Ready_VertexBuffer_For_PreTransform(pAIMesh, PreTransformMatrix);
		break;

	case CModel::TYPE::NON_ANIM:
		m_iVertexStride = sizeof(VTXMESH);
		hr = Ready_VertexBuffer_For_NonAnim(pModel, pAIMesh, PreTransformMatrix, Bones);
		break;

	case CModel::TYPE::ANIM:
		m_iVertexStride = sizeof(VTXANIMMESH);
		hr = Ready_VertexBuffer_For_Anim(pModel, pAIMesh);
		break;
	}

	if (FAILED(hr))
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



	std::vector<_uint> Indices(m_iNumIndices);
	for (size_t i = 0; i < pAIMesh->mNumFaces; ++i)
	{
		Indices[i * 3 + 0] = pAIMesh->mFaces[i].mIndices[0];
		Indices[i * 3 + 1] = pAIMesh->mFaces[i].mIndices[1];
		Indices[i * 3 + 2] = pAIMesh->mFaces[i].mIndices[2];
	}

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	m_InitialData.pSysMem = Indices.data();


	if (FAILED(super::Create_Buffer(&m_pIB)))
		return E_FAIL;
#pragma endregion



	m_Indices = Indices;
	return S_OK;
}

HRESULT CMesh::Initialize_Prototype_From_File(CModel* pModel, const HANDLE& hFile, _fmatrix PreTransformMatrix, _bool isNeedOfRecalculation)
{
	// Load(hFile);
	_ulong dwByte = 0;
	_bool bResult = FALSE;

	bResult = ReadFile(hFile, m_szName, sizeof(m_szName), &dwByte, nullptr);

	bResult = ReadFile(hFile, &m_iMaterialIndex, sizeof(m_iMaterialIndex), &dwByte, nullptr);

	bResult = ReadFile(hFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);
	m_Vertices.resize(m_iNumVertices);
	bResult = ReadFile(hFile, m_Vertices.data(), sizeof(VTXANIMMESH) * (DWORD)m_Vertices.size(), &dwByte, nullptr);

	bResult = ReadFile(hFile, &m_iNumIndices, sizeof(_uint), &dwByte, nullptr);
	m_Indices.resize(m_iNumIndices);
	bResult = ReadFile(hFile, m_Indices.data(), sizeof(_uint) * (DWORD)m_Indices.size(), &dwByte, nullptr);


	if (isNeedOfRecalculation)
	{
		Calculate_Normal_From_Vertex();
		//Calculate_Tangent_From_Vertex();
	}

	bResult = ReadFile(hFile, &m_iNumBones, sizeof(m_iNumBones), &dwByte, nullptr);
	if (0 != m_iNumBones)
	{
		m_BoneIndices.resize(m_iNumBones);
		m_OffsetMatrices.resize(m_iNumBones);
		m_BoneMatrices.resize(m_iNumBones);
		bResult = ReadFile(hFile, m_BoneIndices.data(), sizeof(_uint) * (DWORD)m_BoneIndices.size(), &dwByte, nullptr);
		bResult = ReadFile(hFile, m_OffsetMatrices.data(), sizeof(_float4x4) * (DWORD)m_OffsetMatrices.size(), &dwByte, nullptr);
	}

	m_iIndexStride = 4;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iNumVertexBuffers = 1;

	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER
	HRESULT hr {};
	switch (pModel->Get_Type())
	{
	case CModel::TYPE::PRE_TRANSFORM:
	case CModel::TYPE::NON_ANIM:
		{
		m_iVertexStride = sizeof(VTXMESH);

		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
		m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.StructureByteStride = m_iVertexStride;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;

			vector<VTXMESH> NonAnimVertices(m_iNumVertices);
			for (_uint i = 0; i < m_iNumVertices; ++i)
			{
				
				XMStoreFloat3(&m_Vertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&m_Vertices[i].vPosition), PreTransformMatrix));
				XMStoreFloat3(&m_Vertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&m_Vertices[i].vNormal), PreTransformMatrix));
				NonAnimVertices[i].vPosition = m_Vertices[i].vPosition;
				NonAnimVertices[i].vNormal = m_Vertices[i].vNormal;
				NonAnimVertices[i].vTexCoord = m_Vertices[i].vTexCoord;
				NonAnimVertices[i].vTangent = m_Vertices[i].vTangent;
			}

			ZeroMemory(&m_InitialData, sizeof(m_InitialData));
			m_InitialData.pSysMem = NonAnimVertices.data();

		if (FAILED(super::Create_Buffer(&m_pVB)))
			return E_FAIL;
		}

		break;

	case CModel::TYPE::ANIM:
		m_iVertexStride = sizeof(VTXANIMMESH);

		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
		m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.StructureByteStride = m_iVertexStride;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;

		ZeroMemory(&m_InitialData, sizeof(m_InitialData));
		m_InitialData.pSysMem = m_Vertices.data();

		if (FAILED(super::Create_Buffer(&m_pVB)))
			return E_FAIL;
		break;
	default:
		assert(!"Animation Type is not Initialized");
		break;
	}

	if (FAILED(hr))
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

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	m_InitialData.pSysMem = m_Indices.data(); // filled with file data


	if (FAILED(super::Create_Buffer(&m_pIB)))
		return E_FAIL;
#pragma endregion


	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CMesh::Bind_InputAssembler()
{
	if (!m_isRender)
		return S_OK;

	return super::Bind_InputAssembler();
}

HRESULT CMesh::Render()
{
	if (!m_isRender)
		return S_OK;

	return super::Render();
}

HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones)
{
	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombindedTransformationMatrix());
	}

	//return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices.data(), m_iNumBones);
}

_bool CMesh::Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist)
{
	if (!m_isRender)
		return FALSE;

	vector<_float> Dists;

	for (size_t i = 0; i < m_Indices.size(); i += 3)
	{
		_vector v0 = XMLoadFloat3(&m_Vertices[m_Indices[i]].vPosition);
		_vector v1 = XMLoadFloat3(&m_Vertices[m_Indices[i + 1]].vPosition);
		_vector v2 = XMLoadFloat3(&m_Vertices[m_Indices[i + 2]].vPosition);

		_float fTempDist;
		if (TriangleTests::Intersects(vRayOrigin, vRayDirection, v0, v1, v2, fTempDist))
		{
			Dists.push_back(fTempDist);
		}
	}


	if (Dists.empty())
		return FALSE;
	else
	{
		_float fMinDist = *min_element(Dists.begin(), Dists.end());
		fDist = fMinDist;
		return TRUE;
	}
}

//HRESULT CMesh::Save(const HANDLE& hFile, _bool isNonAnim, _fmatrix PreTransformationMatrix)
HRESULT CMesh::Save(const HANDLE& hFile, CModel* pModel, _fmatrix PreTransformationMatrix)
{
	// 우선은 non anim 은 고려하지 않는다.
	// Anim Mesh의 정보는 Non Anim 의 정보를 모두 포함하고 있기 때문이다. 
	assert(!(INVALID_HANDLE_VALUE == hFile));
	_ulong dwByte = 0;

	WriteFile(hFile, m_szName, sizeof(m_szName), &dwByte, nullptr);
	if (sizeof(m_szName) != dwByte)
		return E_FAIL;

	WriteFile(hFile, &m_iMaterialIndex, sizeof(m_iMaterialIndex), &dwByte, nullptr);

	WriteFile(hFile, &m_iNumVertices, sizeof(m_iNumVertices), &dwByte, nullptr);

	//if (isNonAnim)
	switch (pModel->Get_Type())
	{
	case CModel::TYPE::PRE_TRANSFORM:
	case CModel::TYPE::NON_ANIM:

	{
		_matrix PreTransformationMatrixInverse = XMMatrixInverse(nullptr, PreTransformationMatrix);

		vector<VTXANIMMESH> BeforePreTransformVertices;
		BeforePreTransformVertices.reserve(m_iNumVertices);
		//for (auto& Vertex : BeforePreTransformVertices)
		for (size_t i=0; i< m_iNumVertices; ++i)
		{
			VTXANIMMESH Vertex {};
			XMStoreFloat3(&Vertex.vPosition, XMVector3TransformCoord(XMLoadFloat3(&m_Vertices[i].vPosition), PreTransformationMatrixInverse));
			XMStoreFloat3(&Vertex.vNormal, XMVector3TransformNormal(XMLoadFloat3(&m_Vertices[i].vNormal), PreTransformationMatrixInverse));
			Vertex.vTexCoord = m_Vertices[i].vTexCoord;
			Vertex.vTangent = m_Vertices[i].vTangent;

			BeforePreTransformVertices.push_back(Vertex);
		}
		// non anim mesh 인 경우 pre trnasform 을 이미 곱해서 버퍼에 넣어준다. 그래서 원본 데이터가 훼손되어 있다.
		WriteFile(hFile, BeforePreTransformVertices.data(), sizeof(VTXANIMMESH) * m_iNumVertices, &dwByte, nullptr);
	}
		break;

	case CModel::TYPE::ANIM:
		WriteFile(hFile, m_Vertices.data(), sizeof(VTXANIMMESH) * m_iNumVertices, &dwByte, nullptr);
		break;

	default:
		assert(!"Check model type");
		break;
	}

	WriteFile(hFile, &m_iNumIndices, sizeof(m_iNumIndices), &dwByte, nullptr);
	WriteFile(hFile, m_Indices.data(), sizeof(_uint) * m_iNumIndices, &dwByte, nullptr);

	WriteFile(hFile, &m_iNumBones, sizeof(m_iNumBones), &dwByte, nullptr);
	if (0 != m_iNumBones)
	{
		// 어짜피 안될거깉한데
		WriteFile(hFile, m_BoneIndices.data(), sizeof(_uint) * m_iNumBones, &dwByte, nullptr);
		WriteFile(hFile, m_OffsetMatrices.data(), sizeof(_float4x4) * m_iNumBones, &dwByte, nullptr);
	}

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_PreTransform(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix) // PreTransformMatrix 를 FBX로 로드할땐 사용하지 말고 저장하자.
{
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	std::vector<VTXMESH> Vertices(m_iNumVertices);
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&Vertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&Vertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&Vertices[i].vPosition), PreTransformMatrix));

		memcpy(&Vertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&Vertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&Vertices[i].vNormal), PreTransformMatrix));

		if (nullptr != pAIMesh->mTextureCoords[0])
			memcpy(&Vertices[i].vTexCoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		else
		{
			_uint a = 0;
			++a;
		}

		if (nullptr != pAIMesh->mTangents)
		{
			memcpy(&Vertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
			XMStoreFloat3(&Vertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&Vertices[i].vTangent), PreTransformMatrix));
		}
		else
		{
			_uint a = 0;
			++a;
		}

	}

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	m_InitialData.pSysMem = Vertices.data();

	if (FAILED(super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	//m_Vertices = Vertices;
	m_Vertices.reserve(Vertices.size());
	for (auto& Vertex : Vertices)
	{
		VTXANIMMESH Element {};
		// 위험한거같긴한데 구조체를 상속할거냐, 아님 변환해주는 함수를 만들거냐 
		//memcpy(&Element, &Vertex, sizeof(VTXMESH));
		// 혹은
		Element.vPosition = Vertex.vPosition;
		Element.vNormal = Vertex.vNormal;
		Element.vTexCoord = Vertex.vTexCoord;
		Element.vTangent = Vertex.vTangent;

		m_Vertices.push_back(Element);
	}
	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<CBone*>& Bones)
{
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	std::vector<VTXANIMMESH> Vertices(m_iNumVertices);
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&Vertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		//XMStoreFloat3(&Vertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&Vertices[i].vPosition), PreTransformMatrix));

		memcpy(&Vertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		//XMStoreFloat3(&Vertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&Vertices[i].vNormal), PreTransformMatrix));

		memcpy(&Vertices[i].vTexCoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

		memcpy(&Vertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		XMStoreFloat3(&Vertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&Vertices[i].vNormal), PreTransformMatrix));
	}

	m_iNumBones = pAIMesh->mNumBones;

	m_OffsetMatrices.reserve(m_iNumBones);
	m_BoneIndices.reserve(m_iNumBones);

	m_BoneMatrices.resize(m_iNumBones); //m_BoneMatrices.resize(256, {});

	using blendData = std::pair<_uint, _float>;
	std::vector<std::vector<blendData>> VertexWeights; // 현재 mesh에 해당하는 정점들마다 blend index, blend weight를 모두 저장 후 weight 별로 정렬 후 4개씩 자르기
	VertexWeights.resize(m_iNumVertices);

	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4 OffsetMatrix {};

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		m_BoneIndices.push_back(pModel->Get_BoneIndex_By_Name(pAIBone->mName.C_Str()));

		for (size_t j = 0; j < pAIBone->mNumWeights; ++j)
		{
			aiVertexWeight AIVertexWeight = pAIBone->mWeights[j];
			VertexWeights[AIVertexWeight.mVertexId].push_back({ (_uint)i, (_float)AIVertexWeight.mWeight }); // type 안맞춰주면 컴파일러가 화냄
		}

		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombindedTransformationMatrix());
	}


	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		if (VertexWeights[i].size() > 4)
		{
			std::sort(VertexWeights[i].begin(), VertexWeights[i].end(),
				[](const blendData& Src, const blendData& Dst)->_bool
				{
					return Src.second > Dst.second;
				});
		}

		VertexWeights[i].resize(4);

		Vertices[i].vBlendIndex.x = VertexWeights[i][0].first;
		Vertices[i].vBlendWeight.x = VertexWeights[i][0].second;

		Vertices[i].vBlendIndex.y = VertexWeights[i][1].first;
		Vertices[i].vBlendWeight.y = VertexWeights[i][1].second;

		Vertices[i].vBlendIndex.z = VertexWeights[i][2].first;
		Vertices[i].vBlendWeight.z = VertexWeights[i][2].second;

		Vertices[i].vBlendIndex.w = VertexWeights[i][3].first;
		Vertices[i].vBlendWeight.w = VertexWeights[i][3].second;
	}

	if (0 == m_iNumBones)
	{
		++m_iNumBones;
		_float4x4 IdentityMatirx {};
		XMStoreFloat4x4(&IdentityMatirx, XMMatrixIdentity());

		m_OffsetMatrices.push_back(IdentityMatirx);

		m_BoneIndices.push_back(pModel->Get_BoneIndex_By_Name(m_szName));

		m_BoneMatrices.resize(1);
		XMStoreFloat4x4(&m_BoneMatrices[0], /* offset matrix is Identity */Bones[m_BoneIndices.back()]->Get_CombindedTransformationMatrix());
	}



	vector<VTXMESH> VerticesToBuffer;
	VerticesToBuffer.reserve(Vertices.size());
	for (size_t i = 0; i < Vertices.size(); ++i)
	{
		VTXMESH Vertex {};

		Vertex.vNormal = Vertices[i].vNormal;
		Vertex.vTangent = Vertices[i].vTangent;
		Vertex.vTexCoord = Vertices[i].vTexCoord;


		_matrix BoneMatrixX = XMLoadFloat4x4(&m_BoneMatrices[Vertices[i].vBlendIndex.x]);
		_matrix BoneMatrixY = XMLoadFloat4x4(&m_BoneMatrices[Vertices[i].vBlendIndex.y]);
		_matrix BoneMatrixZ = XMLoadFloat4x4(&m_BoneMatrices[Vertices[i].vBlendIndex.z]);
		_matrix BoneMatrixW = XMLoadFloat4x4(&m_BoneMatrices[Vertices[i].vBlendIndex.w]);

		_float fWeightW = 1 - (Vertices[i].vBlendWeight.x + Vertices[i].vBlendWeight.y + Vertices[i].vBlendWeight.z);
		_matrix BlendMatrix = 
			(BoneMatrixX * Vertices[i].vBlendWeight.x) +
			(BoneMatrixY * Vertices[i].vBlendWeight.y) +
			(BoneMatrixZ * Vertices[i].vBlendWeight.z) +
			(BoneMatrixW * fWeightW);

		XMStoreFloat3(&Vertex.vPosition, XMVector3TransformCoord(XMLoadFloat3(&Vertices[i].vPosition), BlendMatrix * PreTransformMatrix));

		VerticesToBuffer.push_back(Vertex);
	}

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	m_InitialData.pSysMem = VerticesToBuffer.data();

	if (FAILED(super::Create_Buffer(&m_pVB)))
		return E_FAIL;


	/* copy to member vertices */
	m_Vertices.reserve(VerticesToBuffer.size());
	std::for_each(VerticesToBuffer.begin(), VerticesToBuffer.end(), [this](const VTXMESH& SourVertex)->void
		{
			VTXANIMMESH DestVertex {};
			DestVertex.vNormal = SourVertex.vNormal;
			DestVertex.vPosition = SourVertex.vPosition;
			DestVertex.vTexCoord = SourVertex.vTexCoord;
			DestVertex.vTangent = SourVertex.vTangent;

			m_Vertices.push_back(DestVertex);
		});

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_Anim(CModel* pModel, const aiMesh* pAIMesh)
{
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	std::vector<VTXANIMMESH> Vertices(m_iNumVertices);
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&Vertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&Vertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&Vertices[i].vTexCoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&Vertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	m_iNumBones = pAIMesh->mNumBones;

	m_OffsetMatrices.reserve(m_iNumBones);
	m_BoneIndices.reserve(m_iNumBones);

	m_BoneMatrices.resize(m_iNumBones); //m_BoneMatrices.resize(256, {});

	using blendData = std::pair<_uint, _float>;
	std::vector<std::vector<blendData>> VertexWeights; // 현재 mesh에 해당하는 정점들마다 blend index, blend weight를 모두 저장 후 weight 별로 정렬 후 4개씩 자르기
	VertexWeights.resize(m_iNumVertices);

	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4 OffsetMatrix {};

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		m_BoneIndices.push_back(pModel->Get_BoneIndex_By_Name(pAIBone->mName.C_Str()));


		/*pAIBone->mNumWeights :  이 뼈는 몇개의 정점에게 영향을 줍니다. 뼈의 가중치 정보의 개수*/
		for (size_t j = 0; j < pAIBone->mNumWeights; ++j)
		{
			/* pAIBone->mWeights[j] : 이 뼈가 영향을 주는 j번째 정보 : veteix ID, weight */

			aiVertexWeight AIVertexWeight = pAIBone->mWeights[j];
			VertexWeights[AIVertexWeight.mVertexId].push_back({ (_uint)i, (_float)AIVertexWeight.mWeight }); // type 안맞춰주면 컴파일러가 화냄
		}
	}


	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		if (VertexWeights[i].size() > 4)
		{
			std::sort(VertexWeights[i].begin(), VertexWeights[i].end(),
				[](const blendData& Src, const blendData& Dst)->_bool
				{
					return Src.second > Dst.second;
				});
		}

		VertexWeights[i].resize(4);

		Vertices[i].vBlendIndex.x = VertexWeights[i][0].first;
		Vertices[i].vBlendWeight.x = VertexWeights[i][0].second;

		Vertices[i].vBlendIndex.y = VertexWeights[i][1].first;
		Vertices[i].vBlendWeight.y = VertexWeights[i][1].second;

		Vertices[i].vBlendIndex.z = VertexWeights[i][2].first;
		Vertices[i].vBlendWeight.z = VertexWeights[i][2].second;

		Vertices[i].vBlendIndex.w = VertexWeights[i][3].first;
		Vertices[i].vBlendWeight.w = VertexWeights[i][3].second;

		//XMStoreFloat4(&Vertices[i].vBlendWeight, XMVector4Normalize(XMLoadFloat4(&Vertices[i].vBlendWeight)));
		// 각 요소들의 합이 1인것이지 해당 벡터의 길이가 1이아니다. 따라서 normalize 하면 안된다.

		// weight.x = wieght.x * 1/가중치합 이런식으로 조절할 수 있을거같다.
	}

	if (0 == m_iNumBones)
	{
		++m_iNumBones;
		_float4x4 IdentityMatirx {};
		XMStoreFloat4x4(&IdentityMatirx, XMMatrixIdentity());

		m_OffsetMatrices.push_back(IdentityMatirx);

		m_BoneIndices.push_back(pModel->Get_BoneIndex_By_Name(m_szName));

		m_BoneMatrices.resize(1);
		//m_BoneMatrices.push_back(IdentityMatirx); // 어짜피 새로 만드니까 그냥 0 행렬로 채워도 상관없음 

		// 안해도 어짜피 0으로 채워져 있을거임
		//for (auto& Vertex : Vertices)
		//{
		//	Vertex.vBlendIndex.w = 0;
		//}
	}


	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	m_InitialData.pSysMem = Vertices.data();

	if (FAILED(super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	m_Vertices = Vertices;

	return S_OK;
}

void CMesh::Calculate_Normal_From_Vertex()
{
	vector<_float3> vReCalculatedNormal;
	vReCalculatedNormal.assign(m_iNumVertices, { 0.f, 0.f, 0.f });
	for (size_t i = 0; i < m_Indices.size(); i += 3)
	{
		size_t iIndex[3] {};
		iIndex[0] = m_Indices[i];
		iIndex[1] = m_Indices[i + 1];
		iIndex[2] = m_Indices[i + 2];

		for (_uint j = 0; j < 3; ++j)
		{
			if (iIndex[j] >= m_Vertices.size())
				_CrtDbgBreak();
		}

		VTXANIMMESH Vertex[3] {};
		for (_uint j = 0; j < 3; ++j)
			Vertex[j] = m_Vertices[iIndex[j]];

		_vector vPosition[3] {};
		for (_uint j = 0; j < 3; ++j)
			vPosition[j] = XMLoadFloat3(&Vertex[j].vPosition);

		_vector vSrc = XMVector3Normalize(vPosition[1] - vPosition[0]);
		_vector vDst = XMVector3Normalize(vPosition[2] - vPosition[0]);
		_vector vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

		for (_uint j = 0; j < 3; ++j)
			//XMStoreFloat3(&m_Vertices[iIndex[j]].vNormal, XMLoadFloat3(&m_Vertices[iIndex[j]].vNormal) + vNormal);
			XMStoreFloat3(&vReCalculatedNormal[iIndex[j]], XMLoadFloat3(&vReCalculatedNormal[iIndex[j]]) + vNormal);
	}
 	for (size_t i = 0; i < m_iNumVertices; ++i)
		XMStoreFloat3(&m_Vertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&vReCalculatedNormal[i])));

}

void CMesh::Calculate_Tangent_From_Vertex()
{
	auto Compute_Tangent =
		[](const _float3& vPos1, const _float3& vPos2, const _float3& vPos3, const _float2& vUV1, const _float2& vUV2, const _float2& vUV3)->_vector
		{
			_vector vEdge1 = XMLoadFloat3(&vPos2) - XMLoadFloat3(&vPos1);
			_vector vEdge2 = XMLoadFloat3(&vPos3) - XMLoadFloat3(&vPos1);

			_vector vDeltaUV1 = XMLoadFloat2(&vUV2) - XMLoadFloat2(&vUV1);
			_vector vDeltaUV2 = XMLoadFloat2(&vUV3) - XMLoadFloat2(&vUV1);

			_float fDet = XMVectorGetX(vDeltaUV1) * XMVectorGetY(vDeltaUV2) - XMVectorGetY(vDeltaUV1) * XMVectorGetX(vDeltaUV2);

			if (fDet != 0.f)
			{
				return 1.f / fDet * (XMVectorGetY(vDeltaUV2) * vEdge1 - XMVectorGetY(vDeltaUV1) * vEdge2);
			}
			else
			{
				return XMVectorSet(0.f, 0.f, 0.f, 0.f);
			}

		};

	vector<_float3> vReCalculatedTangent;
	vReCalculatedTangent.assign(m_iNumVertices, { 0.f, 0.f, 0.f });
	for (size_t i = 0; i < m_Indices.size(); i += 3)
	{
		size_t iIndex[3] {};
		iIndex[0] = m_Indices[i];
		iIndex[1] = m_Indices[i + 1];
		iIndex[2] = m_Indices[i + 2];

		_vector vTangent {};
		_vector vTangentAcc {};

		vTangent = Compute_Tangent(
			m_Vertices[iIndex[0]].vPosition, m_Vertices[iIndex[1]].vPosition, m_Vertices[iIndex[2]].vPosition,
			m_Vertices[iIndex[0]].vTexCoord, m_Vertices[iIndex[1]].vTexCoord, m_Vertices[iIndex[2]].vTexCoord);

		vTangentAcc = XMLoadFloat3(&vReCalculatedTangent[iIndex[0]]) + XMVector3Normalize(vTangent);
		XMStoreFloat3(&vReCalculatedTangent[iIndex[0]], vTangentAcc);

		//XMStoreFloat3(&m_Vertices[iIndex[0]].vTangent, XMVector3Normalize(vTangent));


		vTangent = Compute_Tangent(
			m_Vertices[iIndex[1]].vPosition, m_Vertices[iIndex[2]].vPosition, m_Vertices[iIndex[0]].vPosition,
			m_Vertices[iIndex[1]].vTexCoord, m_Vertices[iIndex[2]].vTexCoord, m_Vertices[iIndex[0]].vTexCoord);

		vTangentAcc = XMLoadFloat3(&vReCalculatedTangent[iIndex[1]]) + XMVector3Normalize(vTangent);
		XMStoreFloat3(&vReCalculatedTangent[iIndex[1]], vTangentAcc);

		//XMStoreFloat3(&m_Vertices[iIndex[1]].vTangent, XMVector3Normalize(vTangent));


		vTangent = Compute_Tangent(
			m_Vertices[iIndex[2]].vPosition, m_Vertices[iIndex[0]].vPosition, m_Vertices[iIndex[1]].vPosition,
			m_Vertices[iIndex[2]].vTexCoord, m_Vertices[iIndex[0]].vTexCoord, m_Vertices[iIndex[1]].vTexCoord);

		vTangentAcc = XMLoadFloat3(&vReCalculatedTangent[iIndex[2]]) + XMVector3Normalize(vTangent);
		XMStoreFloat3(&vReCalculatedTangent[iIndex[2]], vTangentAcc);
		//XMStoreFloat3(&m_Vertices[iIndex[2]].vTangent, XMVector3Normalize(vTangent));
	}

	//for (auto& Vertex : m_Vertices)
	//{
	//	XMStoreFloat3(&Vertex.vTangent, XMVector3Normalize(XMLoadFloat3(&Vertex.vTangent)));
	//}
	for (size_t i = 0; i < m_iNumVertices; ++i)
		XMStoreFloat3(&m_Vertices[i].vTangent, XMVector3Normalize(XMLoadFloat3(&vReCalculatedTangent[i])));

}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<CBone*>& Bones)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModel, pAIMesh, PreTransformMatrix, Bones)))
	{
		MSG_BOX("Failed To Create : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Create_From_File(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel* pModel, const HANDLE& hFile, _fmatrix PreTransformMatrix, _bool isNeedOfRecalculation)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype_From_File(pModel, hFile, PreTransformMatrix, isNeedOfRecalculation)))
	{
		MSG_BOX("Failed To Create : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
    return nullptr;
}

void CMesh::Free()
{
    super::Free();
}
