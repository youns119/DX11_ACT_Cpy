#include "pch.h"
#include "NPC_GuardCrab.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "CutScene_Manager.h"

#include "Player.h"

CNPC_GuardCrab::CNPC_GuardCrab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CNPC_GuardCrab::CNPC_GuardCrab(const CNPC_GuardCrab& Prototype)
    : super(Prototype)
{
}

HRESULT CNPC_GuardCrab::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_GuardCrab::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);

    if (nullptr == pDesc)
        return E_FAIL;

    if (FAILED(Load_Data(*pDesc)))
        return E_FAIL;

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    m_pCutSceneManager = static_cast<CCutScene_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CutSceneManager", 0));

    m_iDanceIndex = CUtils::Compute_Random(0, 2);

    m_pModelCom->SetUp_Animation(m_iDanceIndex, true, 0.2f);

    return S_OK;
}

void CNPC_GuardCrab::Priority_Update(_float fTimeDelta)
{
}

void CNPC_GuardCrab::Update(_float fTimeDelta)
{
    if (m_pCutSceneManager->Get_CurPlaying() == L"Dance_Fail")
        m_bAnimStop = true;
    else
    {
        if (m_bTalk)
        {
            if (m_pModelCom->Get_CurrAnimationIndex() != (_uint)ANIM_ROAR)
                m_pModelCom->SetUp_Animation((_uint)ANIM_ROAR, true, 0.2f);
        }
        else
        {
            m_bAnimStop = false;

            if (m_pModelCom->Get_CurrAnimationIndex() != m_iDanceIndex)
                m_pModelCom->SetUp_Animation(m_iDanceIndex, true, 0.2f);
        }
    }

    if (!m_bAnimStop)
        m_pModelCom->Play_Animation(fTimeDelta);

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

    if (m_pCutSceneManager->Is_CutSceneFinished(L"Dance"))
        m_pColliderCom->Set_Active(false);
}

void CNPC_GuardCrab::Late_Update(_float fTimeDelta)
{
    if (m_pCutSceneManager->Is_CutSceneFinished(L"Dance"))
        return;
    
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif

}

HRESULT CNPC_GuardCrab::Render()
{
#ifdef _DEBUG
    if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID")))
        return E_FAIL;
#endif // _DEBUG

    if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
        return E_FAIL;

    for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        _uint iShaderPass = 0;
        if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE))
        {
            iShaderPass = (_uint)SHADER_PASS_ANIMMESH::DEFAULT;

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
                return E_FAIL;
        }
        else
        {
            iShaderPass = (_uint)SHADER_PASS_ANIMMESH::BASE_COLOR;

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

HRESULT CNPC_GuardCrab::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix_Ref())))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_ShadowMatrices_To_Shader(m_pShaderCom, "g_LightViewMatrices", "g_LightProjMatrices")))
        return E_FAIL;

    for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
    {

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        m_pShaderCom->Begin((_uint)SHADER_PASS_ANIMMESH::SHADOW);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CNPC_GuardCrab::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        CPlayer* pPlayer = static_cast<CPlayer*>(pOther);
        if (pPlayer == nullptr)
            return;

        pPlayer->KnockBack(35.f, IDamaged::DAMAGE::DAMAGE_HEAVY, this);
        pPlayer->Player_CameraShake_Position(0.07f, 25.f, 1.f);
        m_pGameInstance->Call_Lag(0.01f, 0.08f);

        m_bTalk = true;
        std::function<void()> Callback = [this]()->void
            {
                m_bTalk = false;
            };

        m_pUIManager->Set_DialogueUI(m_iDialogueFileNum, 0, Callback);
        //m_pUIManager->Show_Interaction(L"대화");
    }
    break;
    default:
        break;
    }

}

void CNPC_GuardCrab::On_Collision(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        if (m_pGameInstance->Key_Down(KEY::E))
        {
            m_bTalk = true;
            m_pColliderCom->Set_Active(false);

            std::function<void()> Callback = [this]()->void
                {
                    m_pColliderCom->Set_Active(true);
                    m_bTalk = false;
                };

            m_pUIManager->Set_DialogueUI(m_iDialogueFileNum, 0, Callback);

            CPlayer* pPlayer = static_cast<CPlayer*>(pOther);
            if (pPlayer == nullptr)
                return;

            pPlayer->Clear_Buffer();
        }
    }
    break;
    default:
        break;
    }
}

void CNPC_GuardCrab::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
    //m_pUIManager->Hide_Interaction();
}

HRESULT CNPC_GuardCrab::Load_Data(DESC& Desc)
{
    HRESULT hr{};
    DWORD dwByte{};
    HANDLE hFile = CreateFile(Desc.szFileName, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File");
        CloseHandle(hFile);
        return E_FAIL;
    }

    _float4x4 WorldMat = {}; // 월드 행렬
    _bool bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);

    Desc.GameObjectDesc.WorldMatrix = WorldMat;

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CNPC_GuardCrab::Ready_Components()
{
    /* Com_Model */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_NPC_DanceCrab",
        L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
        L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Collider */
    CCollider_AABB::DESC Desc{};
    {
        Desc.ColliderDesc.pOwner = this;
        Desc.ColliderDesc.iCollisionGroup = (_uint)(COLLISION_GROUP::URCHIN);

        Desc.vExtents = { 2.5f, 2.5f, 2.5f };
        Desc.vCenter = { 0.f, Desc.vExtents.y, 0.f };
    }
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_AABB",
        L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
        return E_FAIL;

    return S_OK;
}

CNPC_GuardCrab* CNPC_GuardCrab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_GuardCrab* pInstance = new CNPC_GuardCrab(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNPC_GuardCrab");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CNPC_GuardCrab::Clone(void* pArg)
{
    CNPC_GuardCrab* pInstance = new CNPC_GuardCrab(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CNPC_GuardCrab");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_GuardCrab::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}
