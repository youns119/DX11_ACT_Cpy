#include "VIBuffer_Rope.h"
#include "GameInstance.h"
#include "Shader_Compute.h"

CVIBuffer_Rope::CVIBuffer_Rope(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CVIBuffer_Rope::CVIBuffer_Rope(const CVIBuffer_Rope& Prototype)
	: super(Prototype)
{
}

HRESULT CVIBuffer_Rope::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CVIBuffer_Rope::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	m_pParentWorldMatrix = pDesc->pWorldMatrix;
	m_fSize = pDesc->fSize;

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 2;
	m_iVertexStride = sizeof(VTXPOS);
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

HRESULT CVIBuffer_Rope::Bind_InputAssembler()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	if (2 > m_RopeVertices.size())
		return S_OK;

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

HRESULT CVIBuffer_Rope::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	if (2 > m_RopeVertices.size())
		return S_OK;

	m_pContext->Draw(2, 0);
	return S_OK;
}


void CVIBuffer_Rope::Set_Info(_fvector vTargetPos)
{
	m_RopeVertices.clear();
	VTXPOS VtxPos{};
	VtxPos.vPosition = _vec3(vTargetPos)._float3();
	m_RopeVertices.push_back(VtxPos);
}

_bool CVIBuffer_Rope::Progress(_float fTimeDelta)
{
	if (true == m_RopeVertices.empty())
		return false;

	VTXPOS VtxPos{};
	VtxPos.vPosition = _float3(m_pParentWorldMatrix->_41, m_pParentWorldMatrix->_42, m_pParentWorldMatrix->_43);

	if (1 == m_RopeVertices.size())
		m_RopeVertices.push_back(VtxPos);
	else
		m_RopeVertices.back().vPosition = VtxPos.vPosition;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	VTXPOS* Dest = static_cast<VTXPOS*>(MappedResource.pData);
	memcpy(MappedResource.pData, m_RopeVertices.data(), sizeof(VTXPOS) * m_RopeVertices.size());
	m_pContext->Unmap(m_pVB, 0);

	return true;
}

void CVIBuffer_Rope::Reset_Life()
{
	m_RopeVertices.clear();
}

CVIBuffer_Rope* CVIBuffer_Rope::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Rope* pInstance = new CVIBuffer_Rope(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CVIBuffer_Rope");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CVIBuffer_Rope::Clone(void* pArg)
{
	CVIBuffer_Rope* pInstance = new CVIBuffer_Rope(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CVIBuffer_Rope");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rope::Free()
{
	super::Free();
}
