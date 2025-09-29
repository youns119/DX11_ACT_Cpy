#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CCollider_OBB final : public CCollider
{
	using super = CCollider;

public:
	struct DESC
	{
		super::DESC ColliderDesc {};

		_float3 vCenter {};
		_float3 vExtents {};
		_float3 vRotation {};
	};

private:
	CCollider_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider_OBB(const CCollider_OBB& Prototype);
	virtual ~CCollider_OBB() = default;

public:
	HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Update(_fmatrix WorldMatirx) override;
	virtual _bool Intersect(CCollider* pTargetCollider) override;

	virtual _bool Intersect_With(const class CCollider_AABB* pOther) override;
	virtual _bool Intersect_With(const class CCollider_OBB* pOther) override;
	virtual _bool Intersect_With(const class CCollider_Sphere* pOther) override;

	virtual _bool Ray_Cast(_fvector RayOrigin, _fvector RayDirection, _float fRayLength, _float& fDist);

	const DirectX::BoundingOrientedBox& Get_Bounding() const { return m_OBB.Get_Bounding(); }

	virtual _vec3 Get_BoundingCenter() const override { return m_OBB.Get_Bounding().Center; }

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

private:
	CBounding<DirectX::BoundingOrientedBox> m_OBB;

public:
	static CCollider_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
