#pragma once

#include "Physics.h"

BEGIN(Engine)
class ENGINE_DLL CPhysics_Sphere final : public CPhysics
{
	using super = CPhysics;

public: 
	struct DESC
	{
		super::DESC PhysicsDesc {};
		_vec3 vPosition {};
		_vec3 vCenter {};
		_float fRadius { 1.f };
	};

private:
	CPhysics_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysics_Sphere(const CPhysics_Sphere& Prototype);
	virtual ~CPhysics_Sphere() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

private:
#ifdef _DEBUG
	_float m_fRadius { 0.f };
	_vec3 m_vCenter {};
#endif

public:
	static CPhysics_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
