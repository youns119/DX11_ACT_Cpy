#include "pch.h"
#include "PinBall_Spliter_L.h"
//#include "Player.h"
#include "GameInstance.h"

#include "Player.h"
#include "Movement.h"

CPinBall_Spliter_L::CPinBall_Spliter_L(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : super{ _pDevice, _pContext }
{
}

CPinBall_Spliter_L::CPinBall_Spliter_L(const CPinBall_Spliter_L& _Prototype)
    : super{ _Prototype }
{
}

HRESULT CPinBall_Spliter_L::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPinBall_Spliter_L::Initialize(void* _pArg)
{
    CGameObject::DESC* pDesc = static_cast<CGameObject::DESC*>(_pArg);

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    {/* PointLight */

        _vector vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
        vPos = XMVectorSetY(vPos, (XMVectorGetY(vPos) + 2.f));


        LIGHT_DESC LightDesc = {};
        LightDesc.eType = LIGHT_TYPE::POINT;
        LightDesc.fRange = 15.f;
        XMStoreFloat4(&LightDesc.vPosition, vPos);
        LightDesc.vDiffuse = _float4(10.f, 10.f, 0.f, 1.f);
        LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
        LightDesc.vAttenuation = _float3(1.f, 0.5f, 0.5f);

        _wstring strLightTag = L"Light_Spliter" + to_wstring(m_iID);
        m_pGameInstance->Add_Light(strLightTag, LightDesc);
        m_pGameInstance->Turn_Off_Light(strLightTag);
    }

    return S_OK;
}

void CPinBall_Spliter_L::Priority_Update(_float fTimeDelta)
{
}

void CPinBall_Spliter_L::Update(_float fTimeDelta)
{
    if (m_bCollision)
    {
        m_fAccTimeDelta += fTimeDelta;
        if (m_fAccTimeDelta >= 0.2f)
        {
            m_bCollision = false;
            m_fAccTimeDelta = 0.f;
            _wstring strLightTag = L"Light_Spliter" + to_wstring(m_iID);
            m_pGameInstance->Turn_Off_Light(strLightTag);
        }
    }

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CPinBall_Spliter_L::Late_Update(_float _fTimeDelta)
{
    if (true == m_pGameInstance->Is_InFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE::POSITION), 10.f))
    {// 절두체 안에 있을 때만 컬링 
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

#ifdef _DEBUG	
        m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif
    }


}

HRESULT CPinBall_Spliter_L::Render()
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

        m_pShaderCom->Begin((_uint)SHADER_PASS_MESH::NON_CULLING);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CPinBall_Spliter_L::On_CollisionEnter(CGameObject* pGameObject, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        CPlayer* pPlayer = static_cast<CPlayer*>(pGameObject);
        if (pPlayer == nullptr || pPlayer->Get_PlayerStat()->bGrappling)
            return;

        CTransform* pTargetTransform = static_cast<CTransform*>(pPlayer->Find_Component(g_strTransformTag));
        _vec3 vTargetPos = pTargetTransform->Get_State(CTransform::STATE::POSITION);
        _vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);

        _vec3 vDir = vTargetPos - vPos;
        vDir.y = 0.f;

        pTargetTransform->LookAt((vTargetPos + vDir.Normalized())._vector());

        CMovement* pMovement = static_cast<CMovement*>(pPlayer->Find_Component(L"Com_Movement"));
        pMovement->Reset_Velocity();
        pMovement->Add_Force(vDir._vector(), 20.f);
    }
    break;
    default:
        break;
    }

    m_pGameInstance->Play_Sound(L"Pinbarge_Slingshot.wav", (_uint)SOUND_CHANNEL::MAPOBJECT, 1.f);
}

void CPinBall_Spliter_L::On_Collision(CGameObject* pGameObject, _uint iGroup)
{
    m_bCollision = true;
    m_fAccTimeDelta = 0.f;
    _wstring strLightTag = L"Light_Spliter" + to_wstring(m_iID);
    m_pGameInstance->Turn_On_Light(strLightTag);
}

void CPinBall_Spliter_L::On_CollisionExit(CGameObject* pGameObject, _uint iGroup)
{
}

HRESULT CPinBall_Spliter_L::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_PINBALL, TEXT("Prototype_Component_Model_SpliterL"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Collider */
    {
        CCollider_OBB::DESC Desc{};
        Desc.ColliderDesc.pOwner = this;
        Desc.vExtents = {11.f, 3.5f, 2.3f };
        Desc.vCenter = { 1.f, -1.f, -1.5f };
        Desc.vRotation = { 0.f, 21.581f, 0.f };

        Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::PINBALL; // 나중에 바꾸거나 할고.

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_OBB",
            L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
            return E_FAIL;
    }
    /*{
        CPhysics_Mesh::DESC Desc{};
        Desc.PhysicsDesc.fStaticFriction = 0.8f;
        Desc.PhysicsDesc.fDynamicFriction = 0.7f;
        Desc.PhysicsDesc.fRestitution = 0.2f;
        Desc.PhysicsDesc.isStatic = true;
        Desc.PhysicsDesc.iFilter = (_uint)PHYSICS_FILTER::DEFAULT;
        Desc.PhysicsDesc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());

        Desc.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
        Desc.isInChunk = true;

        if (FAILED(Add_Component(LEVEL_PINBALL, L"Prototype_Component_Physics_Mesh_SpliterL",
            L"Com_Physics", reinterpret_cast<CComponent**>(&m_pPhysicsCom), &Desc)))
        {
#ifdef _DEBUG
            OutputDebugString(_wstring(L"Prototype Not Ready : Prototype_Component_Physics_Mesh_SpliterL\n").c_str());
#endif
            return S_OK;
        }
    }*/
    return S_OK;

}

_vector CPinBall_Spliter_L::Get_OffsetPosition()
{
    return m_pTransformCom->Get_State(CTransform::STATE::POSITION);
}

CPinBall_Spliter_L* CPinBall_Spliter_L::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPinBall_Spliter_L* pInstance = new CPinBall_Spliter_L(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CPinBall_Spliter_L");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CPinBall_Spliter_L::Clone(void* pArg)
{
    CPinBall_Spliter_L* pInstance = new CPinBall_Spliter_L(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CPinBall_Spliter_L");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPinBall_Spliter_L::Free()
{
    super::Free();

    m_pGameInstance->Remove_Light(L"Light_Spliter" + to_wstring(m_iID));

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
    //Safe_Release(m_pPhysicsCom);
}
