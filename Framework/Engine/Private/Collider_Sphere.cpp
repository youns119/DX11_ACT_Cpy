#include "Collider_Sphere.h"

#include "Prototype_Manager.h"

CCollider_Sphere::CCollider_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCollider(pDevice, pContext)
    , m_Sphere(pDevice, pContext)
{
}

CCollider_Sphere::CCollider_Sphere(const CCollider_Sphere& Prototype)
    : CCollider(Prototype)
    , m_Sphere(Prototype.m_Sphere)
{
}

HRESULT CCollider_Sphere::Initialize_Prototype()
{
    super::Initialize_Prototype();

    m_eType = CCollider::TYPE::OBB;

    return S_OK;
}

HRESULT CCollider_Sphere::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    DirectX::BoundingSphere Prototype {};
    Prototype.Center = pDesc->vCenter;
    Prototype.Radius = pDesc->fRadius;

    m_Sphere.Initialize(Prototype);

    if (FAILED(super::Initialize(&pDesc->ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

void CCollider_Sphere::Update(_fmatrix WorldMatirx)
{
    m_isColl = false; // 이거 안쓰긴할걸 이제

	m_Sphere.Update(WorldMatirx);
}

_bool CCollider_Sphere::Intersect(CCollider* pTargetCollider)
{
    if (false == m_isActive)
        return false;

    return m_isColl = pTargetCollider->Intersect_With(this);
}

_bool CCollider_Sphere::Intersect_With(const CCollider_AABB* pOther)
{
    return m_isColl = m_Sphere.Get_Bounding().Intersects(pOther->Get_Bounding());
}

_bool CCollider_Sphere::Intersect_With(const CCollider_OBB* pOther)
{
    return m_isColl = m_Sphere.Get_Bounding().Intersects(pOther->Get_Bounding());
}

_bool CCollider_Sphere::Intersect_With(const CCollider_Sphere* pOther)
{
    return m_isColl = m_Sphere.Get_Bounding().Intersects(pOther->Get_Bounding());
}

_bool CCollider_Sphere::Ray_Cast(_fvector RayOrigin, _fvector RayDirection, _float fRayLength, _float& fDist)
{
    if (false == m_isActive)
        return false;

    if (m_Sphere.Get_Bounding().Intersects(RayOrigin, RayDirection, fDist))
    {
        return (fRayLength > fDist);
    }

    return false;
}

#ifdef _DEBUG
HRESULT CCollider_Sphere::Render()
{
    if (false == m_isActive)
        return S_OK;

    if (FAILED(super::Render()))
        return E_FAIL;

    //return m_Sphere.Render(m_pBatch->Ptr(), m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));
    //return m_Sphere.Render(m_pBatch, m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));
    return m_Sphere.Render(m_pBatch, m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSetW(XMLoadFloat3(&m_vDefualtColor), 1.f));
}
#endif

CCollider_Sphere* CCollider_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCollider_Sphere* pInstance = new CCollider_Sphere(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed To Create : CCollider_Sphere");
    }

    return pInstance;
}

CCloneable* CCollider_Sphere::Clone(void* pArg)
{
    CCollider_Sphere* pInstance = new CCollider_Sphere(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed To Clone : CCollider_Sphere");
    }

    return pInstance;
}

void CCollider_Sphere::Free()
{
    super::Free();

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
