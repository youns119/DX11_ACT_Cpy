#include "pch.h"
#include "Remains.h"

#include "GameInstance.h"
#include "UI_Manager.h"

#include "UI_Ghost.h"
#include "UI_LocationIcon.h"

#include "Player.h"

CRemains::CRemains(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CRemains::CRemains(const CRemains& Prototype)
    : super(Prototype)
{
}

HRESULT CRemains::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRemains::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObject()))
        return E_FAIL;

    m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

    m_isRender = false;

    return S_OK;
}

void CRemains::Priority_Update(_float fTimeDelta)
{
    super::Priority_Update(fTimeDelta);
}

void CRemains::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Get_CurrentLevelIndex() == m_iCurLevelIndex)
    {
        if (!m_pColliderCom)
        {
            /* Com_Collider */
            {
                CCollider_Sphere::DESC Desc{};
                Desc.ColliderDesc.pOwner = this;
                Desc.ColliderDesc.iCollisionGroup = (_uint)(COLLISION_GROUP::ITEM);

                Desc.fRadius = 1.f;
                Desc.vCenter = { 0.f, 1.f, 0.f };

                if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_Sphere",
                    L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
                    return;
            }
        }
        m_isRender = true;
    }
    else if(m_iCurLevelIndex != -1)
    {
        if (m_pColliderCom)
        {
            Delete_Component(L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom));
            Safe_Release(m_pColliderCom);
        }

        m_isRender = false;
    }

    if (m_pColliderCom)
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

    super::Update(fTimeDelta);
}

void CRemains::Late_Update(_float fTimeDelta)
{
    if (!m_isRender)
        return;

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif

    super::Late_Update(fTimeDelta);
}

HRESULT CRemains::Render()
{
    return S_OK;
}

void CRemains::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        m_pUIManager->Show_Interaction(L"ащ╠Б");
    }
    break;
    default:
        break;
    }
}

void CRemains::On_Collision(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        if (m_pGameInstance->Key_Down(KEY::E))
        {
            m_pUIManager->Set_MicroPlastic(m_iMicroplastic);
            m_iMicroplastic = 0;
            m_iCurLevelIndex = -1;
            Delete_Component(L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom));
            Safe_Release(m_pColliderCom);

            m_pUIManager->Hide_Interaction();
            m_isRender = false;

            CPlayer* pPlayer = static_cast<CPlayer*>(pOther);
            if (pPlayer == nullptr)
                return;

            pPlayer->Clear_Buffer();
        }
    }
    break;
    default:
        break;
    }
}

void CRemains::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
    m_pUIManager->Hide_Interaction();
}

void CRemains::Drop(_float3 vPos, _uint iCurLevelIndex)
{
    m_pTransformCom->Locate(vPos);
    m_iCurLevelIndex = iCurLevelIndex;

    m_iMicroplastic = m_pUIManager->Get_Micro_Ref();
    m_pUIManager->Set_MicroPlastic(-1 * m_iMicroplastic);
}

HRESULT CRemains::Ready_Components()
{
    return S_OK;
}

HRESULT CRemains::Ready_PartObject()
{
    CUI_Ghost::DESC GhostDesc{};
    {
        GhostDesc.UIDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
        GhostDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Ghost.dat";
        GhostDesc.UIDesc.bInWorld = true;
        GhostDesc.UIDesc.fWorldOffsetY = 1.f;
    }
    if (FAILED(Add_PartObject(L"Part_UI_Ghost", LEVEL_STATIC, L"Prototype_GameObject_UI_Ghost", &GhostDesc)))
        return E_FAIL;

    CUI_LocationIcon::DESC LocationIconDesc{};
    {
        LocationIconDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_RemainLocationIcon.dat";
        LocationIconDesc.pNPCWorld = &m_pTransformCom->Get_WorldMatrix_Ref();
    }
    if (FAILED(Add_PartObject(L"Part_UI_LocationIcon", LEVEL_STATIC, L"Prototype_GameObject_UI_LocationIcon", &LocationIconDesc)))
        return E_FAIL;

    return S_OK;
}

CRemains* CRemains::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRemains* pInstance = new CRemains(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CRemains");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CRemains::Clone(void* pArg)
{
    CRemains* pInstance = new CRemains(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CRemains");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRemains::Free()
{
    super::Free();

    Safe_Release(m_pColliderCom);
}
