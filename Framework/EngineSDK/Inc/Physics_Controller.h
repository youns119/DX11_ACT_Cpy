#pragma once

#include "Component.h"

BEGIN(Engine)

class CPhysics_ControllerHit;

struct COLLISION_DESC
{
	COLLISION_DESC() = default;
	COLLISION_DESC(PxControllerCollisionFlags pxFlag) : iFlag(pxFlag) {}

	_bool Is_CollisionDown() const { return iFlag & PxControllerCollisionFlag::eCOLLISION_DOWN; }
	_bool Is_CollisionUp() const { return iFlag & PxControllerCollisionFlag::eCOLLISION_UP; }
	_bool Is_CollisionSide() const { return iFlag & PxControllerCollisionFlag::eCOLLISION_SIDES; }

private:
	_ushort iFlag { 0 };
};

class ENGINE_DLL CPhysics_Controller final : public CComponent
{
	using super = CComponent;

public:
	struct USER_DATA
	{
		USER_DATA() = default;
		USER_DATA(const USER_DATA& Other) = default;
		USER_DATA(_bool _isGhost) : isGhost { _isGhost } {}

		_bool isGhost { false };
	};

	struct DESC
	{
		_float fRadius { 1.f };
		_float fHeight { 1.f };
		PxCapsuleClimbingMode::Enum eClimbingMode { PxCapsuleClimbingMode::eEASY /*eCONSTRAINED*/};

		_vec3 vPosition;
		_float fSlopeLimit { 0.7f };
		_float fInvisibleWallHeight { 0.f };
		_float fStepOffset { 0.1f };
		_float fDensity { 1000.f };
		_float fScaleCoeff { 1.2f };
		PxControllerNonWalkableMode::Enum	eNonWalkableMode { PxControllerNonWalkableMode::ePREVENT_CLIMBING /*ePREVENT_CLIMBING_AND_FORCE_SLIDING*/};

		_float fStaticFriction { 0.6f };
		_float fDynamicFriction { 0.5f };
		_float fRestitution { 0.5f };

		USER_DATA UserData {};

		//_vec3 vUpDirection { 0.f, 1.f, 0.f };
		//_float fMaxJumpHeight { 0.f };
		//_float fContactOffset { 0.1f };
		//_float fVolumeGrowth { 1.5f };
		//_bool bRegisterDeletionListener { true };
		//PxClientID ClientID { PX_DEFAULT_CLIENT };
		//PxUserControllerHitReport* pReportCallback { nullptr };
		//PxControllerBehaviorCallback* pBehaviorCallback { nullptr };

		_float4x4* pFinalMatrix { nullptr };
		_uint iFilter { 1 << 0 };
	};

private:
	CPhysics_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysics_Controller(const CPhysics_Controller& Prototype);
	virtual ~CPhysics_Controller() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

	COLLISION_DESC Move(_vec3 vDisp, _float fMinDist, _float fDeltaTime);
	void Manipulate_Object();
	void Set_Position(_vec3 vPosistion);
	void Die();

	const USER_DATA& Get_UserData() const { return m_UserData; }
	void Set_UserData(const USER_DATA& UserData) { m_UserData = UserData; }

private:
	PxPhysics&		m_Physics;
	PxScene*		m_pScene { nullptr };

	_float			m_fRadius { 0.f };
	_float			m_fHeight { 0.f };
	_float4x4*		m_pFinalMatrix { nullptr };

	PxController*			m_pController { nullptr };
	USER_DATA				m_UserData {};
	PxControllerFilters		m_Filters {};

	_uint m_iChunkIndexX { 0 };
	_uint m_iChunkIndexZ { 0 };

public:
	static CPhysics_Controller* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
