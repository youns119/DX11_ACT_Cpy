#include "pch.h"
#include "NPC_Roland_Death.h"

#include "GameInstance.h"
#include "Enemy_Splitter.h"
#include "Animation.h"
#include "Player.h"
#include "Camera_Player.h"

#include "UI_Manager.h"
#include "CutScene_Manager.h"
#include "CineCamera.h"

#include "NPC_Roland_Death_Shake.h"
#include "NPC_Roland_Death_Move.h"

CNPC_Roland_Death::CNPC_Roland_Death(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CNPC_Roland_Death::CNPC_Roland_Death(const CNPC_Roland_Death& Prototype)
    : super(Prototype)
{
}

HRESULT CNPC_Roland_Death::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Roland_Death::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    pDesc->GameObjectDesc.fSpeedPerSec = 100.f;

    if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObject()))
        return E_FAIL;

    if (FAILED(Ready_Event()))
        return E_FAIL;

    m_pModelCom->SetUp_Animation((_uint)ANIM_HIT, false, 0.2f);
    m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat3(&m_vStartPosition));

    m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    m_pCutSceneManager = static_cast<CCutScene_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CutSceneManager", 0));

    m_vPos.push_back({33.85f, 34.93f, 31.28f});
    m_vPos.push_back({16.91f, 34.65f, 30.60f});
    m_vPos.push_back({17.62f, 33.64f, 20.23f});
    m_vPos.push_back({35.24f, 34.65f, 30.12f});
    m_vPos.push_back({33.74f, 33.67f, 20.67f});
    m_vPos.push_back({17.32f, 34.75f, 31.66f});
    m_vPos.push_back({33.89f, 34.88f, 32.43f});
    m_vPos.push_back({25.26f, 35.29f, 37.35f});
    m_vPos.push_back({25.48f, 35.38f, -5.08f});


    return S_OK;
}

void CNPC_Roland_Death::Priority_Update(_float fTimeDelta)
{
    super::Priority_Update(fTimeDelta);
}

void CNPC_Roland_Death::Update(_float fTimeDelta)
{
    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").front());

    //super ¾÷µ«µµ ¾Èµ·´Ù ÀÌ·¯¸é
    //if (m_pCutSceneManager->Get_CurPlaying() != L"Roland_Death" ||
    //    !pPlayer->Get_PlayerCamera()->IsCurrCam())
    if (m_pCutSceneManager->Get_CurPlaying() != L"Roland_Death")
        m_isRender = false;
    else if (!m_isCutSceneStart && pPlayer->Get_PlayerCamera()->IsCurrCam())
    {
        m_isRender = true;

        pPlayer->Toggle_Render(false);

        m_isCutSceneStart = true;
        m_pGameInstance->Camera_Transition(L"Roland_Death_Camera", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.f);
        m_pCineCamera->Play(L"Roland", L"Pinball1", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
        //m_pCineCamera->Pause();

        CEnemy* pRoland = static_cast<CEnemy*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Monster"));
        pRoland->Death();
    }

    if (!m_isCutSceneStart)
        return;

    if (m_eCurState == STATE_DIE)
    {
        if ((_uint)ANIM_DIE != m_pModelCom->Get_CurrAnimationIndex())
            m_pModelCom->SetUp_Animation((_uint)ANIM_DIE, true, 0.2f);

        _vec3 vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
        _vec3 vGoal = XMLoadFloat3(&m_vPos[m_iIndex]);
        
        _float fDistance = (vGoal - vPosition).Length();
        
        if (fDistance < 1.f)
        {
            Call_SparkEffect();

            if (m_iIndex == 0)
            {
                m_pCineCamera->Stop();
                m_pCineCamera->Play(L"Pinball2", L"Pinball3", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
                m_pCineCamera->Pause();
            }
            else if(m_iIndex == 1)
            {
                m_pCineCamera->Stop();
                m_pCineCamera->Play(L"Pinball3", L"Pinball4", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
                m_pCineCamera->Pause();
            }
            m_iIndex++;
            if (m_iIndex == m_vPos.size())
            {
                m_eCurState = STATE_DOWN;
                m_pCineCamera->Stop();
                m_pCineCamera->Play(L"Pinball4", L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
                m_pCineCamera->Pause();
            }
        }
        else
        {
            m_pTransformCom->LookAt(vGoal._vector());
            m_pTransformCom->Go_Straight(fTimeDelta);
        }

    }
    else if (m_eCurState == STATE_DOWN)
    {
        if (m_fFrame < 1.f)
            m_fFrame += fTimeDelta;
        else
        {
            if (false == m_bFallEffectCalled)
            {
                m_bFallEffectCalled = true;
                Call_FallEffect();
                m_pGameInstance->Play_Sound(L"Roland_Yell.wav", (_uint)SOUND_CHANNEL::CONVERSATION1, 1.0f);
            }
            _float fPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::POSITION));

            if (fPosY >= 10.f)
            {
                m_pTransformCom->Go_Down(fTimeDelta);
                m_pGameInstance->Set_ChannelVolume_With_SpatialDesc((_uint)SOUND_CHANNEL::CONVERSATION1, m_pTransformCom->Get_State(CTransform::STATE::POSITION), _vec2(1.f, 50.f), _vec2(0.5f, 1.0f));
            }
            else
            {
                if (!m_isCutSceneEnd)
                {
                    m_isCutSceneEnd = true;
                    m_pCutSceneManager->End_CutScene(L"Roland_Death");
                    m_pCineCamera->Stop();
                    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").front());
                    pPlayer->Toggle_Render(true);
                }
            }
        }
    }

    if(!m_bAnimStop)
        m_pModelCom->Play_Animation(fTimeDelta);

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

    super::Update(fTimeDelta);
}

void CNPC_Roland_Death::Late_Update(_float fTimeDelta)
{
    if (m_isRender)
    {
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

#ifdef _DEBUG
        m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif
    }

    super::Late_Update(fTimeDelta);
}

HRESULT CNPC_Roland_Death::Render()
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

HRESULT CNPC_Roland_Death::Render_Shadow()
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

void CNPC_Roland_Death::Shake()
{
    m_bAnimStop = true;
    m_pCineCamera->PositionShake(0.1f, 20.f, 3.f);
    m_pCineCamera->RotationShake(0.1f, 20.f, 3.f);
    m_pCineCamera->Resume();
}

void CNPC_Roland_Death::Move()
{
    m_bAnimStop = false;
    m_eCurState = STATE_DIE;
    m_pCineCamera->Stop();
    m_pCineCamera->Play(L"Pinball1", L"Pinball2", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
    m_pCineCamera->Pause();
}

void CNPC_Roland_Death::Call_SparkEffect()
{
    if (m_iIndex < m_vPos.size() - 1)
    {
        _vec3 vPos{}, vLook{}, vGoal{}, vEffectPos{}, vEffectLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
        _float fDistance{};
        vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
        vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK);
        vGoal = XMLoadFloat3(&m_vPos[m_iIndex]);
        fDistance = (vGoal - vPos).Length();

        vEffectPos = vPos + vLook * fDistance * 0.75f;
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ROLAND_GOLFHIT_PTCL_LIGHT, vEffectPos._vector(), vEffectLook._vector(), false, nullptr);
    }
}

void CNPC_Roland_Death::Call_FallEffect()
{
    _vec3 vPos{}, vEffectLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
    vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPSANDBURST, vPos._vector(), vEffectLook._vector(), false, nullptr);
}

