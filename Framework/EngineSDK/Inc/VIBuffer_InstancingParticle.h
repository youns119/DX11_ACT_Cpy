#pragma once

#include "VIBuffer.h"

BEGIN(Engine)
class CShader_Compute;

class ENGINE_DLL CVIBuffer_InstancingParticle abstract : public CVIBuffer
{
	using super = CVIBuffer;

public:
	enum PARTICLE_PATTERN 
	{ 
		EXPLOSION, DROP, RISE, FRACTION, HOLD, 
		RISE_WATER, CHARGING, RISE_WATER_ENV, 
		ASH_FLOW, CROSS_SPREAD, GOD_RAY,
		EXPL_SLOWDOWN, EXPL_SCALEGRADIENT,
		PATTERN_END 
	};
	
public:
	struct DESC
	{
		_uint			iNumInstance = {};
		_float3			vRange = {};
		_float3			vCenter = {};
		_float2			vSize = {};
		_float2			vSpeed = {};
		_float2			vRotation = {}; // Degree
		_float3			vPivot = {};
		_float2			vLifeTime = {};
		_bool			isLoop = {};

		_uint			iPattern = {};
	};

	struct CONST_BUFFER // 32
	{
		_int     iNumInstance;     // 4
		_float   fTimeDelta;       // 8
		_int     bIsLoop;          // 12
		_int     bIsLoopEnd;       // 16

		_float4  vPivot;           // 16
	};

protected:
	CVIBuffer_InstancingParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_InstancingParticle(const CVIBuffer_InstancingParticle& Prototype);
	virtual ~CVIBuffer_InstancingParticle() = default;

public:
	virtual HRESULT Initialize_Prototype(const DESC& pDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_InputAssembler() override;
	virtual HRESULT Render() override;

	virtual _bool Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist) override { return FALSE; }

public:
	_bool	Progress(_float fTimeDelta);

	void	Change_Pattern(_uint iPattern);

	void	Loop_End() { m_isLoopEnd = true; }
	void	Set_Pivot(_fvector vPivot) { XMStoreFloat3(&m_vPivot, vPivot); }
	void	Reset_Life();

	const vector<VTXPARTICLE>& Get_ParticleInfo() const { return m_ParticleVertices; }

private:
	void	Create_CS(_uint iPattern);

protected:
	CShader_Compute*				m_pComputeShader{ nullptr };
	ID3D11Buffer*					m_pVBInstance { nullptr };

	D3D11_BUFFER_DESC				m_InstanceBufferDesc {};
	D3D11_SUBRESOURCE_DATA			m_InstanceInitialData {};

	_uint							m_iInstanceVertexStride { 0 };
	_uint							m_iNumIndexPerInstance { 0 };
	_uint							m_iNumInstance { 0 };

	vector<VTXPARTICLE>				m_ParticleVertices;

	_uint							m_iPattern{};
	vector<_float>					m_Speeds;
	vector<_float>					m_Rotations;
	_bool							m_bRotation{ false };

	_bool							m_isLoop { false };
	_bool							m_isLoopEnd { false };

	_float3							m_vPivot {};

	_bool							m_bIsRenderable{};

public:
	virtual CCloneable* Clone(void* pArg) = 0;
	virtual void Free();
};

END
