#include "VIBuffer_Sphere.h"

CVIBuffer_Sphere::CVIBuffer_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CVIBuffer_Sphere::CVIBuffer_Sphere(const CVIBuffer_Sphere& Prototype)
	: super(Prototype)
{
}

_bool CVIBuffer_Sphere::Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist)
{
	return _bool();
}

HRESULT CVIBuffer_Sphere::Initialize_Prototype(_uint iLatitudeSegments, _uint iLongitudeSegments)
{
	m_iNumVertexBuffers = 1;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iNumVertices = (iLatitudeSegments) * (iLongitudeSegments + 1);
	m_iVertexStride = sizeof(VTXPOSTEX);

	m_iNumIndices = (iLatitudeSegments - 1) * iLongitudeSegments * 2 * 3;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;

#pragma region VERTEX_BUFFER
	vector<VTXPOSTEX> Vertices;
	 _float fRadius = 1.f;
	 
	for (_uint i = 0; i < iLatitudeSegments; ++i)
	{
		_float fTheta = XM_PI / (iLatitudeSegments - 1) * i;
		_float fSinTheta = sinf(fTheta);
		_float fCosTheta = cosf(fTheta);

		for (_uint j = 0; j <= iLongitudeSegments; ++j) // 마지막 점, 시작 점 같은 위치로
		{
			_float fPhi = XM_2PI / iLongitudeSegments * j;
			_float fSinPhi = sinf(fPhi);
			_float fCosPhi = cosf(fPhi);

			_float3 vPosition {};
			vPosition.x = fRadius * fSinTheta * fCosPhi;
			vPosition.y = fRadius * fCosTheta;
			vPosition.z = fRadius * fSinTheta * fSinPhi;

			_float2	vTexCoord {};
			vTexCoord.x = static_cast<_float>(i) / iLatitudeSegments;
			vTexCoord.y = static_cast<_float>(j) / iLongitudeSegments;

			Vertices.push_back({ vPosition, vTexCoord });
		}
	}

	m_Vertices = Vertices;
	if (Vertices.size() != m_iNumVertices)
		_CrtDbgBreak();

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_Vertices.data();

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

#pragma endregion


#pragma region INDEX_BUFFER

	vector<_uint> Indices;
	Indices.reserve(m_iNumIndices);

	for (_uint i = 0; i < iLatitudeSegments - 1; ++i)
	{
		for (_uint j = 0; j < iLongitudeSegments; ++j)
		{
			_uint iLT = i * iLongitudeSegments + j;
			_uint iRT = i * iLongitudeSegments + j + 1;
			_uint iRB = (i + 1) * iLongitudeSegments + j + 1;
			_uint iLB = (i + 1) * iLongitudeSegments + j;

			Indices.push_back(iLT);
			Indices.push_back(iRT);
			Indices.push_back(iRB);

			Indices.push_back(iLT);
			Indices.push_back(iRB);
			Indices.push_back(iLB);
		}
	}
	assert(Indices.size() == m_iNumIndices);
	m_Indices = Indices;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iIndexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	m_Indices.resize(m_iNumIndices);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_Indices.data();

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Sphere::Initialize(void* pArg)
{

	return S_OK;
}

CVIBuffer_Sphere* CVIBuffer_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLatitudeSegments, _uint iLongitudeSegments)
{
	CVIBuffer_Sphere* pInstance = new CVIBuffer_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLatitudeSegments, iLongitudeSegments)))
	{
		MSG_BOX("Failed To Create : CVIBuffer_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CVIBuffer_Sphere::Clone(void* pArg)
{
	CVIBuffer_Sphere* pInstance = new CVIBuffer_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CVIBuffer_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Sphere::Free()
{
	super::Free();
}
