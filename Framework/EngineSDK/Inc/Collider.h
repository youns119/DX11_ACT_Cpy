#pragma once

#include "Component.h"
#include "Bounding.h"
#include "Shared.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CCollider abstract : public CComponent
{
	using super = CComponent;

public:

	struct DESC
	{
		_uint iCollisionGroup { 0 };
		CGameObject* pOwner { nullptr };
	};

	enum class TYPE
	{
		AABB,
		OBB,
		SPHERE,

		MAX
	};

protected:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_fmatrix WorldMatrix) = 0;
	virtual _bool Intersect(CCollider* pTargetCollider) = 0;

	virtual _bool Intersect_With(const class CCollider_AABB* pOther) = 0;
	virtual _bool Intersect_With(const class CCollider_OBB* pOther) = 0;
	virtual _bool Intersect_With(const class CCollider_Sphere* pOther) = 0;

	virtual _bool Ray_Cast(_fvector RayOrigin, _fvector RayDirection, _float fRayLength, _float& fDist) = 0;

	TYPE Get_Type() const { return m_eType; }
	void Set_Active(_bool isActive) { m_isActive = isActive; }
	_bool Is_Active() const { return m_isActive; }

	_uint Get_CollisionGroup() const { return m_iCollisionGroup; }

	_uint Get_ID() const { return m_iID; }

	virtual _vec3 Get_BoundingCenter() const = 0;

	void Set_DefaultColor(_fvector vColor)
	{
		XMStoreFloat3(&m_vDefualtColor, vColor);
	}

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif // _DEBUG


protected:
	_bool	m_isColl { false };
	TYPE	m_eType { TYPE::MAX };
	_bool m_isActive { true };

	_uint m_iCollisionGroup { 0 };

	_float3 m_vDefualtColor{ 0.f, 1.f, 0.f };

#ifdef _DEBUG
protected:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch { nullptr };
	BasicEffect* m_pEffect { nullptr };
	ID3D11InputLayout* m_pInputLayout { nullptr };
#endif

private:
	static _uint s_iIDCounter;
	_uint m_iID { 0 };

public:
	virtual void Free() override;

};

END
