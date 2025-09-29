#include "pch.h"
#include "MapTool_Monster.h"
#include "GameInstance.h"

CMapTool_Monster::CMapTool_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super{ pDevice, pContext }
{
}

CMapTool_Monster::CMapTool_Monster(const CMapTool_Monster& Prototype)
    : super(Prototype)
{
}

HRESULT CMapTool_Monster::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapTool_Monster::Initialize(void* pArg)
{
    CGameObject::DESC* pDesc = static_cast<CGameObject::DESC*>(pArg);
    pDesc->TransformDesc.fSpeedPerSec = 5.f;

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CMapTool_Monster::Priority_Update(_float fTimeDelta)
{
}

void CMapTool_Monster::Update(_float fTimeDelta)
{
}

void CMapTool_Monster::Late_Update(_float fTimeDelta)
{
}

HRESULT CMapTool_Monster::Render()
{
    return S_OK;
}

void CMapTool_Monster::Add_Position(_float3 vPos)
{
    _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    WorldMatrix.r[3] = XMVectorSetW(XMLoadFloat3(&vPos), 1.f);

    _float4x4 SaveMat = {};
    XMStoreFloat4x4(&SaveMat, WorldMatrix);
    m_Positions.push_back(SaveMat);
}

void CMapTool_Monster::Add_Matrix(_float4x4 WorldMat)
{
    m_Positions.push_back(WorldMat);
}

void CMapTool_Monster::Delete_Position(_uint Idx)
{
    m_Positions.erase(m_Positions.begin() + Idx);
}

void CMapTool_Monster::Set_WorldPosition(_float3 vPos)
{
    m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f)); 
}

void CMapTool_Monster::DebugRender()
{
    _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    m_pGameInstance->Add_DebugRender_Box(WorldMatrix, _vec4(1.f, 0.f, 0.f, 1.f));

    for (auto& PosMat : m_Positions)
    {
        //_mat PosMat{};
        //PosMat.r[3] = XMVectorSetW(XMLoadFloat3(&Pos), 1.f);
        //PosMat.position.y += WorldMatrix.up.Length();
        m_pGameInstance->Add_DebugRender_Box(PosMat, _vec4(0.f, 1.f, 0.f, 1.f));
    }
}

CMapTool_Monster* CMapTool_Monster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMapTool_Monster* pInstance = new CMapTool_Monster(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CMapTool_Monster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CMapTool_Monster::Clone(void* pArg)
{
    CMapTool_Monster* pInstance = new CMapTool_Monster(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CMapTool_Monster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapTool_Monster::Free()
{
    super::Free();
}
