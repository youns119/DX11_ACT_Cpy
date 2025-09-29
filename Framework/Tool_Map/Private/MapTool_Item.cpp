#include "pch.h"
#include "MapTool_Item.h"
#include "GameInstance.h"

CMapTool_Item::CMapTool_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super{ pDevice, pContext }
{
}

CMapTool_Item::CMapTool_Item(const CMapTool_Item& Prototype)
    : super( Prototype ) 
{
}

HRESULT CMapTool_Item::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapTool_Item::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    m_iItemType = (_int)pDesc->eType;

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CMapTool_Item::Priority_Update(_float fTimeDelta)
{
}

void CMapTool_Item::Update(_float fTimeDelta)
{
}

void CMapTool_Item::Late_Update(_float fTimeDelta)
{
    _mat WorldMatrix = m_pTransformCom->Get_WorldMatrix();

    if (m_iItemType == 0)
        m_pGameInstance->Add_DebugRender_Box(WorldMatrix, _vec4(1.f, 1.f, 1.f, 1.f));
    else if (m_iItemType == 1)
        m_pGameInstance->Add_DebugRender_Box(WorldMatrix, _vec4(1.f, 1.f, 0.f, 1.f));
    else if (m_iItemType == 2)
        m_pGameInstance->Add_DebugRender_Box(WorldMatrix, _vec4(0.f, 0.f, 1.f, 1.f));
    else 
        m_pGameInstance->Add_DebugRender_Box(WorldMatrix, _vec4(0.f, 0.f, 0.f, 1.f));

}

HRESULT CMapTool_Item::Render()
{
    return S_OK;
}

CMapTool_Item* CMapTool_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMapTool_Item* pInstance = new CMapTool_Item(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CMapTool_Item");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CMapTool_Item::Clone(void* pArg)
{
    CMapTool_Item* pInstance = new CMapTool_Item(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CMapTool_Item");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapTool_Item::Free()
{
    super::Free();

}
