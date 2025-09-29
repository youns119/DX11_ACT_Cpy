#include "pch.h"
#include "Fork.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "Player.h"

_int CFork::s_ActivatedFork = true;

CFork::CFork(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : super{ _pDevice, _pContext }
{
}

CFork::CFork(const CFork& _Prototype)
    : super{ _Prototype }
{
}

HRESULT CFork::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CFork::Initialize(void* _pArg)
{
    DESC* pDesc = static_cast<DESC*>(_pArg);
    //pDesc->TransformDesc.fSpeedPerSec = 5.f;
    //m_strName = (pDesc->GameObjectDesc).strName;

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMVectorSet(37.62f, 1.22f, 25.58f, 1.f));
    m_pTransformCom->Rotate(14.175f, 12.8f, -170.601f);
    return S_OK;
}

void CFork::Priority_Update(_float _fTimeDelta)
{
}

void CFork::Update(_float _fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CFork::Late_Update(_float _fTimeDelta)
{
    if (m_bRender)
    {
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);
#ifdef _DEBUG	
        m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif
    }
}

HRESULT CFork::Render()
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

HRESULT CFork::Render_Shadow()
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

void CFork::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER :
    {
        if (m_bRender)
        {
            CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

            // 활성화 된 애인지 확인
            if (true == s_ActivatedFork)
                pUIManager->Show_Interaction(L"들기");
        }
    }
    break;
    default :
        break;
    }
}
void CFork::On_Collision(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        if (m_bRender)
        {
            if (m_pGameInstance->Key_Down(KEY::E))
            {
                CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
                pUIManager->Hide_Interaction();

                s_ActivatedFork = false;
                m_bRender = false;

                CPlayer* pPlayer = static_cast<CPlayer*>(pOther);
                if (pPlayer == nullptr)
                    return;

                pPlayer->Clear_Buffer();
            }
        }
    }
    break;
    default:
        break;
    }
}
void CFork::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
    CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    pUIManager->Hide_Interaction();
}
HRESULT CFork::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Fork",
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))

    {
        if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), L"Prototype_Component_Model_Fork",
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }
    /* Com_Collider */
    {
        CCollider_Sphere::DESC Desc{};
        Desc.ColliderDesc.pOwner = this;
        Desc.ColliderDesc.iCollisionGroup = (_uint)(COLLISION_GROUP::FORK);

        Desc.fRadius = 1.f;
        Desc.vCenter = { 0.f, Desc.fRadius * 0.5f, 0.f };

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_Sphere",
            L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
            return E_FAIL;
    }
    return S_OK;
}

CFork* CFork::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CFork* pInstance = new CFork(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CFork");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CFork::Clone(void* _pArg)
{
    CFork* pInstance = new CFork(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Clone : CFork");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFork::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}
