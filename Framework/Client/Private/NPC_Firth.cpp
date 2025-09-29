#include "pch.h"
#include "NPC_Firth.h"

#include "GameInstance.h"
#include "Player.h"
#include "UI_Manager.h"
#include "CutScene_Manager.h"
#include "NPC_Player.h"

#include "CineCamera.h"

CNPC_Firth::CNPC_Firth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CNPC_Firth::CNPC_Firth(const CNPC_Firth& Prototype)
    : super(Prototype)
{
}

HRESULT CNPC_Firth::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Firth::Initialize(void* pArg)
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

    if (FAILED(Ready_Event()))
        return E_FAIL;

    m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    m_pCutSceneManager = static_cast<CCutScene_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CutSceneManager", 0));

    return S_OK;
}

void CNPC_Firth::Priority_Update(_float fTimeDelta)
{
    super::Priority_Update(fTimeDelta);
}

void CNPC_Firth::Update(_float fTimeDelta)
{
    CNPC_Player* pPlayer = static_cast<CNPC_Player*>(m_pGameInstance->Find_GameObject_With_Name(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_NPC"), TEXT("NPC_Player")));
    _uint iCurDance = (_uint)pPlayer->Get_CurAnim();

    if (m_pCutSceneManager->Get_CurPlaying() == L"Enter_Club")
    {
        m_pGameInstance->Camera_Transition(L"Club_Camera", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
        m_pCineCamera->Play(L"Club", L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
        m_pCutSceneManager->End_CutScene(L"Enter_Club");
    }

    if (m_pCutSceneManager->Is_CutSceneFinished(L"Dance"))
        m_bBattled = false;

    if (m_bTalk)
    {
        if ((_uint)ANIM_TALK != m_pModelCom->Get_CurrAnimationIndex())
            m_pModelCom->SetUp_Animation((_uint)ANIM_TALK, true, 0.2f);
    }
    else
    {
        if (m_pCutSceneManager->Get_CurPlaying() == L"Dance_Fail")
        {
            if ((_uint)ANIM_IDLE != m_pModelCom->Get_CurrAnimationIndex())
                m_pModelCom->SetUp_Animation((_uint)ANIM_IDLE, true, 0.2f);
        }
        else
        {
            if (iCurDance != m_pModelCom->Get_CurrAnimationIndex())
                m_pModelCom->SetUp_Animation(iCurDance, true, 0.2f);
        }
    }

    m_pModelCom->Play_Animation(fTimeDelta);
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

    super::Update(fTimeDelta);
}

void CNPC_Firth::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif

    super::Late_Update(fTimeDelta);
}

HRESULT CNPC_Firth::Render()
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

HRESULT CNPC_Firth::Render_Shadow()
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

void CNPC_Firth::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
    if (!m_bBattled)
    {
        m_bChatRender = true;
        m_pUIManager->Show_Interaction(L"대화");
    }
}

void CNPC_Firth::On_Collision(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        if (m_pGameInstance->Key_Down(KEY::E))
        {
            m_bTalk = true;
            m_pColliderCom->Set_Active(false);

            if (m_iCurState == 0)
            {
                std::function<void()> Callback = [this]()->void
                    {
                        m_iCurState = 1;
                        m_pColliderCom->Set_Active(true);
                        m_bTalk = false;
                        m_pCutSceneManager->Begin_CutScene(L"Dance");
                        CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").front());
                        pPlayer->Toggle_Render(false);

                        m_bBattled = true;
                    };

                m_pUIManager->Set_DialogueUI(m_iDialogueFileNum, m_iCurState, Callback);
            }
            else if (m_iCurState == 1)
            {
                std::function<void()> Callback = [this]()->void
                    {
                        std::function<void()> HideAfterCallback = [this]()->void
                            {
                                m_iCurState = 2;
                                m_pColliderCom->Set_Active(true);
                                m_bTalk = false;
                            };
                        m_pUIManager->Show_GainPassiveSkillUI(3, HideAfterCallback);
                    };

                m_pUIManager->Set_DialogueUI(m_iDialogueFileNum, m_iCurState, Callback);
            }
            else
            {
                std::function<void()> Callback = [this]()->void
                    {
                        m_pColliderCom->Set_Active(true);
                        m_bTalk = false;
                    };

                m_pUIManager->Set_DialogueUI(m_iDialogueFileNum, m_iCurState, Callback);

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

void CNPC_Firth::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
    m_bChatRender = false;

    m_pUIManager->Hide_Interaction();
}

HRESULT CNPC_Firth::Ready_Components()
{
    /* Com_Model */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_NPC_Firth",
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

        Desc.vExtents = { 20.0f, 5.0f,  7.0f };
        Desc.vCenter = { 0.f, Desc.vExtents.y, 0.f };
    }
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_AABB",
        L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Firth::Ready_PartObject()
{
    // CineCamera
    {
        CCineCamera::DESC CineCameraDesc{};

        CineCameraDesc.CameraDesc.fFovY = XMConvertToRadians(60.f);
        CineCameraDesc.CameraDesc.fNear = 0.1f;
        CineCameraDesc.CameraDesc.fFar = 1000.f;

        CineCameraDesc.strName = L"Club_Camera";
        CineCameraDesc.pOwner = this;

        if (FAILED(Add_PartObject(L"Part_CineCamera", LEVEL_STATIC, L"Prototype_GameObject_CineCamera", &CineCameraDesc)))
            return E_FAIL;

        m_pCineCamera = static_cast<CCineCamera*>(Find_PartObject({ L"Part_CineCamera" }));

        m_pCineCamera->Add_Cinematic(L"Club", "../Bin/DataFiles/CameraData/Club_Firth.data");
    }

    return S_OK;
}

HRESULT CNPC_Firth::Ready_Event()
{
    return S_OK;
}

HRESULT CNPC_Firth::Load_Data(DESC& Desc)
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

CNPC_Firth* CNPC_Firth::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_Firth* pInstance = new CNPC_Firth(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNPC_Firth");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CNPC_Firth::Clone(void* pArg)
{
    CNPC_Firth* pInstance = new CNPC_Firth(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CNPC_Firth");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_Firth::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}
