#include "pch.h"
#include "NPC_Shark.h"

#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"
#include "Animation.h"
#include "UI_Manager.h"
#include "CutScene_Manager.h"

#include "UI_ChatBubble.h"
#include "NPC_Shark_Shell.h"
#include "CineCamera.h"

#include "NPC_Shark_Event_Appear.h"
#include "NPC_Shark_Event_Knock.h"
#include "NPC_Shark_Event_Awake.h"
#include "NPC_Shark_Event_Bite.h"
#include "NPC_Shark_Event_Spit.h"

#include "NPC_Shark_Event_Pause.h"
#include "NPC_Shark_Event_Stop.h"

#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"

CNPC_Shark::CNPC_Shark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CNPC_Shark::CNPC_Shark(const CNPC_Shark& Prototype)
    : super(Prototype)
{
}

HRESULT CNPC_Shark::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Shark::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    pDesc->GameObjectDesc.fSpeedPerSec = 2.f;

    if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Mouth.7.5");

    if (FAILED(Ready_PartObject()))
        return E_FAIL;

    if (FAILED(Ready_Event()))
        return E_FAIL;

    m_pModelCom->SetUp_Animation((_uint)ANIM_HIDE, true, 0.2f);
    m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat3(&m_vStartPosition));
    
    m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    
    m_pCutSceneManager = static_cast<CCutScene_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CutSceneManager", 0));
    m_pCutSceneManager->Begin_CutScene(L"Beach_Start");

    m_pGameInstance->Camera_Transition(L"Shark_Camera", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
    m_pCineCamera->Play(L"Beach_Start", L"Beach_Shell", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
    m_pCineCamera->Pause();

    m_pColliderCom->Set_Active(false);
    return S_OK;
}

void CNPC_Shark::Priority_Update(_float fTimeDelta)
{
    super::Priority_Update(fTimeDelta);
}

void CNPC_Shark::Update(_float fTimeDelta)
{
    // 임시
    if (m_pGameInstance->Key_Down(KEY::EQUALS))
    {
        m_pUIManager->Hide_CutSceneUI();
        m_pCineCamera->Play(L"Beach_Start", L"Camera_Player", 0.1f);
        m_pCineCamera->Stop();
    }

    if (!m_isActive)
        return;

    if (m_pCutSceneManager->Get_CurPlaying() == L"Beach_Start")
        Update_StartCutScene(fTimeDelta);
    else if (m_pCutSceneManager->Get_CurPlaying() == L"Beach_End")
        Update_EndCutScene(fTimeDelta);
    else
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
   
    m_pModelCom->Play_Animation(fTimeDelta);
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

    super::Update(fTimeDelta);
}

void CNPC_Shark::Late_Update(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif

    super::Late_Update(fTimeDelta);
}

HRESULT CNPC_Shark::Render()
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

HRESULT CNPC_Shark::Render_Shadow()
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

void CNPC_Shark::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
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

void CNPC_Shark::On_Collision(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        if (m_pGameInstance->Key_Down(KEY::E))
        {
            m_pColliderCom->Set_Active(false);
            m_bTalk = true;
            std::function<void()> Callback = [this]()->void
                {
                    m_bTalk = false;

                    m_pGameInstance->FadeSwap_Sound(L"Chase.wav", (_uint)SOUND_CHANNEL::BGM, 0.5f, 0.5f);

                    m_pCutSceneManager->Begin_CutScene(L"Beach_End");
                    m_eCurState = STATE_BITE;
                    m_pGameInstance->Camera_Transition(L"Shark_Camera", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.f);
                    m_pCineCamera->Play(L"Beach_End", L"Camera_Player", 0.1f);
                    m_pCineCamera->Pause();
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

void CNPC_Shark::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        m_bChatRender = false;
        m_pUIManager->Hide_Interaction();
    }
    break;
    default:
        break;
    }
}

void CNPC_Shark::Event_Appear()
{
    // 물 튀는 이펙트
    Call_Effect_WaterSplash();
    m_pGameInstance->Play_Sound(L"WaterSplash.wav", (_uint)SOUND_CHANNEL::NPC, 1.f);
    m_pGameInstance->Play_Sound_Loop(L"Beach_BGM.wav", (_uint)SOUND_CHANNEL::BGM, 0.5f);
}

void CNPC_Shark::Event_Knock()
{
    // 스파크? 튀는 이펙트
    // 노크 사운드 찾아서 넣기
    m_pGameInstance->Play_Sound(L"ImpactShell_CeramicL22.wav", (_uint)SOUND_CHANNEL::MAPOBJECT, 1.f);
    m_pGameInstance->Play_Sound(L"ImpactShell_CeramicL12.wav", (_uint)SOUND_CHANNEL::MAPOBJECT, 1.f);
    // 카메라 조금 이상함
    Call_Effect_ShellSpark();
    m_pCineCamera->PositionShake(0.1f, 20.f, 0.1f);
    m_pCineCamera->RotationShake(0.1f, 20.f, 0.1f);

}

void CNPC_Shark::Event_Awake()
{
    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player"));
    if (pPlayer)
        pPlayer->Get_StateMachine()->ChangeState(L"ShellParry");

    m_bPlayerAwaked = true;
    m_pCineCamera->Pause();
    m_pUIManager->Set_CutSceneUI_Caption(L"크릴: 헉 새가 아니었네");
}

void CNPC_Shark::Event_Bite()
{
    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player"));
    if (pPlayer)
        pPlayer->Grabbed(m_pSocketMatrix, &m_pTransformCom->Get_WorldMatrix_Ref());

    Call_Effect_ShellSpark();

    m_pGameInstance->Play_Sound(L"Impact Flesh 1.wav", (_uint)SOUND_CHANNEL::MAPOBJECT, 1.f);

}

void CNPC_Shark::Event_Spit()
{
    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player"));
    if (pPlayer)
    {
        pPlayer->UnGrab(35.f, IDamaged::DAMAGE::DAMAGE_HEAVY, this);
        pPlayer->Shell_Damage(999.f);
    }

    static_cast<CNPC_Shark_Shell*>(Find_PartObject({ L"Part_Shell" }))->Set_IsActive(true);
}

void CNPC_Shark::Pause_CineCamera()
{
    m_pCineCamera->Pause();
}

void CNPC_Shark::Stop_CineCamera()
{
    m_pCineCamera->Stop();
}

void CNPC_Shark::Update_StartCutScene(_float fTimeDelta)
{
    if (m_eCurState == STATE_SWIM)
    {
        _vec3 vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
        _vec3 vGoal = XMLoadFloat3(&m_vGoalPosition);

        _float fDistance = (vGoal - vPosition).Length();

        if (fDistance < 0.1f)
        {
            m_pModelCom->SetUp_Animation((_uint)ANIM_APPEAR, false, 0.2f);
            m_eCurState = STATE_APPEAR;
            m_pCineCamera->Resume();
        }
        else
        {
            m_pTransformCom->LookAt(vGoal._vector());
            m_pTransformCom->Go_Straight(fTimeDelta);
        }

    }
    else if (m_eCurState == STATE_APPEAR)
    {
        if (m_pModelCom->Get_CurrAnimation()->Get_Blendable())
        {
            m_pModelCom->SetUp_Animation((_uint)ANIM_KNOCK, false, 0.2f);
            m_eCurState = STATE_KNOCK;
        }
    }
    else if (m_eCurState == STATE_KNOCK)
    {
        if (m_pModelCom->Get_CurrAnimation()->Get_Blendable())
        {
            m_pModelCom->SetUp_Animation((_uint)ANIM_IDLE, true, 0.2f);
            m_pCineCamera->Stop();
            m_pCineCamera->Play(L"Beach_Shell", L"Beach_Talk", 0.1f);
            m_pCineCamera->Pause();
            m_eCurState = STATE_WAIT;
            m_pGameInstance->Play_Sound(L"Beach_Kril_00.wav", (_uint)SOUND_CHANNEL::CONVERSATION1, 1.f);
            m_pUIManager->Set_CutSceneUI_Caption(L"크릴: 저리가! 아무도 없어!");
        }
    }
    else if (m_eCurState == STATE_WAIT)
    {
        if (!m_pGameInstance->IsPlaying((_uint)SOUND_CHANNEL::CONVERSATION1))
        {
            m_eCurState = STATE_TALK;
            m_pCineCamera->Stop();
            m_pCineCamera->Play(L"Beach_Talk", L"Beach_Kril", 0.1f);
            //m_pCineCamera->Pause();

            m_pGameInstance->Play_Sound(L"Beach_Shark_00.wav", (_uint)SOUND_CHANNEL::CONVERSATION2, 1.f);
            m_pUIManager->Set_CutSceneUI_Caption(L"상어채업자: 선생님, 나와보세요");
        }
    }
    else if (m_eCurState == STATE_TALK)
    {
        if (m_bPlayerAwaked)
        {
            if (!m_pGameInstance->IsPlaying((_uint)SOUND_CHANNEL::CONVERSATION1))
            {
                m_pGameInstance->Play_Sound(L"Beach_Shark_01.wav", (_uint)SOUND_CHANNEL::CONVERSATION2, 1.f);
                m_pUIManager->Set_CutSceneUI_Caption(L"상어채업자: 선생님, 집주인 되십니까?");

                if (m_pModelCom->Get_CurrAnimationIndex() != (_uint)ANIM_TALK)
                    m_pModelCom->SetUp_Animation((_uint)ANIM_TALK, true, 0.2f);

                m_bPlayerAwaked = false;
                m_eCurState = STATE_LOOK;
            }
            else
            {
                if (m_pModelCom->Get_CurrAnimationIndex() != (_uint)ANIM_IDLE)
                    m_pModelCom->SetUp_Animation((_uint)ANIM_IDLE, true, 0.2f);
            }
        }

        if (m_pGameInstance->IsPlaying((_uint)SOUND_CHANNEL::CONVERSATION2))
        {
            if (m_pModelCom->Get_CurrAnimationIndex() != (_uint)ANIM_TALK)
                m_pModelCom->SetUp_Animation((_uint)ANIM_TALK, true, 0.2f);
        }
        else
        {
            if (m_pModelCom->Get_CurrAnimationIndex() != (_uint)ANIM_IDLE)
                m_pModelCom->SetUp_Animation((_uint)ANIM_IDLE, true, 0.2f);
        }
    }
    else if (m_eCurState == STATE_LOOK)
    {
        if (!m_pGameInstance->IsPlaying((_uint)SOUND_CHANNEL::CONVERSATION2))
        {
            if (m_pModelCom->Get_CurrAnimationIndex() != (_uint)ANIM_IDLE)
                m_pModelCom->SetUp_Animation((_uint)ANIM_IDLE, true, 0.2f);

            m_eCurState = STATE_END;
            m_pCineCamera->Stop();
            m_pCineCamera->Play(L"Beach_Kril", L"Camera_Player", 0.1f);
            m_pCineCamera->Pause();

            m_pGameInstance->Play_Sound(L"Beach_Kril_02.wav", (_uint)SOUND_CHANNEL::CONVERSATION1, 1.f);
            m_pUIManager->Set_CutSceneUI_Caption(L"크릴: 등껍질 말하시는건가요? 네 제껀데요 예쁘죠?");

            CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player"));
            if (pPlayer)
                pPlayer->Get_StateMachine()->ChangeState(L"CutScene_Talk");
        }
    }
    else if (m_eCurState == STATE_END)
    {
        if (!m_pGameInstance->IsPlaying((_uint)SOUND_CHANNEL::CONVERSATION1))
        {
            m_pCineCamera->Stop();
            m_pCutSceneManager->End_CutScene(L"Beach_Start");
            m_pColliderCom->Set_Active(true);
        }
    }
}

void CNPC_Shark::Update_EndCutScene(_float fTimeDelta)
{
    if (m_eCurState == STATE_BITE)
    {
        if (m_pModelCom->Get_CurrAnimationIndex() != (_uint)ANIM_ATTACK)
            m_pModelCom->SetUp_Animation((_uint)ANIM_ATTACK, false, 0.2f);

        if (m_pModelCom->Get_CurrAnimation()->Get_Blendable())
        {
            m_pModelCom->SetUp_Animation((_uint)ANIM_DISAPPEAR, false, 0.2f);
            m_eCurState = STATE_HIDE;
        }
    }
    else if (m_eCurState == STATE_HIDE)
    {
        if (m_pModelCom->Get_CurrAnimation()->Get_Blendable())
        {
            m_pCineCamera->Stop();
            m_pCutSceneManager->End_CutScene(L"Beach_End");
            m_isActive = false;
        }
    }
}

void CNPC_Shark::Call_Effect_WaterSplash()
{
     const _float4x4* WorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
    const _float4x4* BoneMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Head.6.4");
    _vector vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
    _vector vPos = (XMLoadFloat4x4(BoneMatrix) * XMLoadFloat4x4(WorldMatrix)).r[(_uint)CTransform::STATE::POSITION];
    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTSCENE_MESH_SHARK_WATERSPLASH, vPos, vZeroLook, false, nullptr);
    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTSCENE_PTCL_SHARK_WATERSPLASH, vPos, vZeroLook, false, nullptr);
    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTSCENE_SPRT_SHARK_WATERSPLASH, vPos, vZeroLook, false, nullptr);
}

