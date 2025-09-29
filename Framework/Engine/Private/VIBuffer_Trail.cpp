#include "VIBuffer_Trail.h"
#include "GameInstance.h"
#include "Shader_Compute.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& Prototype)
	: super(Prototype)
{	
}

HRESULT CVIBuffer_Trail::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	m_pParentWorldMatrix = pDesc->pWorldMatrix;
	m_fSize = pDesc->fSize;
	m_fInterval = pDesc->fInterval;
	m_fIntervalAcc = 0.f;
	m_vLifeTime = pDesc->vLifeTime;
	m_iMaxVtxNum = pDesc->iMaxVtxNum;

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pDesc->iMaxVtxNum;
	m_iVertexStride = sizeof(VTXTRAIL);
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, nullptr, &m_pVB)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Trail::Bind_InputAssembler()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
	};

	_uint iVertexStrides[] = {
		m_iVertexStride,
	};

	_uint iOffsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

HRESULT CVIBuffer_Trail::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	m_pContext->Draw(static_cast<_uint>(m_TrailVertices.size()), 0);
	return S_OK;
}


_bool CVIBuffer_Trail::Progress(_float fTimeDelta)
{
	VTXTRAIL TrailVertex{};
	TrailVertex.vPosition = _float3(m_pParentWorldMatrix->_41, m_pParentWorldMatrix->_42, m_pParentWorldMatrix->_43);
	TrailVertex.vLifeTime = m_vLifeTime;

	if(true == m_TrailVertices.empty())
		m_TrailVertices.push_back(TrailVertex);
	else
	{
		m_TrailVertices.push_back(TrailVertex);
		m_TrailVertices.push_back(TrailVertex);
	}

	if (m_TrailVertices.size() > m_iMaxVtxNum)
	{
		m_TrailVertices.pop_front();
		m_TrailVertices.pop_front();
	}

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	VTXTRAIL* Dest = static_cast<VTXTRAIL*>(MappedResource.pData);
	for (auto& TrailVertex : m_TrailVertices)
	{
		TrailVertex.vLifeTime.y += fTimeDelta;
		*Dest++ = TrailVertex;
	}

	m_pContext->Unmap(m_pVB, 0);

	return true;
}

void CVIBuffer_Trail::Reset_Life()
{
	m_TrailVertices.clear();
}

CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CVIBuffer_Trail::Clone(void* pArg)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	super::Free();
}
