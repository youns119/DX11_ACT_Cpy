#pragma once

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CPhysics abstract : public CComponent
{
	using super = CComponent;
public:
	enum class LOCK_FLAG
	{
		X = PxRigidDynamicLockFlag::eLOCK_LINEAR_X,
		Y = PxRigidDynamicLockFlag::eLOCK_LINEAR_Y,
		Z = PxRigidDynamicLockFlag::eLOCK_LINEAR_Z,
		ANGLE_X = PxRigidDynamicLockFlag::eLOCK_ANGULAR_X,
		ANGLE_Y = PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y,
		ANGLE_Z = PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z,
	};
	friend CPhysics::LOCK_FLAG operator|(const CPhysics::LOCK_FLAG& eLhs, const CPhysics::LOCK_FLAG& lRhs)
	{
		return static_cast<CPhysics::LOCK_FLAG>(static_cast<_uint>(eLhs) | static_cast<_uint>(lRhs));
	}

	struct DESC
	{
		/* material */
		_float fStaticFriction { 0.5f };
		_float fDynamicFriction { 0.5f };
		_float fRestitution { 0.6f };

		LOCK_FLAG eLockFlog { static_cast<LOCK_FLAG>(0) };
		_float fMass { 1.f };
		_bool isStatic { true };
		_uint iFilter { 1 << 0 };

		_vec3 InitialForce {0.f, 0.f, 0.f};

		_float4x4* pFinalMatrix { nullptr };
	};

protected:
	CPhysics(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysics(const CPhysics& Prototype);
	virtual ~CPhysics() = default;

public:
	virtual void Manipulate_Physics(const _mat& Matrix);
	virtual void Manipulate_Object();

	virtual void Add_Force(_vec3 vForce);
	virtual void Add_Torque(_vec3 vTorque);
	void Set_Velocity(_vec3 vVelocity);

	void Sleep();
	void Wake();

	void Add_Count();
	void Sub_Count();

	void Resist_To_Chunk(_vec3 vMin, _vec3 vMax);

	void Remove_From_Scene();
	void Add_To_Scene();

	/* Kinematic - true 는 안움직이는 놈이 됩니다. */
	void Set_Kinematic(_bool isKinematic);

#ifdef _DEBUG
	_uint Get_Count() const { return m_iCount; }
	_bool Is_Simulated() const;
#endif // _DEBUG


protected:
	/* PxRigidDynamic, PxRigidStatic, PxPrgidBody */
	PxRigidActor*	m_pActor { nullptr };
	_float4x4*		m_pFinalMatrix { nullptr };

	PxPhysics&		m_Physics;
	PxScene*		m_pScene { nullptr };

	/* for Rigid Static */
	_uint m_iCount { 0 };

	/* for Rigid Dynamic */
	_uint m_iChunkIndexX { 0 };
	_uint m_iChunkIndexZ { 0 };
	_float m_fKinematicThreshold { 0.00001f };

protected:
	HRESULT Setup_Mass(const DESC& Desc, const _vec3& vCenter);
	HRESULT Setup_InitForce(const DESC& Desc);

public:
	virtual void Free() override;
};
END
