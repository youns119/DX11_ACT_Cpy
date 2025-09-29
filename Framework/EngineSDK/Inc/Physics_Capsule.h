#pragma once

#include "Physics.h"

BEGIN(Engine)
class ENGINE_DLL CPhysics_Capsule final : public CPhysics
{
	using super = CPhysics;

public:
	struct DESC
	{
		super::DESC PhysicsDesc {};
		_vec3 vPosition {};
		_float fRadius { 1.f };
		_float fHeight { 1.f };
	};

private:
	CPhysics_Capsule(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysics_Capsule(const CPhysics_Capsule& Prototype);
	virtual ~CPhysics_Capsule() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;


	//virtual void Manipulate_Physics(const _mat& Matrix) override;

private:
	_float m_fRadius { 0.f };

public:
	static CPhysics_Capsule* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
