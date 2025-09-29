#include "pch.h"
#include "NonMeshCollider.h"

#include "GameInstance.h"
#include "Observer.h"

CNonMeshCollider::CNonMeshCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CNonMeshCollider::CNonMeshCollider(const CNonMeshCollider& Prototype)
    : super(Prototype)
{
}

HRESULT CNonMeshCollider::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNonMeshCollider::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CNonMeshCollider::Priority_Update(_float fTimeDelta)
{
}

void CNonMeshCollider::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CNonMeshCollider::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
    m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif
}

HRESULT CNonMeshCollider::Render()
{
    return S_OK;
}

void CNonMeshCollider::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{

}

void CNonMeshCollider::On_Collision(CGameObject* pOther, _uint iGroup)
{

}

void CNonMeshCollider::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{

}


HRESULT CNonMeshCollider::Ready_Components()
{
    /* Com_Collider */
    {
        CCollider_OBB::DESC Desc{};
        Desc.ColliderDesc.pOwner = this;
        Desc.ColliderDesc.iCollisionGroup = (_uint)(COLLISION_GROUP::WALL); // ���߿� �ٲܼ���.

        //_vec3 vScale = m_pTransformCom->Get_Scale();
        //Desc.vExtents = _vec3(vScale.x * 0.5f, vScale.y * 0.5f, vScale.z * 0.5f)._float3();
        Desc.vExtents = _vec3(1.f, 1.f, 1.f)._float3();
        Desc.vCenter = { 0.f, Desc.vExtents.y, 0.f };
        Desc.vRotation = { 0.f, 0.f, 0.f };

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_OBB",
            L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

CNonMeshCollider* CNonMeshCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNonMeshCollider* pInstance = new CNonMeshCollider(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CNonMeshCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CNonMeshCollider::Clone(void* pArg)
{
    CNonMeshCollider* pInstance = new CNonMeshCollider(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CNonMeshCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNonMeshCollider::Free()
{
    super::Free();

    Safe_Release(m_pColliderCom);
}
