#include "VIBuffer_Terrain.h"
#include "GameInstance.h"

//#include "QuadTree.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
	: CVIBuffer{ Prototype }
	, m_iNumVerticesX{ Prototype.m_iNumVerticesX }
	, m_iNumVerticesZ{ Prototype.m_iNumVerticesZ }
	, m_Vertices{ Prototype.m_Vertices }
	, m_Indices{ Prototype.m_Indices }
	//, m_pQuadTree{ Prototype.m_pQuadTree }
{
	//Safe_AddRef(m_pQuadTree);
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype_From_BMP(const _tchar* pHeightMapFile)
{
	_ulong dwByte{ 0 };
	HANDLE hFile = CreateFile(pHeightMapFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Open File");
		return E_FAIL;
	}

	BITMAPFILEHEADER FileHeader{};
	if (FALSE == ReadFile(hFile, &FileHeader, sizeof(FileHeader), &dwByte, nullptr))
	{
		MSG_BOX("Failed to Open File");
		return E_FAIL;
	}
	BITMAPINFOHEADER InfoHeader{};
	if (FALSE == ReadFile(hFile, &InfoHeader, sizeof(InfoHeader), &dwByte, nullptr))
	{
		MSG_BOX("Failed to Open File");
		return E_FAIL;
	}

	m_iNumVerticesX = InfoHeader.biWidth;
	m_iNumVerticesZ = InfoHeader.biHeight;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	_uint* pPixels = new _uint[m_iNumVertices]{};
	if (FALSE == ReadFile(hFile, pPixels, sizeof(_uint) * m_iNumVertices, &dwByte, nullptr))
	{
		MSG_BOX("Failed to Open File");
		return E_FAIL;
	}

	m_iVertexStride = sizeof(VTXNORTEX);

	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_iIndexStride = 4 /* sizeof(_uint) */;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;

	m_iNumVertexBuffers = 1;

	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;
			//_float fHeight = (pPixels[iIndex] & 0x000000ff) / 255.f * 20.f;

			//pVertices[iIndex].vPosition = m_pVertexPositions[iIndex] = _float3((_float)j, (_float)(pPixels[iIndex] & 0x000000ff) / 30.f, (_float)i);
			pVertices[iIndex].vPosition = m_pVertexPositions[iIndex] = _float3((_float)j, (_float)(pPixels[iIndex] & 0x000000ff) / 255.f * 0.f, (_float)i);
			//pVertices[iIndex].vPosition = m_pVertexPositions[iIndex] = _float3(_float(j) * m_fIntervalX, fHeight, _float(i) * m_fIntervalZ);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			//pVertices[iIndex].vTexCoord = _float2(j, i);
			pVertices[iIndex].vTexCoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}
#pragma endregion

	CloseHandle(hFile);
	Safe_Delete_Array(pPixels);

#pragma region INDEX_BUFFER
	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);
	_uint iIndex{ 0 };
	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint iVertexIndex = i * m_iNumVerticesX + j;

			_vector vSrc{};
			_vector vDst{};
			_vector vNormal{};

			_uint RectIndex[4] =
			{
				iVertexIndex + m_iNumVerticesX,
				iVertexIndex + m_iNumVerticesX + 1,
				iVertexIndex + 1,
				iVertexIndex,
			};

			pIndices[iIndex] = RectIndex[0];
			++iIndex;
			pIndices[iIndex] = RectIndex[1];
			++iIndex;
			pIndices[iIndex] = RectIndex[2];
			++iIndex;

			vSrc = XMLoadFloat3(&pVertices[RectIndex[1]].vPosition) - XMLoadFloat3(&pVertices[RectIndex[0]].vPosition);
			vDst = XMLoadFloat3(&pVertices[RectIndex[2]].vPosition) - XMLoadFloat3(&pVertices[RectIndex[0]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&pVertices[RectIndex[0]].vNormal, XMLoadFloat3(&pVertices[RectIndex[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[RectIndex[1]].vNormal, XMLoadFloat3(&pVertices[RectIndex[1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[RectIndex[2]].vNormal, XMLoadFloat3(&pVertices[RectIndex[2]].vNormal) + vNormal);

			pIndices[iIndex] = RectIndex[0];
			++iIndex;
			pIndices[iIndex] = RectIndex[2];
			++iIndex;
			pIndices[iIndex] = RectIndex[3];
			++iIndex;

			vSrc = XMLoadFloat3(&pVertices[RectIndex[2]].vPosition) - XMLoadFloat3(&pVertices[RectIndex[0]].vPosition);
			vDst = XMLoadFloat3(&pVertices[RectIndex[3]].vPosition) - XMLoadFloat3(&pVertices[RectIndex[0]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&pVertices[RectIndex[0]].vNormal, XMLoadFloat3(&pVertices[RectIndex[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[RectIndex[2]].vNormal, XMLoadFloat3(&pVertices[RectIndex[2]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[RectIndex[3]].vNormal, XMLoadFloat3(&pVertices[RectIndex[3]].vNormal) + vNormal);

		}
	}

	for (_uint i = 0; i < m_iNumVertices; ++i)
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
#pragma endregion

	HRESULT hr{};

	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	{
		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		//m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.StructureByteStride = m_iVertexStride;
		//m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_BufferDesc.MiscFlags = 0;

	}

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	{
		m_InitialData.pSysMem = pVertices;
	}

	hr = Create_Buffer(&m_pVB);
	m_Vertices.resize(m_iNumVertices);
	memcpy(m_Vertices.data(), pVertices, sizeof(VTXNORTEX) * m_iNumVertices);
	Safe_Delete_Array(pVertices);
	if (FAILED(hr))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	{
		m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		//m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_BufferDesc.StructureByteStride = m_iIndexStride;
		//m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
	}

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	{
		m_InitialData.pSysMem = pIndices;
	}

	hr = Create_Buffer(&m_pIB);
	m_Indices.resize(m_iNumIndices);
	memcpy(m_Indices.data(), pIndices, sizeof(_uint) * m_iNumIndices);
	Safe_Delete_Array(pIndices);
	if (FAILED(hr))
		return E_FAIL;

	//m_pQuadTree = CQuadTree::Create(m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX, m_iNumVerticesX * m_iNumVerticesZ - 1, m_iNumVerticesX - 1, 0);
	//if (nullptr == m_pQuadTree)
	//	return E_FAIL;

	return S_OK;

}

HRESULT CVIBuffer_Terrain::Initialize_Prototype_From_Height(const _tchar* pHeightMapFile)
{
	m_iIndexStride = 4 /* sizeof(_uint) */;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iNumVertexBuffers = 1;
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iVertexStride = sizeof(VTXNORTEX);
	_bool bRead = false;
	HANDLE hFile = CreateFile(pHeightMapFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	DWORD dwRead = 0;
	bRead = ReadFile(hFile, &m_iNumVerticesX, sizeof(_uint), &dwRead, nullptr);
	bRead = ReadFile(hFile, &m_iNumVerticesZ, sizeof(_uint), &dwRead, nullptr);
	bRead = ReadFile(hFile, &m_iNumVertices, sizeof(_uint), &dwRead, nullptr);

	m_Vertices.resize(m_iNumVertices);
	bRead = ReadFile(hFile, m_Vertices.data(), sizeof(VTXNORTEX) * m_iNumVertices, &dwRead, nullptr);
	CloseHandle(hFile);

#pragma region INDEX_BUFFER

	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_Indices.resize(m_iNumIndices);
	_uint iIndex { 0 };
	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint iVertexIndex = i * m_iNumVerticesX + j;
			_uint RectIndex[4] =
			{
				iVertexIndex + m_iNumVerticesX,
				iVertexIndex + m_iNumVerticesX + 1,
				iVertexIndex + 1,
				iVertexIndex,
			};

			m_Indices[iIndex] = RectIndex[0];
			++iIndex;
			m_Indices[iIndex] = RectIndex[1];
			++iIndex;
			m_Indices[iIndex] = RectIndex[2];
			++iIndex;

			m_Indices[iIndex] = RectIndex[0];
			++iIndex;
			m_Indices[iIndex] = RectIndex[2];
			++iIndex;
			m_Indices[iIndex] = RectIndex[3];
			++iIndex;
		}
	}
#pragma endregion

	HRESULT hr {};

	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	{
		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		//m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.StructureByteStride = m_iVertexStride;
		//m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;

	}

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	{
		m_InitialData.pSysMem = m_Vertices.data();
	}

	hr = Create_Buffer(&m_pVB);
	if (FAILED(hr))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	{
		m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		//m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_BufferDesc.StructureByteStride = m_iIndexStride;
		//m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
	}

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	{
		m_InitialData.pSysMem = m_Indices.data();
	}

	hr = Create_Buffer(&m_pIB);
	if (FAILED(hr))
		return E_FAIL;

	//m_pQuadTree = CQuadTree::Create(m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX, m_iNumVerticesX * m_iNumVerticesZ - 1, m_iNumVerticesX - 1, 0);
	//if (nullptr == m_pQuadTree)
	//	return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

//HRESULT CVIBuffer_Terrain::Render()
//{
//	//for (auto& Vertex : m_Vertices)
//	//{
//	//	m_pGameInstance->Add_DebugRender_Ray(Vertex.vPosition, Vertex.vNormal, true, {0.f, 1.f, 0.f, 1.f});
//	//}
//	return S_OK;
//}

//void CVIBuffer_Terrain::Culling(_fmatrix WorldMatrix)
//{
//	///* 절두체를 지형의 로컬스페이스로 변환해둔다. */
//	//m_pGameInstance->Transform_Frustum_LocalSpace(WorldMatrix);
//
//	//D3D11_MAPPED_SUBRESOURCE		SubResource{};
//
//	//m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
//
//	//_uint* pIndices = static_cast<_uint*>(SubResource.pData);
//
//	//_uint		iNumIndices = {};
//
//	////m_pQuadTree->Culling(m_pGameInstance, m_pVertexPositions, pIndices, &iNumIndices);
//
//	//m_pContext->Unmap(m_pIB, 0);
//
//	//m_iNumIndices = iNumIndices;
//}

_bool CVIBuffer_Terrain::Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist)
{
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

HRESULT CVIBuffer_Terrain::Generate_HeightMap(_float4 vLocalPickPos, _float fRadius, _float fHeightScale, _float fSigma, _bool IsUp)
{
	//굳이 지역으로 써야하나 ㅎㅎㅠ
	_uint iNumVerticesX = m_iNumVerticesX;
	_uint iNumVerticesZ = m_iNumVerticesZ;

	// 피킹한 위치의 x,z 위치만 꺼내와서 
	_float fPosX = vLocalPickPos.x;
	_float fPosZ = vLocalPickPos.z;

	// (위치/인터벌) 해서 정수부만 사용하기! 그거시 바로 터레인좌표야 
	_int iPosX = (_int)(fPosX / m_fIntervalX);
	_int iPosZ = (_int)(fPosZ / m_fIntervalZ);

	// 터레인에서의 버텍스 인덱스 구하기! 위치 인덱스를 알게 대씀ㅋㅋ
	_uint iPickIdx = iPosZ * iNumVerticesX + iPosX;

	vector<_uint> vAffectedIndices; // 영향을 받을 인덱스 벧터

	//_float fRadius = 10.f;// 인자로 받아오장..
	_float fRadiusSq = fRadius * fRadius; // 반지름 제곱 지름. sqrt은 연산비용이 좀 세대여 그래서 일케 곱하는게 낫다고 함 

	// for문 범위 계산 정확하게 조정
	for (_int i = (_int)floor(iPosX - fRadius); i <= (_int)ceil(iPosX + fRadius); ++i)
	{
		for (_int j = (_int)floor(iPosZ - fRadius); j <= (_int)ceil(iPosZ + fRadius); ++j)
		{
			//if (i >= iNumVerticesX || j >= iNumVerticesZ)
			if (i < 0 || j < 0 || i >= (_int)iNumVerticesX || j >= (_int)iNumVerticesZ)
				continue;

			_float fDistSq = (_float(i) - _float(iPosX)) * (_float(i) - _float(iPosX)) +
				(_float(j) - _float(iPosZ)) * (_float(j) - _float(iPosZ));

			if (fDistSq <= fRadiusSq)
			{
				_uint iVertexIdx = j * iNumVerticesX + i;
				vAffectedIndices.push_back(iVertexIdx);
			}
		}
	}
	//_float fSigma = fRadius / 1.f; // fSigma 값을 줄여서 가중치 감소를 더 급격히
	//_float fSigma = 1.f; // fSigma 값을 줄여서 가중치 감소를 더 급격히
	_float fTwoSigmaSq = 2.0f * fSigma * fSigma;  // 가우시안 계산을 위한 값

	for (auto& idx : vAffectedIndices) // 범위 내 점들
	{
		_uint iX = idx % iNumVerticesX; // 범내점 위치를 구하는 거임
		_uint iZ = idx / iNumVerticesX;

		// 거리 계산
		_float fDistSq = (_float(iX) - _float(iPosX)) * (_float(iX) - _float(iPosX)) + (_float(iZ) - _float(iPosZ)) * (_float(iZ) - _float(iPosZ));

		// 가우시안 가중치 계산
		_float fWeight = exp(-fDistSq / fTwoSigmaSq);

		// 높이값 증가 (원하는 비율로 조정 가능)
		if (IsUp)
		{
			m_Vertices[idx].vPosition.y += fWeight * fHeightScale; /*비율 조정 가능*/
		}
		else if (!IsUp)
		{
			m_Vertices[idx].vPosition.y -= fWeight * fHeightScale; /*비율 조정 가능*/
			//if (m_Vertices[idx].vPosition.y < 0)
			//	m_Vertices[idx].vPosition.y = 0.f;
		}
	}

	for (_uint i = 0; i < vAffectedIndices.size(); i++)
	{
		m_Vertices[vAffectedIndices[i]].vNormal = _float3(0.f, 0.f, 0.f);
	}

	// 노멀 벡터 다시 계산
	//for (auto& idx : vAffectedIndices) // 범위 내 점들
	for (_uint idx = 0; idx < m_Vertices.size(); idx++) // 범위 내 점들
	{
		_uint iX = idx % iNumVerticesX; // 범내점 위치를 구하는 거임
		_uint iZ = idx / iNumVerticesX;

		if (iX > 0 && iX < iNumVerticesX - 1 &&
			iZ > 0 && iZ < iNumVerticesZ - 1)
		{
			_uint RectIndex[4] =
			{
				idx + m_iNumVerticesX,
				idx + m_iNumVerticesX + 1,
				idx + 1,
				idx,
			};

			_vector vSrc{};
			_vector vDst{};
			_vector vNormal{};

			vSrc = XMLoadFloat3(&m_Vertices[RectIndex[1]].vPosition) - XMLoadFloat3(&m_Vertices[RectIndex[0]].vPosition);
			vDst = XMLoadFloat3(&m_Vertices[RectIndex[2]].vPosition) - XMLoadFloat3(&m_Vertices[RectIndex[0]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&m_Vertices[RectIndex[0]].vNormal, XMLoadFloat3(&m_Vertices[RectIndex[0]].vNormal) + vNormal);
			XMStoreFloat3(&m_Vertices[RectIndex[1]].vNormal, XMLoadFloat3(&m_Vertices[RectIndex[1]].vNormal) + vNormal);
			XMStoreFloat3(&m_Vertices[RectIndex[2]].vNormal, XMLoadFloat3(&m_Vertices[RectIndex[2]].vNormal) + vNormal);

			vSrc = XMLoadFloat3(&m_Vertices[RectIndex[2]].vPosition) - XMLoadFloat3(&m_Vertices[RectIndex[0]].vPosition);
			vDst = XMLoadFloat3(&m_Vertices[RectIndex[3]].vPosition) - XMLoadFloat3(&m_Vertices[RectIndex[0]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&m_Vertices[RectIndex[0]].vNormal, XMLoadFloat3(&m_Vertices[RectIndex[0]].vNormal) + vNormal);
			XMStoreFloat3(&m_Vertices[RectIndex[2]].vNormal, XMLoadFloat3(&m_Vertices[RectIndex[2]].vNormal) + vNormal);
			XMStoreFloat3(&m_Vertices[RectIndex[3]].vNormal, XMLoadFloat3(&m_Vertices[RectIndex[3]].vNormal) + vNormal);
		}
	}

	for (auto& idx : vAffectedIndices)
	{
		if (idx % iNumVerticesX == 0 || idx % iNumVerticesX == iNumVerticesX - 1 ||
			idx / iNumVerticesX == 0 || idx / iNumVerticesX == iNumVerticesZ - 1)
		{
			// 테두리 버텍스의 노멀을 주변과 스무딩 처리
			_float3 vSmoothedNormal = {};
			XMStoreFloat3(&vSmoothedNormal, XMLoadFloat3(&m_Vertices[idx].vNormal) * 0.8f + XMVectorSet(0.f, 1.f, 0.f, 1.f) * 0.2f);
			XMStoreFloat3(&m_Vertices[idx].vNormal, XMVector3Normalize(XMLoadFloat3(&vSmoothedNormal)));
		}
	}


	for (auto& Vtx : m_Vertices)
	{
		XMStoreFloat3(&Vtx.vNormal, XMVector3Normalize(XMLoadFloat3(&Vtx.vNormal)));
	}


	D3D11_MAPPED_SUBRESOURCE tSubResource = {};

	//m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &tSubResource);
	VTXNORTEX* pVertices = reinterpret_cast<VTXNORTEX*>(tSubResource.pData);

	for (auto& idx : vAffectedIndices)
	{
		pVertices[idx].vPosition.y = m_Vertices[idx].vPosition.y; // 높이 업데이트
		//pVertices[idx].vNormal.y = m_Vertices[idx].vNormal.y; // 노말 업데이트
	}
	_uint iCount = {};
	for (auto& Vertex : m_Vertices)
	{
		pVertices[iCount++].vNormal = Vertex.vNormal; // 노말 업데이트
	}
	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Save_HeightMap(const _tchar* pHeightMapFile)
{
	HANDLE hFile = CreateFile(pHeightMapFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	DWORD dwWritten = 0;
	_uint iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	WriteFile(hFile, &m_iNumVerticesX, sizeof(_uint), &dwWritten, nullptr);
	WriteFile(hFile, &m_iNumVerticesZ, sizeof(_uint), &dwWritten, nullptr);
	WriteFile(hFile, &iNumVertices, sizeof(_uint), &dwWritten, nullptr);

	vector<VTXNORTEX> vHeightData(iNumVertices);
	for (_uint i = 0; i < iNumVertices; ++i)
	{
		vHeightData[i].vPosition = m_Vertices[i].vPosition;
		vHeightData[i].vNormal = m_Vertices[i].vNormal;
		vHeightData[i].vTexCoord = m_Vertices[i].vTexCoord;
	}

	WriteFile(hFile, vHeightData.data(), sizeof(VTXNORTEX) * iNumVertices, &dwWritten, nullptr);
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Load_HeightMap(const _tchar* pHeightMapFile)
{
	_bool bRead = false;
	HANDLE hFile = CreateFile(pHeightMapFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	DWORD dwRead = 0;
	_uint iNumVerticesX = {};
	_uint iNumVerticesZ = {};
	_uint iNumVertices = {};
	bRead = ReadFile(hFile, &iNumVerticesX, sizeof(_uint), &dwRead, nullptr);
	bRead = ReadFile(hFile, &iNumVerticesZ, sizeof(_uint), &dwRead, nullptr); 
	bRead = ReadFile(hFile, &iNumVertices, sizeof(_uint), &dwRead, nullptr);

	vector<VTXNORTEX> vHeightData(iNumVertices);
	bRead = ReadFile(hFile, vHeightData.data(), sizeof(VTXNORTEX) * iNumVertices, &dwRead, nullptr);
	// 높이 데이터 적용
	for (_uint i = 0; i < iNumVertices; ++i)
	{
		m_Vertices[i].vPosition = vHeightData[i].vPosition;
		m_Vertices[i].vNormal = vHeightData[i].vNormal;
		m_Vertices[i].vTexCoord = vHeightData[i].vTexCoord;
	}
	CloseHandle(hFile);

	// GPU 버퍼 업데이트
	D3D11_MAPPED_SUBRESOURCE tSubResource;
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &tSubResource);

	VTXNORTEX* pVertices = reinterpret_cast<VTXNORTEX*>(tSubResource.pData);
	for (_uint i = 0; i < iNumVertices; ++i)
	{
		pVertices[i].vPosition = m_Vertices[i].vPosition;
		pVertices[i].vNormal = m_Vertices[i].vNormal;
		pVertices[i].vTexCoord = m_Vertices[i].vTexCoord;
	}

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}


CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFile, _bool isBMPFile)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (true == isBMPFile)
	{
		if (FAILED(pInstance->Initialize_Prototype_From_BMP(pHeightMapFile)))
		{
			MSG_BOX("Failed To Created : CVIBuffer_Terrain");
			Safe_Release(pInstance);
		}
	}
	else
	{
		if (FAILED(pInstance->Initialize_Prototype_From_Height(pHeightMapFile)))
		{
			MSG_BOX("Failed To Created : CVIBuffer_Terrain");
			Safe_Release(pInstance);
		}
	}


	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumVerticesX, _uint iNumVerticesZ)
{
	MSG_BOX("Not Impled");
	return nullptr;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	super::Free();
}
