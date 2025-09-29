#include "VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube& Prototype)
    : super(Prototype)
{
}

_bool CVIBuffer_Cube::Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist)
{
	// todo imple ray cast with cube mesh
	return FALSE;
}

HRESULT CVIBuffer_Cube::Initialize_Prototype()
{
	m_iNumVertices = 8;
	m_iVertexStride = sizeof(VTXCUBE);
	m_iNumIndices = 36;
	m_iIndexStride = 2;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	m_Vertices.resize(m_iNumVertices);

	m_Vertices[0].vPosition = _float3(-1.f, 1.f, -1.f);
	m_Vertices[0].vTexCoord = m_Vertices[0].vPosition;

	m_Vertices[1].vPosition = _float3(1.f, 1.f, -1.f);
	m_Vertices[1].vTexCoord = m_Vertices[1].vPosition;

	m_Vertices[2].vPosition = _float3(1.f, -1.f, -1.f);
	m_Vertices[2].vTexCoord = m_Vertices[2].vPosition;

	m_Vertices[3].vPosition = _float3(-1.f, -1.f, -1.f);
	m_Vertices[3].vTexCoord = m_Vertices[3].vPosition;

	// ÈÄ¸é
	m_Vertices[4].vPosition = _float3(-1.f, 1.f, 1.f);
	m_Vertices[4].vTexCoord = m_Vertices[4].vPosition;

	m_Vertices[5].vPosition = _float3(1.f, 1.f, 1.f);
	m_Vertices[5].vTexCoord = m_Vertices[5].vPosition;

	m_Vertices[6].vPosition = _float3(1.f, -1.f, 1.f);
	m_Vertices[6].vTexCoord = m_Vertices[6].vPosition;

	m_Vertices[7].vPosition = _float3(-1.f, -1.f, 1.f);
	m_Vertices[7].vTexCoord = m_Vertices[7].vPosition;


	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_Vertices.data();

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

	m_Indices.resize(m_iNumIndices);


	m_Indices[0] = 1; m_Indices[1] = 5; m_Indices[2] = 6;
	m_Indices[3] = 1; m_Indices[4] = 6; m_Indices[5] = 2;

	m_Indices[6] = 4; m_Indices[7] = 0; m_Indices[8] = 3;
	m_Indices[9] = 4; m_Indices[10] = 3; m_Indices[11] = 7;


	m_Indices[12] = 4; m_Indices[13] = 5; m_Indices[14] = 1;
	m_Indices[15] = 4; m_Indices[16] = 1; m_Indices[17] = 0;


	m_Indices[18] = 3; m_Indices[19] = 2; m_Indices[20] = 6;
	m_Indices[21] = 3; m_Indices[22] = 6; m_Indices[23] = 7;


	m_Indices[24] = 7; m_Indices[25] = 6; m_Indices[26] = 5;
	m_Indices[27] = 7; m_Indices[28] = 5; m_Indices[29] = 4;


	m_Indices[30] = 0; m_Indices[31] = 1; m_Indices[32] = 2;
	m_Indices[33] = 0; m_Indices[34] = 2; m_Indices[35] = 3;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_Indices.data();

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Cube::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Cube* CVIBuffer_Cube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CVIBuffer_Cube");
		Safe_Release(pInstance);
	}
	         
	return pInstance;
}

CCloneable* CVIBuffer_Cube::Clone(void* pArg)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CVIBuffer_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	super::Free();
}
