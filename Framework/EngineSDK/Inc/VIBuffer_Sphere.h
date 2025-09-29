#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Sphere final : public CVIBuffer
{
	using super = CVIBuffer;

private:
	CVIBuffer_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Sphere(const CVIBuffer_Sphere& Prototype);
	virtual ~CVIBuffer_Sphere() = default;

	virtual _bool Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist) override;

public:
	virtual HRESULT Initialize_Prototype(_uint iLatitudeSegments, _uint iLongitudeSegments);
	virtual HRESULT Initialize(void* pArg) override;


private:
	std::vector<VTXPOSTEX> m_Vertices;
	std::vector<_uint> m_Indices;

public:
	static CVIBuffer_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLatitudeSegments = 20, _uint iLongitudeSegments = 20);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END