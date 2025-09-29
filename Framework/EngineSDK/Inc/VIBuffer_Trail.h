#pragma once
#include "VIBuffer.h"


BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
	using super = CVIBuffer;

public:
	struct DESC
	{
		const _float4x4*	pWorldMatrix = {}; // ���� ��ġ ������ �߰��ϱ� ���� �θ��� ���� ��Ʈ����
		_uint				iMaxVtxNum = {};	// ���ۿ� ������ ������ �ִ� ����, �ִ� ������ ������ ���� ������ ������ ���ۿ��� ���� : 
		_float				fSize = {};			// ���� : 0.30
		_float				fInterval = {};		// �ʺ�, ���� �ֱ� ���Ե� �������� �Ÿ��� fInterval �̻��� ��쿡�� ���ۿ� �߰� : 0.05
		_float2				vLifeTime = {};		// ����, �� ������ ������ ���ϸ� ���ۿ��� ���� : 0.25
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