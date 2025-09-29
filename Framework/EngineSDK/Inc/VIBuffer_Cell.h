#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class CVIBuffer_Cell final : public CVIBuffer
{
	using super = CVIBuffer;

private:
	CVIBuffer_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Cell(const CVIBuffer_Cell& Prototype);
	virtual ~CVIBuffer_Cell() = default;

public:
	virtual HRESULT Initialize_Prototype(const _float3* pPoints);
	virtual HRESULT Initialize(void* pArg) override;

	virtual _bool Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist) override { return FALSE; };

	void Update(const _float3* pPoints, _uint iNumPoints);

public:
	static CVIBuffer_Cell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
