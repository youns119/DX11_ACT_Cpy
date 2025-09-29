#pragma once

#include "Engine_Defines.h"

#include "Component.h"

BEGIN(Engine)

class CTransform;

class ENGINE_DLL CMovement
	: public CComponent
{
	using super = CComponent;

public:
	struct DESC
	{
		_float fMoveAccel{};
		_float fMoveDecel{};
		_float fMaxSpeed{};
		_float fMinSpeed{};
		_float fForceFriction{};
		_float fRotationScale{};

		_float fInAirControlRatio{};
		_float fMaxFallingSpeed{};
		_float fGravityScale{};
	};

private:
	CMovement(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMovement(const CMovement& Prototype);
	virtual ~CMovement() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

public:
	_float3 Get_HorizontalVelocity() const { return m_vHorizontalVelocity; }
	_float3 Get_VerticalVelocity() const { return m_vVerticalVelocity; }
	_float3 Get_TotalHorizontalVelocity() const { return m_vTotalHorizontalVelocity; }
	_float3 Get_TotalVerticalVelocity() const { return m_vTotalVerticalVelocity; }
	_float3 Get_ForceVelocity() const { return m_vForceVelocity; }
	_float Get_HorizontalSpeed() const { return m_fHorizontalSpeed; }
	_float Get_VerticalSpeed() const { return m_fVerticalSpeed; }

	_float Get_Accel() const { return m_fMoveAccel; }
	_float Get_Decel() const { return m_fMoveDecel; }
	_float Get_MinSpeed() const { return m_fMinSpeed; }
	_float Get_MaxSpeed() const { return m_fMaxSpeed; }
	_float Get_ForceFriction() const { return m_fForceFriction; }
	_float Get_RotationScale() const { return m_fRotationScale; }

	_float Get_InAirControlRatio() const { return m_fInAirControlRatio; }
	_float Get_MaxFallingSpeed() const { return m_fMaxFallingSpeed; }
	_float Get_GravityScale() const { return m_fGravityScale; }

	_float3 Get_InputDir() const { return m_vInputDir; }

	_bool Get_InAir() const { return m_bInAir; }
	_bool Get_RotateToDirection() const { return m_bRotateToDirection; }

	void Set_HorizontalVelocity(_fvector vHorizontalVelocity)
	{
		XMStoreFloat3(&m_vHorizontalVelocity, vHorizontalVelocity);
	}
	void Set_VerticalVelocity(_fvector vVerticalVelocity)
	{
		XMStoreFloat3(&m_vVerticalVelocity, vVerticalVelocity);
	}
	void Set_ForceVelocity(_fvector vForceVelocity)
	{
		XMStoreFloat3(&m_vForceVelocity, vForceVelocity);
	}

	void Set_Accel(_float fAccel) { m_fMoveAccel = fAccel; }
	void Set_Decel(_float fDecel) { m_fMoveDecel = fDecel; }
	void Set_MinSpeed(_float fMinSpeed) { m_fMinSpeed = fMinSpeed; }
	void Set_MaxSpeed(_float fMaxSpeed) { m_fMaxSpeed = fMaxSpeed; }
	void Set_ForceFriction(_float fForceFriction) { m_fForceFriction = fForceFriction; }
	void Set_RotationScale(_float fRotationScale) { m_fRotationScale = fRotationScale; }

	void Set_InAirControlRatio(_float fInAirControlRatio) { m_fInAirControlRatio = fInAirControlRatio; }
	void Set_MaxFallingSpeed(_float fMaxFallingSpeed) { m_fMaxFallingSpeed = fMaxFallingSpeed; }
	void Set_GravityScale(_float fGravtyScale) { m_fGravityScale = fGravtyScale; }

	void Set_InAir(_bool bInAir) { m_bInAir = bInAir; }
	void Set_RotateToDirection(_bool bRotateToDirection) { m_bRotateToDirection = bRotateToDirection; }
	void Set_RotateToDirection_3D(_bool bRotateToDirection_3D) { m_bRotateToDirection_3D = bRotateToDirection_3D; }

	void Set_UpdateComponent(CTransform* pTransform);

public:
	virtual void Add_Velocity(_fvector vDir, _float fTimeDelta);
	virtual void Add_HorizontalVelocity(_fvector vDir, _float fTimeDelta);
	virtual void Add_VerticalVelocity(_fvector vDir, _float fTimeDelta);
	virtual void Add_Force(_fvector vDir, _float fForce);
	void Add_InputDir(_fvector vInputDir)
	{
		XMStoreFloat3(&m_vInputDir, XMLoadFloat3(&m_vInputDir) + vInputDir);
	}

public :
	_bool Has_Input()
	{
		return _vec3(m_vInputDir).Length() != 0.f;
	}

public:
	virtual void TurnToDirection(_fvector _vDirection, _float fTimeDelta);

public:
	void FlyMode(_bool bFlyMode) { m_bFlyMode = bFlyMode; }
	void RollingMode
	(
		_bool bRollingMode,
		_float fRollSpeed = 0.f,
		_float fPivot = 0.f
	);
	void Activate(_bool bActive) { m_bActive = bActive; }
	void Reset_Velocity();
	void Reset_Horizontal();
	void Reset_Vertical();

protected:
	CTransform* m_pUpdateComponent{};

protected:
	_float3 m_vHorizontalVelocity{};
	_float3 m_vVerticalVelocity{};
	_float3 m_vTotalHorizontalVelocity{};
	_float3 m_vTotalVerticalVelocity{};
	_float3 m_vForceVelocity{};

	_bool m_bActive{ true };
	_float m_fMoveAccel{};
	_float m_fMoveDecel{};
	_float m_fMaxSpeed{};
	_float m_fMinSpeed{};
	_float m_fForceFriction{};
	_float m_fRotationScale{ 1.f };

	_float m_fInAirControlRatio{};
	_float m_fMaxFallingSpeed{};
	_float m_fGravityScale{ 1.f };

protected:
	_float3 m_vInputDir{};

protected:
	_bool m_bInAir{};
	_bool m_bRotateToDirection{};
	_bool m_bRotateToDirection_3D{};

protected:
	_float m_fHorizontalSpeed{};
	_float m_fVerticalSpeed{};

protected:
	_bool m_bFlyMode{};

protected:
	virtual void Decrease_HorizontalVelocity(_float fTimeDelta);
	virtual void Decrease_Velocity(_float fTimeDelta);
	virtual void Decrease_Force(_float fTimeDelta);
	virtual void Apply_Gravity(_float fTimeDelta);
	virtual void RotateToDirection(_float fTimeDelta);
	virtual void RotateToDirection_3D(_float fTimeDelta);
	virtual void Calculate_Speed();

public:
	static CMovement* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg);
	virtual void Free() override;
};

END