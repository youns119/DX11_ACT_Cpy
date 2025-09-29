#include "VIBuffer_Particle_Rect.h"

#include "GameInstance.h"

CVIBuffer_Particle_Rect::CVIBuffer_Particle_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super( pDevice, pContext )
{
}

CVIBuffer_Particle_Rect::CVIBuffer_Particle_Rect(const CVIBuffer_Particle_Rect& Prototype)
	: super( Prototype )
{
}

HRESULT CVIBuffer_Particle_Rect::Initialize_Prototype(const super::DESC& Desc)
{
	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTXPOSTEX);

	m_iNumInstance = Desc.iNumInstance;
	m_iInstanceVertexStride = sizeof(VTXPARTICLE);

	m_iNumIndexPerInstance = 6;

	//m_iNumIndices = m_iNumIndexPerInstance * m_iNumInstance;
	m_iNumIndices = m_iNumIndexPerInstance;
	m_iNumIndices = m_iNumIndexPerInstance;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;

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

	vector<VTXPOSTEX> Vertices;
	Vertices.resize(m_iNumVertices);

	Vertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	Vertices[0].vTexCoord = _float2(0.f, 0.f);

	Vertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	Vertices[1].vTexCoord = _float2(1.f, 0.f);

	Vertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	Vertices[2].vTexCoord = _float2(1.f, 1.f);

	Vertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	Vertices[3].vTexCoord = _float2(0.f, 1.f);

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

	vector<_ushort> Indices;
	Indices.resize(m_iNumIndices);

	_uint		iNumInstance = { 0 };

	//for (size_t i = 0; i < m_iNumInstance; i++)
	{
		Indices[iNumInstance++] = 0;
		Indices[iNumInstance++] = 1;
		Indices[iNumInstance++] = 2;

		Indices[iNumInstance++] = 0;
		Indices[iNumInstance++] = 2;
		Indices[iNumInstance++] = 3;
	}



	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = Indices.data();

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
#pragma endregion


#pragma region INSTANCE_BUFFER
	/* 사봔을 만들때 실제 활용하기위한 인스턴스 정점 버퍼를 할당하기위해 필요한 초기 데이터를 미리 다 셋팅해놓는다.  */
	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
	m_InstanceBufferDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_ParticleVertices.resize(m_iNumInstance);
	
	m_Speeds.resize(m_iNumInstance);

	m_isLoop = Desc.isLoop;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float4 vTranslation = {};

		vTranslation.x = CUtils::Compute_Random(Desc.vCenter.x - Desc.vRange.x * 0.5f, Desc.vCenter.x + Desc.vRange.x * 0.5f);
		vTranslation.y = CUtils::Compute_Random(Desc.vCenter.y - Desc.vRange.y * 0.5f, Desc.vCenter.y + Desc.vRange.y * 0.5f);
		vTranslation.z = CUtils::Compute_Random(Desc.vCenter.z - Desc.vRange.z * 0.5f, Desc.vCenter.z + Desc.vRange.z * 0.5f);
		vTranslation.w = 1;

		m_Speeds[i] = CUtils::Compute_Random(Desc.vSpeed.x, Desc.vSpeed.y);


		m_ParticleVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		m_ParticleVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		m_ParticleVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		m_ParticleVertices[i].vTranslation = vTranslation;
		m_ParticleVertices[i].vLifeTime.x = CUtils::Compute_Random(Desc.vLifeTime.x, Desc.vLifeTime.y);

		_float fScale = CUtils::Compute_Random(Desc.vSize.x, Desc.vSize.y);

		XMStoreFloat4(&m_ParticleVertices[i].vRight, XMLoadFloat4(&m_ParticleVertices[i].vRight) * fScale);
		XMStoreFloat4(&m_ParticleVertices[i].vUp, XMLoadFloat4(&m_ParticleVertices[i].vUp) * fScale);
		XMStoreFloat4(&m_ParticleVertices[i].vLook, XMLoadFloat4(&m_ParticleVertices[i].vLook) * fScale);
	}

	m_InstanceInitialData.pSysMem = m_ParticleVertices.data();
#pragma endregion


	return S_OK;
}

HRESULT CVIBuffer_Particle_Rect::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
			
	if (FAILED(super::Initialize(&pDesc->InstancingParticleDesc)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Particle_Rect* CVIBuffer_Particle_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const super::DESC& Desc)
{
	CVIBuffer_Particle_Rect* pInstance = new CVIBuffer_Particle_Rect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc)))
	{
		MSG_BOX("Failed To Create : CVIBuffer_Particle_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CVIBuffer_Particle_Rect::Clone(void* pArg)
{
	CVIBuffer_Particle_Rect* pInstance = new CVIBuffer_Particle_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CVIBuffer_Particle_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Particle_Rect::Free()
{
	super::Free();
}
