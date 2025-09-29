#pragma once

#include "Component.h"

BEGIN(Engine)

class CShader;
class CNavigation;

class ENGINE_DLL CTransform final : public CComponent
{
public:
	struct TRANSFORM_DESC
	{
		_float		fSpeedPerSec{ 0.f };
		_float		fRotationPerSec{ 0.f };
		_float4x4	WorldMatrix 
		{ 
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f,
		};
	};

public:
	enum class STATE
	{
		RIGHT,
		UP,
		LOOK,
		POSITION,
		MAX
	};


private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	//CTransform(const CTransform& Prototype);
	virtual ~CTransform() = default;


public:
	_vector Get_State(STATE eState) const { return XMLoadFloat4x4(&m_WorldMatrix).r[_uint(eState)]; }
	void Set_State(STATE eState, _fvector vState)
	{
		XMStoreFloat3(reinterpret_cast<_float3*>(&m_WorldMatrix.m[(_uint)eState]), vState);
	}
	_matrix Get_WorldMatrix_Inversed() const { return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)); }
	_matrix Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }
	const _float4x4& Get_WorldMatrix_Ref() const { return m_WorldMatrix; }
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	void Go_Straight(_float fTimeDelta, CNavigation* pNavigation = nullptr, _uint iCellOptionMask = 1 << 0);
	void Go_Backward(_float fTimeDelta, CNavigation* pNavigation = nullptr, _uint iCellOptionMask = 1 << 0);
	void Go_Left(_float fTimeDelta, CNavigation* pNavigation = nullptr, _uint iCellOptionMask = 1 << 0);
	void Go_Right(_float fTimeDelta, CNavigation* pNavigation = nullptr, _uint iCellOptionMask = 1 << 0);
	void Go_Up(_float fTimeDelta, CNavigation* pNavigation = nullptr, _uint iCellOptionMask = 1 << 0);
	void Go_Down(_float fTimeDelta, CNavigation* pNavigation = nullptr, _uint iCellOptionMask = 1 << 0);

	void Go_Toward(_fvector vDirection, _float fTimeDelta, CNavigation* pNavigation = nullptr, _uint iCellOptionMask = 1 << 0);
	void Move_By_Path(_float fTimeDelta, CNavigation* pNavigation, _fvector vGoalPosition, _float fDelay = 0.f, _uint iOptionMask = 0xffffffff);

	void Locate(const _float3& vLocation);
	void Locate(_float fX, _float fY, _float fZ) { Locate({ fX, fY, fZ }); }

	void LookAt(_fvector vAt);
	void UpAt(_fvector vAt);
	//void SetForwardUp(_fvector vLook, _fvector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f));

	void Turn(const _float3& vAxis, _float fTimeDelta); /* 실시간 회전 */
	void Rotate(const _float3& vAxis, _float fRadians); /* 고정 회전 */
	void Turn(_fvector vAxis, _float fTimeDelta); /* 실시간 회전 */
	void Rotate(_fvector vAxis, _float fRadians); /* 고정 회전 */
	void Rotate_By_State(STATE eState, _float fRadians);
	void Turn_Pivot(_fvector vPivot, _gvector vAxis, _float fTimeDelta);

	void Rotate(_float fX, _float fY, _float fZ); /* 고정 회전, Quaternion 사용 */
	void Rotate(_fvector vRotation);

	void Set_Scale(const _float3& vScale);
	void Set_Scale(_float fX, _float fY, _float fZ) { Set_Scale({ fX, fY, fZ }); }
	//_float3 Compute_Scaled();
	_float3 Get_Scale();
	_float3 Get_Rotation();

	void Set_Speed(_float fSpeed) { m_fSpeedPerSec = fabs(fSpeed); }
	void Set_RotationSpeed(_float fRotationSpeed) { m_fRotationPerSec = fRotationSpeed; }
	_float Get_Speed() const { return m_fSpeedPerSec; }
	_float Get_RotationSpeed() const { return m_fRotationPerSec; }
	void Multiply_Speed(_float fTimes) { m_fSpeedPerSec *= fTimes; }

	void Jump(_float JumpForce);
	_bool FreeFall(_float fTimeDelta, CNavigation* pNavigation = nullptr, _uint iOptionMask = 0xffffffff);

public:
	//HRESULT Bind_ShaderResource(CShader* pShader, const _char* pConstantName);
	HRESULT Bind_WorldMatrix_To_Shader(CShader* pShader, const _char* pConstantName);

public:
	static constexpr _float s_fGravityAcc { -13.f };

private:
	_float4x4		m_WorldMatrix{};

	_float		m_fSpeedPerSec{ 0.f };
	_float		m_fRotationPerSec{ 0.f };

	_float m_fVerticalVelocity { 0.f };
	//_float3 m_vFreeFallVelocity {};

	vector<pair<_float3, _float3>> m_PathEdge;
	_float m_fDelayTimeAcc { 0.f };

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
