#include "pch.h"
#include "StreamVent.h"
#include "GameInstance.h"

#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"

#include "Player.h"

_uint CStreamVent::s_iVentNum = {};

CStreamVent::CStreamVent(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : super{ _pDevice, _pContext }
{
}

CStreamVent::CStreamVent(const CStreamVent& _Prototype)
    : super{ _Prototype }
{
}

HRESULT CStreamVent::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CStreamVent::Initialize(void* _pArg)
{
    DESC* pDesc = static_cast<DESC*>(_pArg);
    //pDesc->TransformDesc.fSpeedPerSec = 5.f;
    m_strName = (pDesc->GameObjectDesc).strName;

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_iVentNum = s_iVentNum++;

    XMStoreFloat4x4(&m_ColliderMatrix, m_pTransformCom->Get_WorldMatrix());
    for (size_t i = 0; i < EFFECT_END; i++)
        m_pEffect[i] = nullptr;

    return S_OK;
}

void CStreamVent::Priority_Update(_float _fTimeDelta)
{
}

void CStreamVent::Update(_float _fTimeDelta)
{
        m_fAccTimeDelta += _fTimeDelta;
        _vec3 vScale = { 1.f, 1.f, 1.f };

        switch (m_eState)
        {
        case STATE::WAITING:
            if (m_bFirst)
            {
                if (m_iVentNum % 2 == 0)
                {
                    m_eState = STATE::STREAMING;
                    m_fAccTimeDelta = 0.f;
                    m_bFirst = false;
                }
                else
                {
                    if (m_fAccTimeDelta >= 2.f)
                    {
                        m_eState = STATE::STREAMING;
                        m_fAccTimeDelta = 0.f;
                        m_bFirst = false;
                        Call_SteamEffect();
                    }
                }
            }
            break;

        case STATE::STREAMING:
            if (m_fAccTimeDelta <= 1.8f)
            {
                _float t = m_fAccTimeDelta / 0.2f;
                vScale.z = 1.f + t * 19.f;
                if (vScale.z >= 20.f)
                    vScale.z = 20.f;
                if (m_iVentNum == 0)
                    m_pGameInstance->Set_ChannelVolume_With_SpatialDesc((_uint)SOUND_CHANNEL::MAPOBJECT, m_pTransformCom->Get_State(CTransform::STATE::POSITION), _vec2(1.f, 256.f), _vec2(0.f, 0.8f));
                else if (m_iVentNum == 1)
                    m_pGameInstance->Set_ChannelVolume_With_SpatialDesc((_uint)SOUND_CHANNEL::SUBMAPOBJECT, m_pTransformCom->Get_State(CTransform::STATE::POSITION), _vec2(1.f, 256.f), _vec2(0.f, 0.8f));

            }
            else if (m_fAccTimeDelta > 1.8f && m_fAccTimeDelta <= 2.f)
            {
                _float t = (m_fAccTimeDelta - 1.8f) / 0.2f;
                vScale.z = 20.f - t * 19.f;
                if (vScale.z < 1.f)
                    vScale.z = 1.f;
                if (m_iVentNum == 0)
                    m_pGameInstance->Set_ChannelVolume_With_SpatialDesc((_uint)SOUND_CHANNEL::MAPOBJECT, m_pTransformCom->Get_State(CTransform::STATE::POSITION), _vec2(1.f, 256.f), _vec2(0.f, 0.5f));
                else if (m_iVentNum == 1)
                    m_pGameInstance->Set_ChannelVolume_With_SpatialDesc((_uint)SOUND_CHANNEL::SUBMAPOBJECT, m_pTransformCom->Get_State(CTransform::STATE::POSITION), _vec2(1.f, 256.f), _vec2(0.f, 0.5f));
            }

            if (m_fAccTimeDelta >= 2.f)
            {
                m_eState = STATE::COOLDOWN;
                m_fAccTimeDelta = 0.f;
                LoopEnd_SteamEffect();
                if (m_iVentNum == 0)
                    m_pGameInstance->Stop_Sound((_uint)SOUND_CHANNEL::MAPOBJECT);
                else if (m_iVentNum == 1)
                    m_pGameInstance->Stop_Sound((_uint)SOUND_CHANNEL::SUBMAPOBJECT);
            }
            break;

        case STATE::COOLDOWN:

            vScale.z = 0.6f;

            if (m_fAccTimeDelta >= 3.f)
            {
                m_eState = STATE::STREAMING;
                m_fAccTimeDelta = 0.f;
                Call_SteamEffect();

                if (m_iVentNum == 2)
                {
                    m_pGameInstance->Play_Sound(L"WaterJet.wav", (_uint)SOUND_CHANNEL::MAPOBJECT, 0.f);
                    m_pGameInstance->Set_ChannelVolume_With_SpatialDesc((_uint)SOUND_CHANNEL::MAPOBJECT, m_pTransformCom->Get_State(CTransform::STATE::POSITION), _vec2(1.f, 256.f), _vec2(0.f, 0.8f));
                }
                else if (m_iVentNum == 1)
                {
                    m_pGameInstance->Play_Sound(L"WaterJet.wav", (_uint)SOUND_CHANNEL::SUBMAPOBJECT, 0.f);
                    m_pGameInstance->Set_ChannelVolume_With_SpatialDesc((_uint)SOUND_CHANNEL::SUBMAPOBJECT, m_pTransformCom->Get_State(CTransform::STATE::POSITION), _vec2(1.f, 256.f), _vec2(0.f, 0.8f));
                }
            }
            break;
        }

        _mat VentColliderMat = m_pTransformCom->Get_WorldMatrix();
        _mat ScaleMatrix = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
        _mat ColliderWorld = ScaleMatrix * VentColliderMat;

        XMStoreFloat4x4(&m_ColliderMatrix, ColliderWorld._matrix());
        m_pColliderCom->Update(XMLoadFloat4x4(&m_ColliderMatrix));
}

void CStreamVent::Late_Update(_float _fTimeDelta)
{
    if (true == m_pGameInstance->Is_InFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE::POSITION), 80.f))
    {// 절두체 안에 있을 때만 컬링 

        _float fMaxRange = m_pGameInstance->Get_FogParams().vRange.y;
        super::Compute_CamDistance();

        if (fMaxRange > m_fCamDistance + 40.f)
        {
            m_bRender = true;
            m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
            m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);
#ifdef _DEBUG
            m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
            //if (nullptr != m_pPhysicsCom && m_pPhysicsCom->Get_Count() > 0)
            if (nullptr != m_pPhysicsCom && m_pPhysicsCom->Is_Simulated())
            {
                m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pPhysicsCom);
            }
