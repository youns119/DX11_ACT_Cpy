#include "VIBuffer_Cell.h"

CVIBuffer_Cell::CVIBuffer_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CVIBuffer_Cell::CVIBuffer_Cell(const CVIBuffer_Cell& Prototype)
	: super(Prototype)
{
}

HRESULT CVIBuffer_Cell::Initialize_Prototype(const _float3* pPoints)
{
	m_iNumVertices = 3;
	m_iVertexStride = sizeof(VTXPOS);
	m_iNumIndices = 4; // 0, 1, 2, 0
	m_iIndexStride = 2;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	//m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	//m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	//m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	//m_Vertices.reserve(m_iNumVertices); // 피킹 안할거니까 그냥 멤버에 저장안할게 // 멤버도 없다어짜피
	vector<VTXPOS> Vertices;
	Vertices.reserve(m_iNumVertices);

	memcpy(Vertices.data(), pPoints, sizeof(_float3) * 3);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	//m_InitialData.pSysMem = pPoints;
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

	//vector<_ushort> Indices { 0, 1, 2, 0 };
	vector<_ushort> Indices { 0, 1, 2 };

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = Indices.data();

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Cell::Initialize(void* pArg)
{
	return S_OK;
}

void CVIBuffer_Cell::Update(const _float3* pPoints, _uint iNumPoints)
{
	D3D11_MAPPED_SUBRESOURCE SubResource {};

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	{
		memcpy(SubResource.pData, pPoints, sizeof(_float3) * iNumPoints);
	}
	m_pContext->Unmap(m_pVB, 0);
}

CVIBuffer_Cell* CVIBuffer_Cell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints)
{
	CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pPoints)))
	{
		MSG_BOX("Failed To Create : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Cell::Clone(void* pArg)
{
	CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cell::Free()
{
	super::Free();
}
