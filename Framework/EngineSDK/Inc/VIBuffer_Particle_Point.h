#pragma once

#include "VIBuffer_InstancingParticle.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle_Point final : public CVIBuffer_InstancingParticle
{
	using super = CVIBuffer_InstancingParticle;

public:
	struct DESC
	{
		CVIBuffer_InstancingParticle::DESC ParticleDesc;
	};

private:
	CVIBuffer_Particle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& Prototype);
	virtual ~CVIBuffer_Particle_Point() = default;

public:
	virtual HRESULT Initialize_Prototype(const super::DESC& Desc) override;
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
public:
	void	Set_Desc(void* pArg);
#endif


public:
	static CVIBuffer_Particle_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_InstancingParticle::DESC& Desc);
	virtual CCloneable* Clone(void* pArg) override; 
	virtual void Free() override;
};

END