#endif // _DEBUG
        }
    }
    else
        m_bRender = false;

}

HRESULT CStreamVent::Render()
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

HRESULT CStreamVent::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix_Ref())))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_ShadowMatrices_To_Shader(m_pShaderCom, "g_LightViewMatrices", "g_LightProjMatrices")))
        return E_FAIL;

    for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
    {
        m_pShaderCom->Begin((_uint)SHADER_PASS_MESH::SHADOW);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CStreamVent::Call_SteamEffect()
{
    if (nullptr != m_pEffect[BUBBLE])
    {
        m_pEffect[BUBBLE]->Clear_Info(0.f);
        m_pEffect[BUBBLE] = nullptr;
    }
    if (nullptr != m_pEffect[SMOKE])
    {
        m_pEffect[SMOKE]->Clear_Info(0.f);
        m_pEffect[SMOKE] = nullptr;
    }

    _vector vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
    _vector vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK) * -1.f;

    m_pEffect[BUBBLE] = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_STEAM_BUBBLE, vPos, vLook, false, nullptr);
    m_pEffect[SMOKE] = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_STEAM_SMOKE, vPos, vLook, false, nullptr);
}

void CStreamVent::LoopEnd_SteamEffect()
{
    if (nullptr != m_pEffect[BUBBLE])
        static_cast<CVIBuffer_InstancingParticle*>(m_pEffect[BUBBLE]->Find_Component(L"Com_VIBuffer"))->Loop_End();
    if (nullptr != m_pEffect[SMOKE])
        static_cast<CVIBuffer_InstancingParticle*>(m_pEffect[SMOKE]->Find_Component(L"Com_VIBuffer"))->Loop_End();
}

HRESULT CStreamVent::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_StreamVent",
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))

    {
        if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), L"Prototype_Component_Model_StreamVent",
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }
    /* Com_Collider */
    {
        CCollider_OBB::DESC Desc{};
        Desc.ColliderDesc.pOwner = this;
        Desc.vExtents = { 1.f, 1.f, 1.f };
        Desc.vCenter = { 0.f, 0.f, 0.f };
        Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::STEAM; // 나중에 바꾸거나 할고.

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_OBB",
            L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
            return E_FAIL;
    }
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

        if (FAILED(Add_Component(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_StreamVent",
            L"Com_Physics", reinterpret_cast<CComponent**>(&m_pPhysicsCom), &Desc)))
            return E_FAIL;
    }


    return S_OK;
}

void CStreamVent::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        CPlayer* pPlayer = static_cast<CPlayer*>(pOther);
        if (pPlayer == nullptr || pPlayer->Get_PlayerStat_Const()->bInvincible)
            return;

        pPlayer->KnockBack(60.f, IDamaged::DAMAGE::DAMAGE_SUPER, this);
        pPlayer->Player_CameraShake_Position(0.07f, 25.f, 1.5);
        m_pGameInstance->Call_Lag(0.01f, 0.1f);
    }
    break;
    default:
        break;
    }
}

void CStreamVent::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CStreamVent::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

CStreamVent* CStreamVent::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CStreamVent* pInstance = new CStreamVent(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CStreamVent");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CStreamVent::Clone(void* _pArg)
{
    CStreamVent* pInstance = new CStreamVent(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Clone : CStreamVent");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStreamVent::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pPhysicsCom);
}
