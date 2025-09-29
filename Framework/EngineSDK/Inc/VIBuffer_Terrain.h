#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
	using super = CVIBuffer;

private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype_From_BMP(const _tchar* pHeightMapFile);
	virtual HRESULT Initialize_Prototype_From_Height(const _tchar* pHeightMapFile);
	virtual HRESULT Initialize(void* pArg) override;
	//virtual HRESULT Render() override;

	//void Culling(_fmatrix WorldMatrix); // 쿼드트리인대 진자 안슬가??
	//HeightMap
	HRESULT Generate_HeightMap(_float4 vLocalPickPos, _float fRadius, _float fHeightScale, _float fSigma, _bool IsUp);
	HRESULT Save_HeightMap(const _tchar* pHeightMapFile);
	HRESULT Load_HeightMap(const _tchar* pHeightMapFile);

	const vector<VTXNORTEX>&	Get_VertexData() const { return m_Vertices; }
	const vector<_uint>&		Get_IndexData() const { return m_Indices; }
	_uint						Get_NumWidthVertices() const { return m_iNumVerticesX; }
	_uint						Get_NumDepthVertices() const { return m_iNumVerticesZ; }

	virtual _bool Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist) override;

private:
	_uint      m_iNumVerticesX{ 0 };
	_uint      m_iNumVerticesZ{ 0 };
	_float      m_fIntervalX = { 1.f };
	_float      m_fIntervalZ = { 1.f };

	vector<VTXNORTEX>      m_Vertices;
	vector<_uint>         m_Indices;

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFile, _bool isBMPFile = true);
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumVerticesX, _uint iNumVerticesZ);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
