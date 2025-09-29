#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_MyTerrain final : public CVIBuffer
{
public:
	struct MYTERRAIN_DESC
	{
		_uint		iX{ 0 };
		_uint		iZ{ 0 };

		_float		fTileSizeX{ 0.f };
		_float		fTileSizeZ{ 0.f };
	};

private:
	CVIBuffer_MyTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_MyTerrain(const CVIBuffer_MyTerrain& Prototype);
	virtual ~CVIBuffer_MyTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	_uint		m_iX{ 0 };
	_uint		m_iZ{ 0 };
	_float		m_fTileSizeX{ 0.f };
	_float		m_fTileSizeZ{ 0.f };

public:
	static CVIBuffer_MyTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
