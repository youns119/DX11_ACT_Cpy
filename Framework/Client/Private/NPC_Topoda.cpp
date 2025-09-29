#include "pch.h"
#include "NPC_Topoda.h"

#include "GameInstance.h"
#include "Player.h"

#include "UI_Manager.h"
#include "NPC_Manager.h"

#include "UI_LocationIcon.h"
#include "UI_ChatBubble.h"

CNPC_Topoda::CNPC_Topoda(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CNPC_Topoda::CNPC_Topoda(const CNPC_Topoda& Prototype)
    : super(Prototype)
{
}

HRESULT CNPC_Topoda::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Topoda::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    if (FAILED(Load_Data(*pDesc)))
        return E_FAIL;

    if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObject()))
        return E_FAIL;

    m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    m_pNPCManager = static_cast<CNPC_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_NPCManager", 0));
    Safe_AddRef(m_pNPCManager);

    m_iCurState = m_pNPCManager->Load_State(L"Topoda");

    return S_OK;
}

void CNPC_Topoda::Priority_Update(_float fTimeDelta)
{
    super::Priority_Update(fTimeDelta);
}

void CNPC_Topoda::Update(_float fTimeDelta)
{
    Check_InFrustum();
    Change_Animation();

    if (m_bInFrustum)
        m_pModelCom->Play_Animation(fTimeDelta);

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

    super::Update(fTimeDelta);
}

void CNPC_Topoda::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

    if (!m_bInFrustum)
        return;

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif

    super::Late_Update(fTimeDelta);
}

HRESULT CNPC_Topoda::Render()
{
    if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID")))
        return E_FAIL;

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
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
        {
            iShaderPass = (_uint)SHADER_PASS_ANIMMESH::BASE_COLOR; // pass base color;
            m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor");
        }
        else if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::NORMAL))
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::NORMAL, "g_NormalTexture")))
                return E_FAIL;
        }
        else
        {
            iShaderPass = (_uint)SHADER_PASS_ANIMMESH::NON_NORMAL_MAPPING;
        }

        m_pShaderCom->Begin(iShaderPass);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CNPC_Topoda::Render_Shadow()
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

void CNPC_Topoda::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        m_bChatRender = true;
        m_pUIManager->Show_Interaction(L"대화");
    }
    break;
    default:
        break;
    }
}

void CNPC_Topoda::On_Collision(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        if (m_pGameInstance->Key_Down(KEY::E))
        {
            m_bTalk = true;
            m_pColliderCom->Set_Active(false);

            switch (m_iCurState)
            {
            case 0: // 첫대화
                Gain_Skill();
                break;
            case 1: // 나머지
            {
                std::function<void()> Callback = [this]()->void
                    {
                        m_pColliderCom->Set_Active(true);
                    };
                m_pUIManager->Set_DialogueUI(m_iDialogueFileNum, m_iCurState, Callback);
            }
                break;
            default:
                break;
            }

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

void CNPC_Topoda::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
    m_bChatRender = false;
    m_pUIManager->Hide_Interaction();
}

void CNPC_Topoda::Check_InFrustum()
{
    // 절두체 안에 있을 때만 컬링
    if (m_pGameInstance->Is_InFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE::POSITION), 20.f))
    {
        _float fMaxRange = m_pGameInstance->Get_FogParams().vRange.y;
        super::Compute_CamDistance();

        if (fMaxRange > m_fCamDistance + 20.f)
            m_bInFrustum = true;
    }
    else
        m_bInFrustum = false;
}

void CNPC_Topoda::Change_Animation()
{
    if (m_bTalk)
    {
        if ((_uint)ANIM_TALK != m_pModelCom->Get_CurrAnimationIndex())
            m_pModelCom->SetUp_Animation((_uint)ANIM_TALK, true, 0.2f);
    }
    else
    {
        if ((_uint)ANIM_IDLE != m_pModelCom->Get_CurrAnimationIndex())
            m_pModelCom->SetUp_Animation((_uint)ANIM_IDLE, true, 0.2f);
    }
}

void CNPC_Topoda::Gain_Skill()
{
    std::function<void()> Callback = [this]()->void
        {    
            std::function<void()> HideAfterCallback = [this]()->void
                {
                    m_iCurState = 1;
                    m_pColliderCom->Set_Active(true);
                    m_bTalk = false;
                };
            m_pUIManager->Show_GainActiveSkillUI(0, HideAfterCallback);
        };
    m_pUIManager->Set_DialogueUI(m_iDialogueFileNum, m_iCurState, Callback);
}

HRESULT CNPC_Topoda::Ready_Components()
{
    /* Com_Model */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_NPC_Topoda",
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
        Desc.ColliderDesc.iCollisionGroup = (_uint)(COLLISION_GROUP::NPC);

        Desc.vExtents = { 0.1f, 0.1f, 0.1f };
        Desc.vCenter = { 0.f, Desc.vExtents.y, 0.f };
    }
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_AABB",
        L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Topoda::Ready_PartObject()
{
    CUI_ChatBubble::DESC ChatDesc{};
    {
        ChatDesc.pRender = &m_bChatRender;

        ChatDesc.UIDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
        ChatDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_ChatBubble.dat";
        ChatDesc.UIDesc.bInWorld = true;
        ChatDesc.UIDesc.fWorldOffsetY = 3.5f;
    }
    if (FAILED(Add_PartObject(L"Part_UI_ChatBubble", LEVEL_STATIC, L"Prototype_GameObject_UI_ChatBubble", &ChatDesc)))
        return E_FAIL;

    CUI_LocationIcon::DESC LocationIconDesc{};
    {
        LocationIconDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_LocationIcon.dat";
        LocationIconDesc.pNPCWorld = &m_pTransformCom->Get_WorldMatrix_Ref();
    }
    if (FAILED(Add_PartObject(L"Part_UI_LocationIcon", LEVEL_STATIC, L"Prototype_GameObject_UI_LocationIcon", &LocationIconDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Topoda::Load_Data(DESC& Desc)
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

CNPC_Topoda* CNPC_Topoda::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_Topoda* pInstance = new CNPC_Topoda(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNPC_Topoda");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CNPC_Topoda::Clone(void* pArg)
{
    CNPC_Topoda* pInstance = new CNPC_Topoda(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CNPC_Topoda");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_Topoda::Free()
{
    super::Free();

    if (m_pNPCManager)
        m_pNPCManager->Save_State(L"Topoda", m_iCurState);

    Safe_Release(m_pNPCManager);

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}
