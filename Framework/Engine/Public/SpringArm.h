#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CSpringArm
	: public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::GAMEOBJECT_DESC GameObjectDesc{};

		_float3 vOffset{};
		_float3 vTargetOffset{};
		_float fArmLength{};

		_float fLagSpeed{ 1.f };
		_float fLagRotation{ 1.f };

		_float fCollisionOffset{};
	};

protected:
	CSpringArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpringArm(const CSpringArm& Prototype);
	virtual ~CSpringArm() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_InheritRotation(_bool _bInheritRotation) { m_bInheritRotation = _bInheritRotation; }

	void Set_ActiveLag(_bool _bActiveLag) { m_bActiveLag = _bActiveLag; }
	void Set_LagSpeed(_float fLagSpeed) { m_fLagSpeed = fLagSpeed; }
	void Set_LagRotation(_float fLagRotation) { m_fLagRotation = fLagRotation; }

	void Set_PhysicsCollision(_bool _bPhysicsCollision) { m_bPhysicsCollision = _bPhysicsCollision; }

public :
	//void Reset_Matrix(_fvector vDir, );

protected:
	_float3 m_vOffset{};
	_float3 m_vTargetOffset{};
	_float m_fArmLength{};

	_float3 m_vStartOffset{};
	_float3 m_vDirOffset{};

private:
	_bool m_bActiveLag{ true };
	_float m_fLagSpeed{ 1.f };
	_float m_fLagRotation{ 1.f };

private:
	_float m_fEpsilon{ 0.01f };

protected:
	_bool m_bPhysicsCollision{};
	_float m_fCollisionOffset{};
	_float m_fCollisionDist{};

protected:
	_bool m_bInheritRotation{};

protected:
	_float m_fPrevArmLength{};
	_float4 m_vPrevPosition{};
	_float4 m_vPrevRotation{};

protected:
	void Calculate_Offset();
	void Calculate_Combined();
	void Calculate_ArmLength();
	_vector Calculate_PositionLag();
	_vector Calculate_RotationLag();

protected:
	void PhysicsCollision();

public:
	static CSpringArm* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg);
	virtual void Free() override;
};

END