#include "pch.h"
#include "HeartKelp.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "UI_Manager.h"

#include "Player.h"

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

    m_bActive = true;

    {/* PointLight */

        _vector vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
        vPos = XMVectorSetY(vPos, (XMVectorGetY(vPos) + 2.5f));


        LIGHT_DESC LightDesc = {};
        LightDesc.eType = LIGHT_TYPE::POINT;
        LightDesc.fRange = 20.f;
        XMStoreFloat4(&LightDesc.vPosition, vPos);
        LightDesc.vDiffuse = _float4(8.f, 8.f, 0.5f, 1.f);
        LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
        //LightDesc.vSpecular = m_vSpecular;
        LightDesc.vAttenuation = _float3(1.f, 1.f, 0.8f);

        _wstring strLightTag = L"Light_HeartKelp" + to_wstring(m_iID);
        m_pGameInstance->Add_Light(strLightTag, LightDesc);

    }

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
    if (!m_bActive)
        return;
        

    if (true == m_pGameInstance->Is_InFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE::POSITION), 1.5f))
    {// 절두체 안에 있을 때만 컬링 

        _float fMaxRange = m_pGameInstance->Get_FogParams().vRange.y;
        super::Compute_CamDistance();

        if (fMaxRange > m_fCamDistance + 1.5f)
        {
            m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
            m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);
#ifdef _DEBUG
            m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif // _DEBUG
        }
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

HRESULT CHeartKelp::Render_Shadow()
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
        Desc.ColliderDesc.iCollisionGroup = (_uint)(COLLISION_GROUP::HEARTKELP);

        Desc.vExtents = { 3.0f, 3.0f,  3.0f };
        Desc.vCenter = { 0.f, Desc.vExtents.y, 0.f };

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_AABB",
            L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

void CHeartKelp::On_CollisionEnter(CGameObject* pGameObject, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER :
    {
        CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

        if (m_bActive)
            pUIManager->Show_Interaction(L"뽑기");
    }
    break;
    default :
        break;
    }
}

void CHeartKelp::On_Collision(CGameObject* pGameObject, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        if (m_pGameInstance->Key_Down(KEY::E))
        {
            if (m_bRender)
            {
                CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
                m_pGameInstance->Delete_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_InteractionObject", this);
                m_bRender = false;

                m_pGameInstance->Turn_Off_Light(L"Light_HeartKelp" + to_wstring(m_iID));
            }
            CPlayer* pPlayer = static_cast<CPlayer*>(pGameObject);
            if (pPlayer == nullptr)
                return;

            if (pPlayer->Get_PlayerStat_Const()->bUseItem)
            {
                CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
                pUIManager->Hide_Interaction();

                m_bActive = false;
            }

            pPlayer->Clear_Buffer();
        }
    }
    break;
    default:
        break;
    }
}

void CHeartKelp::On_CollisionExit(CGameObject* pGameObject, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
        pUIManager->Hide_Interaction();
    }
    break;
    default:
        break;
    }
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

    m_pGameInstance->Remove_Light(L"Light_HeartKelp" + to_wstring(m_iID));

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);   
}
