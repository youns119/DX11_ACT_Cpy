#include "pch.h"
#include "Spong.h"
#include "GameInstance.h"

CSpong::CSpong(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : super{ _pDevice, _pContext }
{
}

CSpong::CSpong(const CSpong& _Prototype)
    : super{ _Prototype }
{
}

HRESULT CSpong::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSpong::Initialize(void* _pArg)
{
    DESC* pDesc = static_cast<DESC*>(_pArg);
    //pDesc->TransformDesc.fSpeedPerSec = 5.f;
    m_strName = (pDesc->GameObjectDesc).strName;

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
    return S_OK;
}

void CSpong::Priority_Update(_float _fTimeDelta)
{
}

void CSpong::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(KEY::M))
        m_bCollision = true;

    if (m_bCollision)
    {
        _float3 vScale = m_pTransformCom->Get_Scale();
        m_fAccTimeDelta += fTimeDelta;

        _float A = 0.5f;  // �ִ� ���� (30% �پ��)
        _float b = 1.5f;  // ���� ��� (������ ����)
        _float w = 7.0f; // ź�� ���ļ�

        _float scaleEffect = A * exp(-b * m_fAccTimeDelta) * cos(w * m_fAccTimeDelta);
        vScale.y = 1.0f - scaleEffect;
        vScale.x = 1.0f + scaleEffect * 0.5f;
        vScale.z = 1.0f + scaleEffect * 0.5f;

        if (m_fAccTimeDelta > 1.6f)
        {
            m_bCollision = false;
            m_fAccTimeDelta = 0.0f;
            vScale = { 1.0f, 1.0f, 1.0f };
        }

        m_pTransformCom->Set_Scale(vScale);
    }

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

}

void CSpong::Late_Update(_float _fTimeDelta)
{
    if (true == m_pGameInstance->Is_InFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE::POSITION), 40.f))
    {// ����ü �ȿ� ���� ���� �ø� 

        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

#ifdef _DEBUG	
        m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif
    }
}

HRESULT CSpong::Render()
{
    if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID"))) // �O�� ����
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

HRESULT CSpong::Render_Shadow()
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

void CSpong::On_CollisionEnter(CGameObject* pGameObject, _uint iGroup)
{
    m_pGameInstance->Play_Sound(L"Spong.wav", (_uint)SOUND_CHANNEL::MAPOBJECT, 0.3f);

    m_bCollision = true;
    m_fAccTimeDelta = 0.f;

    _vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE::UP);

    CMovement* pMovement = static_cast<CMovement*>(pGameObject->Find_Component(L"Com_Movement"));
    if (pMovement == nullptr)
        return;

    pMovement->Add_Force(vUp._vector(), 70.f);

    CTransform* pTransform = static_cast<CTransform*>(pGameObject->Find_Component(g_strTransformTag));
    if (pTransform == nullptr)
        return;

    _vector vPos{pTransform->Get_State(CTransform::STATE::POSITION)}, vLook{XMVectorSet(0.f, 0.f, 1.f, 0.f)};
    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_SPONG_SANDBURST, vPos, vLook, false, nullptr);
}

HRESULT CSpong::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Spong",
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))

    {
        if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), L"Prototype_Component_Model_Spong",
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }

    /* Com_Collider */
    {
        CCollider_OBB::DESC Desc{};
        Desc.ColliderDesc.pOwner = this;
        Desc.vExtents = { 5.f, 2.5f, 5.f };
        Desc.vCenter = { 0.f, 0.1f, 0.f };
        Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MAP_INTERACTION; // ���߿� �ٲٰų� �Ұ�.

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_OBB",
            L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
            return E_FAIL;
    }
    return S_OK;
}

CSpong* CSpong::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CSpong* pInstance = new CSpong(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CSpong");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CSpong::Clone(void* _pArg)
{
    CSpong* pInstance = new CSpong(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Clone : CSpong");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSpong::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}
