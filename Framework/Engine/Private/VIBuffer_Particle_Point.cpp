#include "VIBuffer_Particle_Point.h"

#include "GameInstance.h"

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& Prototype)
	: super(Prototype)
{
}

HRESULT CVIBuffer_Particle_Point::Initialize_Prototype(const super::DESC& Desc)
{
	return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOINT);

	m_iNumInstance = pDesc->ParticleDesc.iNumInstance;
	m_iInstanceVertexStride = sizeof(VTXPARTICLE);

	m_iNumIndexPerInstance = 1;

	m_iNumIndices = m_iNumIndexPerInstance * m_iNumInstance;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;

	m_iNumVertexBuffers = 2;

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;


#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	vector<VTXPOINT> Vertices;
	Vertices.resize(m_iNumVertices);

	Vertices[0].vPosition = _float3(0.f, 0.f, 0.f);
	Vertices[0].fPSize = 1.f;


	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = Vertices.data();

	if (FAILED(super::Create_Buffer(&m_pVB)))
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

	vector<_uint> Indices;
	Indices.resize(m_iNumIndices, 0);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = Indices.data();

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
#pragma endregion


#pragma region INSTANCE_BUFFER
	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
	m_InstanceBufferDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_ParticleVertices.resize(m_iNumInstance);

	m_Speeds.resize(m_iNumInstance);

	m_isLoop = pDesc->ParticleDesc.isLoop;
	m_iPattern = pDesc->ParticleDesc.iPattern;
	m_vPivot = pDesc->ParticleDesc.vPivot;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float4 vTranslation = {};

		vTranslation.x = CUtils::Compute_Random(pDesc->ParticleDesc.vCenter.x - pDesc->ParticleDesc.vRange.x * 0.5f, pDesc->ParticleDesc.vCenter.x + pDesc->ParticleDesc.vRange.x * 0.5f);
		vTranslation.y = CUtils::Compute_Random(pDesc->ParticleDesc.vCenter.y - pDesc->ParticleDesc.vRange.y * 0.5f, pDesc->ParticleDesc.vCenter.y + pDesc->ParticleDesc.vRange.y * 0.5f);
		vTranslation.z = CUtils::Compute_Random(pDesc->ParticleDesc.vCenter.z - pDesc->ParticleDesc.vRange.z * 0.5f, pDesc->ParticleDesc.vCenter.z + pDesc->ParticleDesc.vRange.z * 0.5f);
		vTranslation.w = 1;

		m_Speeds[i] = CUtils::Compute_Random(pDesc->ParticleDesc.vSpeed.x, pDesc->ParticleDesc.vSpeed.y);

		m_ParticleVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		m_ParticleVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		m_ParticleVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		m_ParticleVertices[i].vTranslation = vTranslation;
		m_ParticleVertices[i].vLifeTime.x = CUtils::Compute_Random(pDesc->ParticleDesc.vLifeTime.x, pDesc->ParticleDesc.vLifeTime.y);
		m_ParticleVertices[i].fSpeed = m_Speeds[i];
		_float fScale = CUtils::Compute_Random(pDesc->ParticleDesc.vSize.x, pDesc->ParticleDesc.vSize.y);

		XMStoreFloat4(&m_ParticleVertices[i].vRight, XMLoadFloat4(&m_ParticleVertices[i].vRight) * fScale);
		XMStoreFloat4(&m_ParticleVertices[i].vUp, XMLoadFloat4(&m_ParticleVertices[i].vUp) * fScale);
		XMStoreFloat4(&m_ParticleVertices[i].vLook, XMLoadFloat4(&m_ParticleVertices[i].vLook) * fScale);
	}

	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
	m_InstanceInitialData.pSysMem = m_ParticleVertices.data();
#pragma endregion

	if (FAILED(super::Initialize(&pDesc->ParticleDesc)))
	//if (FAILED(super::Initialize(nullptr)))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG
void CVIBuffer_Particle_Point::Set_Desc(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	m_Speeds.clear();
	m_ParticleVertices.clear();
	m_vPivot = pDesc->ParticleDesc.vPivot;
	
	Safe_Release(m_pVBInstance);

	// 새로운 Desc 적용하여 새로운 파티클 생성
	Initialize_Prototype(pDesc->ParticleDesc);
	Initialize(pArg);
}
#endif

CVIBuffer_Particle_Point* CVIBuffer_Particle_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_InstancingParticle::DESC& Desc)
{
	CVIBuffer_Particle_Point* pInstance = new CVIBuffer_Particle_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc)))
	{
		MSG_BOX("Failed To Create : CVIBuffer_Particle_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CVIBuffer_Particle_Point::Clone(void* pArg)
{
	CVIBuffer_Particle_Point* pInstance = new CVIBuffer_Particle_Point(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CVIBuffer_Particle_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Particle_Point::Free()
{
	super::Free();
}
