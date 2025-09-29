#include "pch.h"
#include "PinBall_Bumper.h"
//#include "Player.h"
#include "GameInstance.h"

#include "Player.h"
#include "Movement.h"

CPinBall_Bumper::CPinBall_Bumper(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : super{ _pDevice, _pContext }
{
}

CPinBall_Bumper::CPinBall_Bumper(const CPinBall_Bumper& _Prototype)
    : super{ _Prototype }
{
}

HRESULT CPinBall_Bumper::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPinBall_Bumper::Initialize(void* _pArg)
{
    CGameObject::DESC* pDesc = static_cast<CGameObject::DESC*>(_pArg);

    m_strName = pDesc->strName;
    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE::POSITION));

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

        _wstring strLightTag = L"Light_Bumper" + to_wstring(m_iID);
        m_pGameInstance->Add_Light(strLightTag, LightDesc);
        m_pGameInstance->Turn_Off_Light(strLightTag);
    }

    return S_OK;
}

void CPinBall_Bumper::Priority_Update(_float fTimeDelta)
{
}

void CPinBall_Bumper::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(KEY::M) && !m_bCollision)
    {
        m_bCollision = true;
        //XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE::POSITION));

    }
    if (m_bCollision)
    {
        m_fAccTimeDelta += fTimeDelta;
        _vector vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);

        _float fPosY = {};
        //_float fPosY = XMVectorGetY(vPos);

        if (m_fAccTimeDelta <= 0.15f)
        {
            _float fRatio = m_fAccTimeDelta / 0.15f;
            fPosY = m_vPosition.y - (0.6f * fRatio);
        }
        else if (m_fAccTimeDelta <= 0.3f)
        {
            _float fRatio = (m_fAccTimeDelta - 0.15f) / 0.15f;
            fPosY = (m_vPosition.y - 0.6f) + (0.6f * fRatio);
        }
        else
        {
            m_bCollision = false;
            m_fAccTimeDelta = 0.0f;
            fPosY = m_vPosition.y; 

            _wstring strLightTag = L"Light_Bumper" + to_wstring(m_iID);
            m_pGameInstance->Turn_Off_Light(strLightTag);
        }

        vPos = XMVectorSetY(vPos, fPosY);

        m_pTransformCom->Set_State(CTransform::STATE::POSITION, vPos);
    }

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CPinBall_Bumper::Late_Update(_float _fTimeDelta)
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

HRESULT CPinBall_Bumper::Render()
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

void CPinBall_Bumper::On_CollisionEnter(CGameObject* pGameObject, _uint iGroup)
{
    m_bCollision = true;

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
        pMovement->Add_Force(vDir._vector(), 15.f);
    }
    break;
    default:
        break;
    }
    m_pGameInstance->Play_Sound(L"Pinbarge_Bumper1.wav", (_uint)SOUND_CHANNEL::MAPOBJECT, 1.f);
}

void CPinBall_Bumper::On_Collision(CGameObject* pGameObject, _uint iGroup)
{
    _wstring strLightTag = L"Light_Bumper" + to_wstring(m_iID);
    m_pGameInstance->Turn_On_Light(strLightTag);
}

void CPinBall_Bumper::On_CollisionExit(CGameObject* pGameObject, _uint iGroup)
{
    if (m_bCollision == false)
    {
        _wstring strLightTag = L"Light_Bumper" + to_wstring(m_iID);
        m_pGameInstance->Turn_Off_Light(strLightTag);
    }
}

_vector CPinBall_Bumper::Get_OffsetPosition()
{
    return m_pTransformCom->Get_State(CTransform::STATE::POSITION);
}

HRESULT CPinBall_Bumper::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_PINBALL, L"Prototype_Component_Model_BumperBottom",
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Collider */
    {
        CCollider_Sphere::DESC Desc{};
        Desc.ColliderDesc.pOwner = this;
        Desc.fRadius = 10.f;
        Desc.vCenter = { 0.f, Desc.fRadius, 0.f };
        Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::PINBALL; // 나중에 바꾸거나 할고.

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_Sphere",
            L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
            return E_FAIL;
    }

    return S_OK;

}

CPinBall_Bumper* CPinBall_Bumper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPinBall_Bumper* pInstance = new CPinBall_Bumper(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CPinBall_Bumper");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CPinBall_Bumper::Clone(void* pArg)
{
    CPinBall_Bumper* pInstance = new CPinBall_Bumper(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CPinBall_Bumper");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPinBall_Bumper::Free()
{
    super::Free();

    m_pGameInstance->Remove_Light(L"Light_Bumper" + to_wstring(m_iID));

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}
