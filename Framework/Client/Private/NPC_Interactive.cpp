#include "pch.h"
#include "NPC_Interactive.h"

#include "GameInstance.h"
#include "Player.h"
#include "Animation.h"

#include "UI_Manager.h"

#include "UI_ChatBubble.h"

CNPC_Interactive::CNPC_Interactive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CNPC_Interactive::CNPC_Interactive(const CNPC_Interactive& Prototype)
    : super(Prototype)
{
}

HRESULT CNPC_Interactive::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Interactive::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    if (FAILED(Load_Data(*pDesc)))
        return E_FAIL;

    if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components(*pDesc)))
        return E_FAIL;

    if (FAILED(Ready_PartObject(*pDesc)))
        return E_FAIL;

    m_iDialogueFileNum = pDesc->iDialogueFileNum;

    m_iIdleAnimNumber = pDesc->iIdleAnimNumber;
    m_iTalkAnimNumber = pDesc->iTalkAnimNumber;

    m_pModelCom->SetUp_Animation(m_iIdleAnimNumber, true, 0.2f);
    m_pModelCom->Play_Animation(1.f / 200.f);

    m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

    return S_OK;
}

void CNPC_Interactive::Priority_Update(_float fTimeDelta)
{
    super::Priority_Update(fTimeDelta);
}

void CNPC_Interactive::Update(_float fTimeDelta)
{
    Check_InFrustum();
    Change_Animation();

    if (m_bInFrustum)
        m_pModelCom->Play_Animation(fTimeDelta);

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

    super::Update(fTimeDelta);
}

void CNPC_Interactive::Late_Update(_float fTimeDelta)
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

HRESULT CNPC_Interactive::Render()
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

HRESULT CNPC_Interactive::Render_Shadow()
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

void CNPC_Interactive::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        if(!m_bTalked)
            m_bChatRender = true;

        m_pUIManager->Show_Interaction(L"대화");
    }
    break;
    default:
        break;
    }
}

void CNPC_Interactive::On_Collision(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        if (m_pGameInstance->Key_Down(KEY::E))
        {
            if (!m_bTalked)
                m_bTalked;

            m_bTalk = true;
            m_pColliderCom->Set_Active(false);
            std::function<void()> Callback = [this]()->void
                {
                    m_bTalk = false;
                    m_pColliderCom->Set_Active(true);
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

void CNPC_Interactive::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
    m_bChatRender = false;
    m_pUIManager->Hide_Interaction();
}

void CNPC_Interactive::Check_InFrustum()
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

void CNPC_Interactive::Change_Animation()
{
    if (m_bTalk)
    {
        if (m_iTalkAnimNumber != m_pModelCom->Get_CurrAnimationIndex())
            m_pModelCom->SetUp_Animation(m_iTalkAnimNumber, true, 0.2f);
    }
    else
    {
        if (m_iIdleAnimNumber != m_pModelCom->Get_CurrAnimationIndex())
            m_pModelCom->SetUp_Animation(m_iIdleAnimNumber, true, 0.2f);
    }
}

HRESULT CNPC_Interactive::Load_Data(DESC& Desc)
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

HRESULT CNPC_Interactive::Ready_Components(DESC& Desc)
{
    /* Com_Model */
    CModel::DESC ModelDesc{};
    ModelDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_STATIC, Desc.strModelTag,
        L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &ModelDesc)))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
        L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Collider */
    CCollider_AABB::DESC ColliderDesc{};
    {
        ColliderDesc.ColliderDesc.pOwner = this;
        ColliderDesc.ColliderDesc.iCollisionGroup = (_uint)(COLLISION_GROUP::NPC);

        ColliderDesc.vExtents = Desc.vColliderExtents;
        ColliderDesc.vCenter = { 0.f, ColliderDesc.vExtents.y, 0.f };
    }
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_AABB",
        L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Interactive::Ready_PartObject(DESC& Desc)
{
    CUI_ChatBubble::DESC ChatDesc{};
    {
        ChatDesc.pRender = &m_bChatRender;

        ChatDesc.UIDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
        ChatDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_ChatBubble.dat";
        ChatDesc.UIDesc.bInWorld = true;
        ChatDesc.UIDesc.fWorldOffsetY = Desc.fChatBubbleHeight;
    }
    if (FAILED(Add_PartObject(L"Part_UI_ChatBubble", LEVEL_STATIC, L"Prototype_GameObject_UI_ChatBubble", &ChatDesc)))
        return E_FAIL;

    return S_OK;
}

CNPC_Interactive* CNPC_Interactive::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_Interactive* pInstance = new CNPC_Interactive(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNPC_Interactive");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CNPC_Interactive::Clone(void* pArg)
{
    CNPC_Interactive* pInstance = new CNPC_Interactive(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CNPC_Interactive");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_Interactive::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}
