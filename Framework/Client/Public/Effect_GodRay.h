#pragma once

#include "Client_Defines.h"
#include "SpriteEffect.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Particle_Point;
END

BEGIN(Client)
class CEffect_GodRay final : public CSpriteEffect
{
	using super = CSpriteEffect;

private:
	CEffect_GodRay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_GodRay(const CEffect_GodRay& Prototype);
	virtual ~CEffect_GodRay() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Revealage() override;


	HRESULT			Ready_Components(LEVELID eLevelID, CVIBuffer_Particle_Point::DESC& Desc);

private:
	CShader*					m_pShaderCom{ nullptr };
	CVIBuffer_Particle_Point*	m_pVIBufferCom{ nullptr };

public:
	static CEffect_GodRay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
