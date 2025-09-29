#pragma once
#include "VIBuffer.h"


BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
	using super = CVIBuffer;

public:
	struct DESC
	{
		const _float4x4*	pWorldMatrix = {}; // 정점 위치 정보를 추가하기 위한 부모의 월드 매트릭스
		_uint				iMaxVtxNum = {};	// 버퍼에 삽입할 정점의 최대 개수, 최대 개수를 넘으면 가장 오래된 정점을 버퍼에서 삭제 : 
		_float				fSize = {};			// 높이 : 0.30
		_float				fInterval = {};		// 너비, 가장 최근 삽입된 정점과의 거리가 fInterval 이상인 경우에만 버퍼에 추가 : 0.05
		_float2				vLifeTime = {};		// 수명, 각 정점은 수명이 다하면 버퍼에서 삭제 : 0.25
	};

	struct TrailPoint
	{
		_float3				vPosition{};
		_float2				vLifeTime{};

	};

private:
	CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& Prototype);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_InputAssembler() override;
	virtual HRESULT Render() override;

	virtual _bool Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist) override { return FALSE; };

public:
	_bool	Progress(_float fTimeDelta);
	void	Reset_Life();

private:
	std::deque<VTXTRAIL>			m_TrailVertices;
	
	const _float4x4*				m_pParentWorldMatrix{nullptr};
	_uint							m_iMaxVtxNum{};
	_float							m_fSize{};
	_float							m_fInterval{};
	_float							m_fIntervalAcc{};
	_float2							m_vLifeTime{};


public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg);
	virtual void Free();
};

END