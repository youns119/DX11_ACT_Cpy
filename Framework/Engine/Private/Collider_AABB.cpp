#include "Collider_AABB.h"

#include "Prototype_Manager.h"

CCollider_AABB::CCollider_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCollider(pDevice, pContext)
	, m_AABB(pDevice, pContext)
{
}

CCollider_AABB::CCollider_AABB(const CCollider_AABB& Prototype)
	: CCollider(Prototype)
	, m_AABB(Prototype.m_AABB)
{
}

HRESULT CCollider_AABB::Initialize_Prototype()
{
	super::Initialize_Prototype();

	m_eType = CCollider::TYPE::AABB;

	return S_OK;
}

HRESULT CCollider_AABB::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;

	BoundingBox Prototype {};
	Prototype.Center = pDesc->vCenter;
	Prototype.Extents = pDesc->vExtents;

	m_AABB.Initialize(Prototype);

	if (FAILED(super::Initialize(&pDesc->ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

void CCollider_AABB::Update(_fmatrix WorldMatirx)
{
	m_isColl = false; // 이거 안쓰긴할걸 이제

	m_AABB.Update_IgnoreScale(WorldMatirx);
}

_bool CCollider_AABB::Intersect(CCollider* pTargetCollider)
{
	if (false == m_isActive)
		return false;

	_bool isColl { false };

	//switch (pTargetCollider->Get_Type())
	//{
	//case TYPE::AABB:
	//	break;

	//case TYPE::OBB:
	//	//m_AABB.Intersect(pTargetCollider->Bound)
	//	break;
	//}
	m_isColl = pTargetCollider->Intersect_With(this);

	return m_isColl;
}

_bool CCollider_AABB::Intersect_With(const CCollider_AABB* pOther)
{
	return m_isColl = m_AABB.Get_Bounding().Intersects(pOther->Get_Bounding());
}

_bool CCollider_AABB::Intersect_With(const CCollider_OBB* pOther)
{
	return m_isColl = m_AABB.Get_Bounding().Intersects(pOther->Get_Bounding());
}

_bool CCollider_AABB::Intersect_With(const CCollider_Sphere* pOther)
{
	return m_isColl = m_AABB.Get_Bounding().Intersects(pOther->Get_Bounding());
}

_bool CCollider_AABB::Ray_Cast(_fvector RayOrigin, _fvector RayDirection, _float fRayLength, _float& fDist)
{
	if (false == m_isActive)
		return false;

	if (m_AABB.Get_Bounding().Intersects(RayOrigin, RayDirection, fDist))
	{
		return (fRayLength > fDist);
	}

	return false;
}

#ifdef _DEBUG
HRESULT CCollider_AABB::Render()
{
	if (false == m_isActive)
		return S_OK;

	if (FAILED(super::Render()))
		return E_FAIL;

	//return m_AABB.Render(m_pBatch->Ptr(), m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));
	//return m_AABB.Render(m_pBatch, m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));
	return m_AABB.Render(m_pBatch, m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSetW(XMLoadFloat3(&m_vDefualtColor), 1.f));
}
#endif

CCollider_AABB* CCollider_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCollider_AABB* pInstance = new CCollider_AABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed To Create : CCollider_AABB");
	}

	return pInstance;
}

CCloneable* CCollider_AABB::Clone(void* pArg)
{
	CCollider_AABB* pInstance = new CCollider_AABB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed To Clone : CCollider_AABB");
	}

	return pInstance;
}

void CCollider_AABB::Free()
{
	super::Free();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
