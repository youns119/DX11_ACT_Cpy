#include "pch.h"
#include "FakeMoonBrick.h"
#include "GameInstance.h"

CFakeMoonBrick::CFakeMoonBrick(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : super{ _pDevice, _pContext }
{
}

CFakeMoonBrick::CFakeMoonBrick(const CFakeMoonBrick& _Prototype)
    : super{ _Prototype }
{
}

HRESULT CFakeMoonBrick::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CFakeMoonBrick::Initialize(void* _pArg)
{
    CGameObject::DESC* pDesc = static_cast<CGameObject::DESC*>(_pArg);

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CFakeMoonBrick::Priority_Update(_float _fTimeDelta)
{
}

void CFakeMoonBrick::Update(_float _fTimeDelta)
{
    //m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CFakeMoonBrick::Late_Update(_float _fTimeDelta)
{
    if (true == m_pGameInstance->Is_InFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE::POSITION), 40.f))
    {// 절두체 안에 있을 때만 컬링 
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

        //#ifdef _DEBUG	
        //        m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
        //#endif
    }
}

HRESULT CFakeMoonBrick::Render()
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
            iShaderPass = (_uint)SHADER_PASS_MESH::DEFAULT;

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
                return E_FAIL;
        }
        else
        {
            iShaderPass = (_uint)SHADER_PASS_MESH::BASE_COLOR;

            if (FAILED(m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor")))
                return E_FAIL;
        }

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::NORMAL, "g_NormalTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::METALNESS, "g_MetalicTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::SHININESS, "g_SmoothnessTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::EMISSIVE, "g_EmissiveMaskTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_EmissionColor(m_pShaderCom, i, "g_vEmissionColor")))
            return E_FAIL;

        m_pShaderCom->Begin(iShaderPass);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CFakeMoonBrick::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_CRABTOMB, TEXT("Prototype_Component_Model_FakeMoonBrick"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    ///* Com_Collider */
    //{
    //    CCollider_AABB::DESC Desc{};
    //    Desc.ColliderDesc.pOwner = this;
    //    Desc.ColliderDesc.iCollisionGroup = (_uint)(COLLISION_GROUP::MAP_INTERACTION);

    //    Desc.vExtents = { 3.0f, 3.0f,  3.0f };
    //    Desc.vCenter = { 0.f, Desc.vExtents.y, 0.f };

    //    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_AABB",
    //        L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
    //        return E_FAIL;
    //}

    {
        CPhysics_Mesh::DESC Desc{};
        Desc.PhysicsDesc.fStaticFriction = 0.8f;
        Desc.PhysicsDesc.fDynamicFriction = 0.7f;
        Desc.PhysicsDesc.fRestitution = 0.2f;
        Desc.PhysicsDesc.isStatic = true;
        Desc.PhysicsDesc.iFilter = (_uint)PHYSICS_FILTER::DEFAULT;
        Desc.PhysicsDesc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());

        Desc.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
        Desc.isInChunk = true;

        if (FAILED(Add_Component(LEVEL_CRABTOMB, L"Prototype_Component_Physics_Mesh_FakeMoonBrick",
            L"Com_Physics", reinterpret_cast<CComponent**>(&m_pPhysicsCom), &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

CFakeMoonBrick* CFakeMoonBrick::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CFakeMoonBrick* pInstance = new CFakeMoonBrick(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CFakeMoonBrick");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CFakeMoonBrick::Clone(void* _pArg)
{
    CFakeMoonBrick* pInstance = new CFakeMoonBrick(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Clone : CFakeMoonBrick");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFakeMoonBrick::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    //Safe_Release(m_pColliderCom);
    Safe_Release(m_pPhysicsCom);
}
