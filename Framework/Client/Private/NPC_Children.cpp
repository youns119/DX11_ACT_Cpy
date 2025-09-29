#include "pch.h"
#include "NPC_Children.h"

#include "GameInstance.h"
#include "Player.h"

#include "UI_Manager.h"
#include "NPC_Manager.h"

#include "UI_LocationIcon.h"
#include "UI_ChatBubble.h"

const _float3 CNPC_Children::s_vCenter = { 212.63f, 9.82f, 309.01f };
_float CNPC_Children::s_fTime = {0.f};
_bool CNPC_Children::s_bTalk = { false };
_int CNPC_Children::s_iCurState = { 0 };

CNPC_Children::CNPC_Children(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CNPC_Children::CNPC_Children(const CNPC_Children& Prototype)
    : super(Prototype)
{
}

HRESULT CNPC_Children::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Children::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_PartObject()))
        return E_FAIL;

    m_fAngleOffset = pDesc->fAngleOffset;

    m_pModelCom->SetUp_Animation((_uint)ANIM_RUN, true, 0.2f);

    m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    m_pNPCManager = static_cast<CNPC_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_NPCManager", 0));
    Safe_AddRef(m_pNPCManager);

    s_iCurState = m_pNPCManager->Load_State(L"Children");

    return S_OK;
}

void CNPC_Children::Priority_Update(_float fTimeDelta)
{
    super::Priority_Update(fTimeDelta);
}

void CNPC_Children::Update(_float fTimeDelta)
{
    Check_InFrustum();

    if (s_bTalk)
        Talk();
    else
        Move(fTimeDelta);

    if (m_bInFrustum)
        m_pModelCom->Play_Animation(fTimeDelta);

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

    super::Update(fTimeDelta);
}

void CNPC_Children::Late_Update(_float fTimeDelta)
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

HRESULT CNPC_Children::Render()
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

HRESULT CNPC_Children::Render_Shadow()
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

void CNPC_Children::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        m_pUIManager->Show_Interaction(L"대화");
    }
    break;
    default:
        break;
    }
}

void CNPC_Children::On_Collision(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        if (m_pGameInstance->Key_Down(KEY::E))
        {
            m_pColliderCom->Set_Active(false);

            std::function<void()> Callback = [this]()->void
                {
                    if (s_iCurState == 0)
                    {
                        s_iCurState = 1;
                        m_pUIChatBubble->Set_IsActive(false);
                    }
                    s_bTalk = false;
                    m_pColliderCom->Set_Active(true);
                };
            m_pUIManager->Set_DialogueUI(m_iDialogueFileNum, s_iCurState, Callback);
            s_bTalk = true;

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

void CNPC_Children::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
    m_pUIManager->Hide_Interaction();
}

void CNPC_Children::Check_InFrustum()
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

void CNPC_Children::Talk()
{
    if ((_uint)ANIM_TALK != m_pModelCom->Get_CurrAnimationIndex())
        m_pModelCom->SetUp_Animation((_uint)ANIM_TALK, true, 0.2f);

    CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
    CTransform* pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
    m_pTransformCom->LookAt(pPlayerTransform->Get_State(CTransform::STATE::POSITION));
}

void CNPC_Children::Move(_float fTimeDelta)
{
    if ((_uint)ANIM_RUN != m_pModelCom->Get_CurrAnimationIndex())
        m_pModelCom->SetUp_Animation((_uint)ANIM_RUN, true, 0.2f);

    s_fTime += fTimeDelta;

    _float fAngle = s_fTime * m_fRotationSpeed + m_fAngleOffset;

    _float x = s_vCenter.x + cosf(fAngle) * s_fRadius;
    _float z = s_vCenter.z + sinf(fAngle) * s_fRadius;

    _vector vPosition = XMVectorSet(x, s_vCenter.y, z, 1.f);
    _vector vCenter = XMLoadFloat3(&s_vCenter);

    m_pTransformCom->LookAt(vPosition);
    m_pTransformCom->Set_State(CTransform::STATE::POSITION, vPosition);
}

HRESULT CNPC_Children::Ready_Components(DESC* pDesc)
{
    /* Com_Model */
    if (FAILED(Add_Component(LEVEL_STATIC, pDesc->strModelTag,
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

        Desc.vExtents = { 7.0f, 5.0f, 7.0f };
        Desc.vCenter = { 0.f, Desc.vExtents.y, 0.f };
    }
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_AABB",
        L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Children::Ready_PartObject()
{
    CUI_ChatBubble::DESC ChatDesc{};
    {
        ChatDesc.pRender = &m_bChatRender;

        ChatDesc.UIDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
        ChatDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_ChatBubble.dat";
        ChatDesc.UIDesc.bInWorld = true;
        ChatDesc.UIDesc.fWorldOffsetY = 3.0f;
    }
    if (FAILED(Add_PartObject(L"Part_UI_ChatBubble", LEVEL_STATIC, L"Prototype_GameObject_UI_ChatBubble", &ChatDesc)))
        return E_FAIL;

    m_pUIChatBubble = static_cast<CUI_ChatBubble*>(Find_PartObject({ L"Part_UI_ChatBubble" }));

    CUI_LocationIcon::DESC LocationIconDesc{};
    {
        LocationIconDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_LocationIcon.dat";
        LocationIconDesc.pNPCWorld = &m_pTransformCom->Get_WorldMatrix_Ref();
    }
    if (FAILED(Add_PartObject(L"Part_UI_LocationIcon", LEVEL_STATIC, L"Prototype_GameObject_UI_LocationIcon", &LocationIconDesc)))
        return E_FAIL;

    m_pUILocIcon = static_cast<CUI_LocationIcon*>(Find_PartObject({ L"Part_UI_LocationIcon" }));

    return S_OK;
}

CNPC_Children* CNPC_Children::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_Children* pInstance = new CNPC_Children(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNPC_Children");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CNPC_Children::Clone(void* pArg)
{
    CNPC_Children* pInstance = new CNPC_Children(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CNPC_Children");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_Children::Free()
{
    super::Free();

    if (m_pNPCManager)
        m_pNPCManager->Save_State(L"Children", s_iCurState);

    Safe_Release(m_pNPCManager);

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}