void CNPC_Shark::Call_Effect_ShellSpark()
{
    const _float4x4* WorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
    const _float4x4* BoneMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Mouth.7.5");
    _vector vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
    _vector vLook = XMLoadFloat4x4(WorldMatrix).r[(_uint)CTransform::STATE::LOOK];
    _vector vUp = XMLoadFloat4x4(WorldMatrix).r[(_uint)CTransform::STATE::UP];
    _vector vPivot = vLook * 0.4f - vUp * 0.2f;
    _vector vPos = (XMLoadFloat4x4(BoneMatrix) * XMLoadFloat4x4(WorldMatrix)).r[(_uint)CTransform::STATE::POSITION] + vPivot;
    CEffectObject* pEffect = nullptr;
    pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PTCL_HITSPARK, vPos, vZeroLook, false, nullptr);
    if (pEffect != nullptr)
        static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(vPivot);
    pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HIT, vPos, vZeroLook, false, nullptr);
    if (pEffect != nullptr)
        static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(vPivot);
    pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_SPRITE_HIT, vPos, vZeroLook, false, nullptr);
    if (pEffect != nullptr)
        static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(vPivot);
}

HRESULT CNPC_Shark::Ready_Components()
{
    /* Com_Model */
    CModel::DESC ModelDesc{};
    ModelDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_NPC_Shark",
        L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &ModelDesc)))
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

        Desc.vExtents = { 4.0f, 3.0f, 4.0f };
        Desc.vCenter = { 0.f, Desc.vExtents.y, 0.f };
    }
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_AABB",
        L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Shark::Ready_PartObject()
{
    CUI_ChatBubble::DESC ChatDesc{};
    {
        ChatDesc.pRender = &m_bChatRender;

        ChatDesc.UIDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
        ChatDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_ChatBubble.dat";
        ChatDesc.UIDesc.bInWorld = true;
        ChatDesc.UIDesc.fWorldOffsetY = 7.f;
    }
    if (FAILED(Add_PartObject(L"Part_UI_ChatBubble", LEVEL_STATIC, L"Prototype_GameObject_UI_ChatBubble", &ChatDesc)))
        return E_FAIL;

    // CineCamera
    {
        CCineCamera::DESC CineCameraDesc{};

        CineCameraDesc.CameraDesc.fFovY = XMConvertToRadians(60.f);
        CineCameraDesc.CameraDesc.fNear = 0.1f;
        CineCameraDesc.CameraDesc.fFar = 1000.f;

        CineCameraDesc.strName = L"Shark_Camera";
        CineCameraDesc.pOwner = this;

        if (FAILED(Add_PartObject(L"Part_CineCamera", LEVEL_STATIC, L"Prototype_GameObject_CineCamera", &CineCameraDesc)))
            return E_FAIL;

        m_pCineCamera = static_cast<CCineCamera*>(Find_PartObject({ L"Part_CineCamera" }));
        m_pCineCamera->Add_Cinematic(L"Beach_Start", "../Bin/DataFiles/CameraData/Beach.data");
        m_pCineCamera->Add_Cinematic(L"Beach_Shell", "../Bin/DataFiles/CameraData/Beach_Shell.data");
        m_pCineCamera->Add_Cinematic(L"Beach_Talk", "../Bin/DataFiles/CameraData/Beach_Talk.data");
        m_pCineCamera->Add_Cinematic(L"Beach_Kril", "../Bin/DataFiles/CameraData/Beach_Kril.data");
        m_pCineCamera->Add_Cinematic(L"Beach_End", "../Bin/DataFiles/CameraData/Beach_End.data");
    }

    CNPC_Shark_Shell::DESC ShellDesc{};
    {
        ShellDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
        ShellDesc.pSocketMatrix = m_pSocketMatrix;
        ShellDesc.szFilePath = "../Bin/DataFiles/PartObjectData/Shark_Test.data";
        ShellDesc.pModel = m_pModelCom;
        ShellDesc.pOwner = this;
    }
    if (FAILED(Add_PartObject(L"Part_Shell", LEVEL_BEACH, L"Prototype_GameObject_Shark_Shell", &ShellDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Shark::Ready_Event()
{
    if (FAILED(Add_Event(L"Appear", CNPC_Shark_Event_Appear::Create(this))))
        return E_FAIL;

    if (FAILED(Add_Event(L"Knock", CNPC_Shark_Event_Knock::Create(this))))
        return E_FAIL;

    if (FAILED(Add_Event(L"Awake", CNPC_Shark_Event_Awake::Create(this))))
        return E_FAIL;

    if (FAILED(Add_Event(L"Spit", CNPC_Shark_Event_Spit::Create(this))))
        return E_FAIL;

    if (FAILED(Add_Event(L"Bite", CNPC_Shark_Event_Bite::Create(this))))
        return E_FAIL;


    if (FAILED(Add_Event(L"Cam_Puase", CNPC_Shark_Event_Pause::Create(this))))
        return E_FAIL;

    if (FAILED(Add_Event(L"Cam_Stop", CNPC_Shark_Event_Stop::Create(this))))
        return E_FAIL;

    return S_OK;
}

CNPC_Shark* CNPC_Shark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_Shark* pInstance = new CNPC_Shark(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNPC_Shark");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CNPC_Shark::Clone(void* pArg)
{
    CNPC_Shark* pInstance = new CNPC_Shark(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CNPC_Shark");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_Shark::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}
