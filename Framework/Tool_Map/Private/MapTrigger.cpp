#include "pch.h"
#include "MapTrigger.h"

#include "GameInstance.h"

CMapTrigger::CMapTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CMapTrigger::CMapTrigger(const CMapTrigger& Prototype)
    : super(Prototype)
{
}

HRESULT CMapTrigger::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapTrigger::Initialize(void* pArg)
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

void CMapTrigger::Priority_Update(_float fTimeDelta)
{
}

void CMapTrigger::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CMapTrigger::Late_Update(_float fTimeDelta)
{
    //_mat WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    //m_pGameInstance->Add_DebugRender_Box(WorldMatrix, _vec4(1.f, 0.f, 1.f, 1.f));
    m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
}

HRESULT CMapTrigger::Render()
{
    return S_OK;
}

HRESULT CMapTrigger::Ready_Components()
{
    /* Com_Collider */
    {
        CCollider_OBB::DESC Desc{};
        Desc.ColliderDesc.pOwner = this;
        Desc.ColliderDesc.iCollisionGroup = (_uint)(COLLISION_GROUP::TRIGGER);

        //_vec3 vScale = m_pTransformCom->Get_Scale();
        //Desc.vExtents = _vec3(vScale * 0.5f)._float3();
        Desc.vExtents = _vec3(_vec3(1.f, 1.f, 1.f) * 0.5f)._float3();
        //Desc.vCenter = { 0.f, Desc.vExtents.y, 0.f };
        Desc.vCenter = _float3{};
        Desc.vRotation = { 0.f, 0.f, 0.f };

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_OBB",
            L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

CMapTrigger* CMapTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMapTrigger* pInstance = new CMapTrigger(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CMapTrigger");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CMapTrigger::Clone(void* pArg)
{
    CMapTrigger* pInstance = new CMapTrigger(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CMapTrigger");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapTrigger::Free()
{
    super::Free();

    Safe_Release(m_pColliderCom);
}
