#include "pch.h"
#include "Instancing_KelpGround.h"
#include "GameInstance.h"

CInstancing_KelpGround::CInstancing_KelpGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CInstancing_KelpGround::CInstancing_KelpGround(const CInstancing_KelpGround& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CInstancing_KelpGround::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CInstancing_KelpGround::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CInstancing_KelpGround::Priority_Update(_float fTimeDelta)
{
}

void CInstancing_KelpGround::Update(_float fTimeDelta)
{
    m_fAccTimeDelta += fTimeDelta;

    _float fStrength = 0.03f;
    _float fFrequency = 7.f; // ÃÊ
    _vec3 BendDir = m_vBendDir.Normalized();
    m_vBendFactor = BendDir * sinf(m_fAccTimeDelta / fFrequency * XM_2PI) * fStrength;

    if (m_fAccTimeDelta > fFrequency)
        m_fAccTimeDelta -= fFrequency;
}

void CInstancing_KelpGround::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
}

HRESULT CInstancing_KelpGround::Render()
{
    if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vBendFactor", &m_vBendFactor, sizeof(_float3))))
        return E_FAIL;

    for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
    {
        _uint iMaterialIndex = m_pModelCom->Get_MaterialIndex_In_Mesh(i);
        _uint iShaderPass = m_pModelCom->Get_ShaderPass(iMaterialIndex);

        if (iShaderPass == 1)
        {
            iShaderPass = (_uint)SHADER_PASS_MESH::BASE_COLOR;
            if (FAILED(m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor")))
                return E_FAIL;
        }
        else if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
        {
            iShaderPass = 1; // pass base color;
            if (FAILED(m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor")))
                return E_FAIL;
        }
        else if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::NORMAL))
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::NORMAL, "g_NormalTexture")))
                return E_FAIL;
        }
        else
        {
            if (iShaderPass == (_uint)SHADER_PASS_MESH::UV_WRAPPING)
                iShaderPass = (_uint)SHADER_PASS_MESH::UV_WRAPPING_EXCEPT_NORMAL;
            else
                iShaderPass = (_uint)SHADER_PASS_MESH::NON_NORMAL_MAPPING;
        }

        m_pShaderCom->Begin(iShaderPass);
        m_pVIBufferCom->Bind_InputAssembler();

        m_pVIBufferCom->Render();
    }

    return S_OK;
}

void CInstancing_KelpGround::Input_InstancingInfo(const _int& iNumInstancing, const _float3& vCenter, const _float3& vRange, const _float2& vSize)
{
    m_pVIBufferCom->Input_InstancingInfo(iNumInstancing, vCenter, vRange, vSize);
}

void CInstancing_KelpGround::Save_InstancingFile(HANDLE hFile)
{
    m_pVIBufferCom->Save_InstancingFile(hFile);
}

void CInstancing_KelpGround::Delete_PrevInstancing(_uint _iNumDeleteInstance)
{
    m_pVIBufferCom->Delete_PrevInstancing(_iNumDeleteInstance);
}

_bool CInstancing_KelpGround::Find_PickedInstance_WorldMatrix(const _float3& vPickedPos, _float4x4& OutMatrix)
{
    return m_pVIBufferCom->Find_PickedInstance_WorldMatrix(vPickedPos, OutMatrix);
}
_bool CInstancing_KelpGround::Get_PickInstanceMesh_WorldMatrix(_float4x4& OutMatrix)
{
    return m_pVIBufferCom->Get_PickInstanceMesh_WorldMatrix(OutMatrix);

}
_bool CInstancing_KelpGround::Set_PickInstanceMesh_WorldMatrix(_float4x4& InMatrix)
{
    return m_pVIBufferCom->Set_PickInstanceMesh_WorldMatrix(InMatrix);
}

HRESULT CInstancing_KelpGround::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh_Instancing"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instancing_Mesh_KelpGround"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_KelpGround"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

CInstancing_KelpGround* CInstancing_KelpGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInstancing_KelpGround* pInstance = new CInstancing_KelpGround(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CInstancing_KelpGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInstancing_KelpGround::Clone(void* pArg)
{
    CInstancing_KelpGround* pInstance = new CInstancing_KelpGround(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CInstancing_KelpGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInstancing_KelpGround::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pModelCom);
}
