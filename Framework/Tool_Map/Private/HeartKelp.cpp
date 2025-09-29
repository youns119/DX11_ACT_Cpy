#include "pch.h"
#include "HeartKelp.h"
#include "GameInstance.h"

CHeartKelp::CHeartKelp(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : super{ _pDevice, _pContext }
{
}

CHeartKelp::CHeartKelp(const CHeartKelp& _Prototype)
    : super{ _Prototype }
{
}

HRESULT CHeartKelp::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CHeartKelp::Initialize(void* _pArg)
{
    CGameObject::DESC* pDesc = static_cast<CGameObject::DESC*>(_pArg);

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;

}

void CHeartKelp::Priority_Update(_float _fTimeDelta)
{
}

void CHeartKelp::Update(_float _fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CHeartKelp::Late_Update(_float _fTimeDelta)
{
    if (true == m_pGameInstance->Is_InFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE::POSITION), 1.5f))
    {// 절두체 안에 있을 때만 컬링 
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG	
        m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif
    }

}

HRESULT CHeartKelp::Render()
{
    if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID"))) // 즁요 ㅋㅋ
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
        return E_FAIL;

    //_uint			iNumMeshes = (_uint)m_pModelCom->Get_NumMeshes();

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
        m_pModelCom->Render(i);
    }



    return S_OK;


}

HRESULT CHeartKelp::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_HeartKelp"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Collider */
    {
        CCollider_AABB::DESC Desc{};
        Desc.ColliderDesc.pOwner = this;
        Desc.ColliderDesc.iCollisionGroup = (_uint)(COLLISION_GROUP::MAP_INTERACTION);

        Desc.vExtents = { 3.0f, 3.0f,  3.0f };
        Desc.vCenter = { 0.f, Desc.vExtents.y, 0.f };

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_AABB",
            L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

CHeartKelp* CHeartKelp::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CHeartKelp* pInstance = new CHeartKelp(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CHeartKelp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CHeartKelp::Clone(void* _pArg)
{
    CHeartKelp* pInstance = new CHeartKelp(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Clone : CHeartKelp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHeartKelp::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);   
}
