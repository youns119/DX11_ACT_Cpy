#include "VIBuffer_MyTerrain.h"

CVIBuffer_MyTerrain::CVIBuffer_MyTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_MyTerrain::CVIBuffer_MyTerrain(const CVIBuffer_MyTerrain& Prototype)
	: CVIBuffer { Prototype }
{
}

HRESULT CVIBuffer_MyTerrain::Initialize_Prototype()
{
	m_iX = 32;
	m_iZ = 32;
	HRESULT hr{};

	m_iNumVertices = m_iX * m_iZ;
	m_iVertexStride = sizeof(VTXPOSTEX);

	m_iNumIndices = (m_iX - 1) * (m_iZ - 1) * 3 * 2;
	m_iIndexStride = 4;

	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;


#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	{
		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.StructureByteStride = m_iVertexStride;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
	}

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices]{};
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);
	{
		_uint iIndex{ 0 };
		for (_uint i = 0; i < m_iZ; ++i)
		{
			for (_uint j = 0; j < m_iX; ++j)
			{
				iIndex = i * m_iX + j;
				pVertices[iIndex].vPosition = _float3(j, 0.f, i);
				pVertices[iIndex].vTexCoord = _float2(j, i);
			}
		}
	}

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	{
		m_InitialData.pSysMem = pVertices;
	}

	hr = CVIBuffer::Create_Buffer(&m_pVB);
	Safe_Delete_Array(pVertices);

	if (FAILED(hr))
		return E_FAIL;
#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	{
		m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.StructureByteStride = m_iIndexStride;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
	}

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);
	{
		_uint iVertexIndex{ 0 };
		_uint iIndex{ 0 };
		for (_uint i = 0; i < m_iZ - 1; ++i)
		{
			for (_uint j = 0; j < m_iX - 1; ++j)
			{
				iVertexIndex = i * m_iX + j;

				pIndices[iIndex] = iVertexIndex;
				++iIndex;
				pIndices[iIndex] = iVertexIndex + m_iX;
				++iIndex;
				pIndices[iIndex] = iVertexIndex + 1;
				++iIndex;

				pIndices[iIndex] = iVertexIndex + 1;
				++iIndex;
				pIndices[iIndex] = iVertexIndex + m_iX;
				++iIndex;
				pIndices[iIndex] = iVertexIndex + 1 + m_iX;
				++iIndex;
			}
		}

		int a = 0;
	}

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	{
		m_InitialData.pSysMem = pIndices;
	}

	hr = CVIBuffer::Create_Buffer(&m_pIB);
	Safe_Delete_Array(pIndices);

	if (FAILED(hr))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_MyTerrain::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	// 여기서 값을 넣어주기 이전에 이미 버퍼를 다 작업 하니까 initialize에서 값을 넣어주는 의미가 없음
	MYTERRAIN_DESC* pDesc = static_cast<MYTERRAIN_DESC*>(pArg);
	m_iX = pDesc->iX;
	m_iZ = pDesc->iZ;
	m_fTileSizeX = pDesc->fTileSizeX;
	m_fTileSizeZ = pDesc->fTileSizeZ;

	return S_OK;
}

CVIBuffer_MyTerrain* CVIBuffer_MyTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

	CVIBuffer_MyTerrain* pInstance = new CVIBuffer_MyTerrain{ pDevice, pContext };

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CVIBuffer_MyTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_MyTerrain::Clone(void* pArg)
{
	CVIBuffer_MyTerrain* pInstance = new CVIBuffer_MyTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CVIBuffer_MyTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_MyTerrain::Free()
{
	CVIBuffer::Free();
}
