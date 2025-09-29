#include "VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CVIBuffer_Rect::Initialize_Prototype()
{
	HRESULT hResult{};

	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTXPOSTEX);

	m_iNumIndices = 6;
	m_iIndexStride = 2;

	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;

	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexCoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexCoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexCoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexCoord = _float2(0.f, 1.f);

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	m_InitialData.pSysMem = pVertices;

	hResult = CVIBuffer::Create_Buffer(&m_pVB);
	Safe_Delete_Array(pVertices);

	if (FAILED(hResult))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices]{};
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	m_InitialData.pSysMem = pIndices;

	hResult = CVIBuffer::Create_Buffer(&m_pIB);
	Safe_Delete_Array(pIndices);

	if (FAILED(hResult))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect::Initialize(void* pArg)
{
	return S_OK;
}

_bool CVIBuffer_Rect::Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist)
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

CVIBuffer_Rect* CVIBuffer_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Rect::Clone(void* pArg)
{
	CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect::Free()
{
	CVIBuffer::Free();
}
