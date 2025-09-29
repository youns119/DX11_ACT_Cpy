#pragma once
#include "VIBuffer.h"


BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Rope final : public CVIBuffer
{
	using super = CVIBuffer;

public:
	struct DESC
	{
		const _float4x4*	pWorldMatrix = {};	// 정점 위치 정보를 추가하기 위한 부모의 월드 매트릭스
		_float				fSize = {};			// 높이 : 0.30
	};

private:
	CVIBuffer_Rope(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Rope(const CVIBuffer_Rope& Prototype);
	virtual ~CVIBuffer_Rope() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_InputAssembler() override;
	virtual HRESULT Render() override;

	virtual _bool Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist) override { return FALSE; };

public:
	void	Set_Info(_fvector vTargetPos);
	_bool	Progress(_float fTimeDelta);
	void	Reset_Life();

private:
	const _float4x4*				m_pParentWorldMatrix{ nullptr };

	vector<VTXPOS>					m_RopeVertices;
	_float							m_fSize{};

public:
	static CVIBuffer_Rope* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg);
	virtual void Free();
};

END