HRESULT CNPC_Roland_Death::Ready_Components()
{
    /* Com_Model */
    CModel::DESC ModelDesc{};
    ModelDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_NPC_Roland_Death",
        L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &ModelDesc)))
        return E_FAIL;


    /* Com_Shader */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
        L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CCollider_Sphere::DESC Desc{};
    Desc.ColliderDesc.pOwner = this;
    Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::NPC;

    Desc.fRadius = { 2.5f };
    Desc.vCenter = { 0.0f, 0.f, 0.0f };

    /* Com_Collider */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_Sphere",
        L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Roland_Death::Ready_Event()
{
    if (FAILED(Add_Event(L"Shake", CNPC_Roland_Death_Shake::Create(this))))
        return E_FAIL;

    if (FAILED(Add_Event(L"Move", CNPC_Roland_Death_Move::Create(this))))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Roland_Death::Ready_PartObject()
{
    // CineCamera
    {
        CCineCamera::DESC CineCameraDesc{};

        CineCameraDesc.CameraDesc.fFovY = XMConvertToRadians(60.f);
        CineCameraDesc.CameraDesc.fNear = 0.1f;
        CineCameraDesc.CameraDesc.fFar = 1000.f;

        CineCameraDesc.strName = L"Roland_Death_Camera";
        CineCameraDesc.pOwner = this;

        if (FAILED(Add_PartObject(L"Part_CineCamera", LEVEL_STATIC, L"Prototype_GameObject_CineCamera", &CineCameraDesc)))
            return E_FAIL;

        m_pCineCamera = static_cast<CCineCamera*>(Find_PartObject({ L"Part_CineCamera" }));
        m_pCineCamera->Add_Cinematic(L"Roland", "../Bin/DataFiles/CameraData/Roland_Death.data");
        m_pCineCamera->Add_Cinematic(L"Pinball1", "../Bin/DataFiles/CameraData/Roland_Death_1.data");
        m_pCineCamera->Add_Cinematic(L"Pinball2", "../Bin/DataFiles/CameraData/Roland_Death_2.data");
        m_pCineCamera->Add_Cinematic(L"Pinball3", "../Bin/DataFiles/CameraData/Roland_Death_3.data");
        m_pCineCamera->Add_Cinematic(L"Pinball4", "../Bin/DataFiles/CameraData/Roland_Death_4.data");
    }

    return S_OK;
}

CNPC_Roland_Death* CNPC_Roland_Death::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_Roland_Death* pInstance = new CNPC_Roland_Death(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNPC_Roland_Death");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CNPC_Roland_Death::Clone(void* pArg)
{
    CNPC_Roland_Death* pInstance = new CNPC_Roland_Death(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CNPC_Roland_Death");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_Roland_Death::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);

}
