#pragma once

#include "VIBuffer_InstancingParticle.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle_Rect final : public CVIBuffer_InstancingParticle
{
	using super = CVIBuffer_InstancingParticle;

public:
	struct DESC
	{
		super::DESC InstancingParticleDesc {};
	};

private:
	CVIBuffer_Particle_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Particle_Rect(const CVIBuffer_Particle_Rect& Prototype);
	virtual ~CVIBuffer_Particle_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype(const super::DESC& Desc) override;
	virtual HRESULT Initialize(void* pArg) override;

private:

public:
	static CVIBuffer_Particle_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const super::DESC& Desc);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
