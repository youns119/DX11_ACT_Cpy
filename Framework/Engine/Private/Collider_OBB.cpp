#include "Collider_OBB.h"

#include "Prototype_Manager.h"

CCollider_OBB::CCollider_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCollider(pDevice, pContext)
    , m_OBB(pDevice, pContext)
{
}

CCollider_OBB::CCollider_OBB(const CCollider_OBB& Prototype)
    : CCollider(Prototype)
    , m_OBB(Prototype.m_OBB)
{
}

HRESULT CCollider_OBB::Initialize_Prototype()
{
    super::Initialize_Prototype();

    m_eType = CCollider::TYPE::OBB;

    return S_OK;
}

HRESULT CCollider_OBB::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    DirectX::BoundingOrientedBox Prototype {};
    Prototype.Center = pDesc->vCenter;
    Prototype.Extents = pDesc->vExtents;
    XMStoreFloat4(&Prototype.Orientation, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

    m_OBB.Initialize(Prototype);

    if (FAILED(super::Initialize(&pDesc->ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

void CCollider_OBB::Update(_fmatrix WorldMatirx)
{
    m_isColl = false; // 이거 안쓰긴할걸 이제

	m_OBB.Update(WorldMatirx);

}

_bool CCollider_OBB::Intersect(CCollider* pTargetCollider)
{
    if (false == m_isActive)
        return false;

    return m_isColl = pTargetCollider->Intersect_With(this);
}

_bool CCollider_OBB::Intersect_With(const CCollider_AABB* pOther)
{
    return m_isColl = m_OBB.Get_Bounding().Intersects(pOther->Get_Bounding());
}

_bool CCollider_OBB::Intersect_With(const CCollider_OBB* pOther)
{
	return m_isColl = m_OBB.Get_Bounding().Intersects(pOther->Get_Bounding());
}

_bool CCollider_OBB::Intersect_With(const CCollider_Sphere* pOther)
{
    return m_isColl= m_OBB.Get_Bounding().Intersects(pOther->Get_Bounding());
}

_bool CCollider_OBB::Ray_Cast(_fvector RayOrigin, _fvector RayDirection, _float fRayLength, _float& fDist)
{
    if (false == m_isActive)
        return false;

    if (m_OBB.Get_Bounding().Intersects(RayOrigin, RayDirection, fDist))
    {
        return (fRayLength > fDist);
    }

    return false;
}

#ifdef _DEBUG
HRESULT CCollider_OBB::Render()
{
    if (false == m_isActive)
        return S_OK;

    if (FAILED(super::Render()))
        return E_FAIL;

    //return m_OBB.Render(m_pBatch->Ptr(), m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));
    //HRESULT hr = m_OBB.Render(m_pBatch, m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));
    HRESULT hr = m_OBB.Render(m_pBatch, m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSetW(XMLoadFloat3(&m_vDefualtColor), 1.f));

    return hr;
}
#endif

CCollider_OBB* CCollider_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCollider_OBB* pInstance = new CCollider_OBB(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed To Create : CCollider_OBB");
    }

    return pInstance;
}

CCloneable* CCollider_OBB::Clone(void* pArg)
{
    CCollider_OBB* pInstance = new CCollider_OBB(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed To Clone : CCollider_OBB");
    }

    return pInstance;
}

void CCollider_OBB::Free()
{
    super::Free();

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
