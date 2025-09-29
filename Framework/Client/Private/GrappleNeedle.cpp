#include "pch.h"
#include "GrappleNeedle.h"
#include "GameInstance.h"

CGrappleNeedle::CGrappleNeedle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : super{ _pDevice, _pContext }
{
}

CGrappleNeedle::CGrappleNeedle(const CGrappleNeedle& _Prototype)
    : super{ _Prototype }
{
}

HRESULT CGrappleNeedle::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CGrappleNeedle::Initialize(void* _pArg)
{
    CGameObject::DESC* pDesc = static_cast<CGameObject::DESC*>(_pArg);

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CGrappleNeedle::Priority_Update(_float _fTimeDelta)
{
}

void CGrappleNeedle::Update(_float _fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CGrappleNeedle::Late_Update(_float _fTimeDelta)
{
    if (true == m_pGameInstance->Is_InFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE::POSITION), 30.f))
    {// 절두체 안에 있을 때만 컬링 

        _float fMaxRange = m_pGameInstance->Get_FogParams().vRange.y;
        super::Compute_CamDistance();

        if (fMaxRange > m_fCamDistance + 30.f)
        {
            m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
#ifdef _DEBUG
            m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif // _DEBUG
        }
    }
}

HRESULT CGrappleNeedle::Render()
{
    if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID"))) // 즁요 ㅋㅋ
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
        return E_FAIL;

    for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
    {
        _uint iMaterialIndex = m_pModelCom->Get_MaterialIndex_In_Mesh(i);
        _uint iShaderPass = m_pModelCom->Get_ShaderPass(iMaterialIndex);


        if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE))
        {
            if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::NORMAL))
            {
                iShaderPass = (_uint)SHADER_PASS_MESH::DEFAULT;

                if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
                    return E_FAIL;
                if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::NORMAL, "g_NormalTexture")))
                    return E_FAIL;
            }
            else
            {
                iShaderPass = (_uint)SHADER_PASS_MESH::NON_NORMAL_MAPPING;

                if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
                    return E_FAIL;
            }
        }
        else
        {
            if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::NORMAL))
            {
                iShaderPass = (_uint)SHADER_PASS_MESH::BASE_COLOR;

                if (FAILED(m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor")))
                    return E_FAIL;
                if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::NORMAL, "g_NormalTexture")))
                    return E_FAIL;
            }
            else
            {
                iShaderPass = (_uint)SHADER_PASS_MESH::BASE_COLOR_NON_NORMAL;

                if (FAILED(m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor")))
                    return E_FAIL;
            }
        }

        m_pShaderCom->Begin(iShaderPass);
        m_pModelCom->Render(i);
    }

    return S_OK;


}

_vec3 CGrappleNeedle::Get_GrapplePos()
{
    _vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);

    return vPos;
}

_uint CGrappleNeedle::Get_Type()
{
    _uint iType = m_pColliderCom->Get_CollisionGroup();

    return iType;
}

HRESULT CGrappleNeedle::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_GrappleNeedle"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Collider */
    {
        CCollider_Sphere::DESC Desc{};
        Desc.ColliderDesc.pOwner = this;
        Desc.ColliderDesc.iCollisionGroup = (_uint)(COLLISION_GROUP::GRAPPLENEEDLE);

        Desc.fRadius = 1.f;
        Desc.vCenter = { 0.f, 0.f, 0.f };

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_Sphere",
            L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

CGrappleNeedle* CGrappleNeedle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CGrappleNeedle* pInstance = new CGrappleNeedle(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CGrappleNeedle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CGrappleNeedle::Clone(void* _pArg)
{
    CGrappleNeedle* pInstance = new CGrappleNeedle(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Clone : CGrappleNeedle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGrappleNeedle::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}
