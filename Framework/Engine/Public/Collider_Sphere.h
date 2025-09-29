#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CCollider_Sphere final : public CCollider
{
	using super = CCollider;

public:
	struct DESC
	{
		super::DESC ColliderDesc {};

		_float3 vCenter {};
		_float fRadius { 0.f };
	};

private:
	CCollider_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider_Sphere(const CCollider_Sphere& Prototype);
	virtual ~CCollider_Sphere() = default;

public:
	HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Update(_fmatrix WorldMatirx) override;
	virtual _bool Intersect(CCollider* pTargetCollider) override;

	virtual _bool Intersect_With(const class CCollider_AABB* pOther) override;
	virtual _bool Intersect_With(const class CCollider_OBB* pOther) override;
	virtual _bool Intersect_With(const class CCollider_Sphere* pOther) override;

	virtual _bool Ray_Cast(_fvector RayOrigin, _fvector RayDirection, _float fRayLength, _float& fDist);


	const DirectX::BoundingSphere& Get_Bounding() const { return m_Sphere.Get_Bounding(); }

	virtual _vec3 Get_BoundingCenter() const override { return m_Sphere.Get_Bounding().Center; }

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

private:
	CBounding<DirectX::BoundingSphere> m_Sphere;

public:
	static CCollider_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
