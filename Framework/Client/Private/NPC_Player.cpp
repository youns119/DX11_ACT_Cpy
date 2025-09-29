#include "pch.h"
#include "NPC_Player.h"

#include "GameInstance.h"
#include "Player.h"
#include "UI_Manager.h"
#include "NPC_Player_Event_Dialogue.h"

#include "CineCamera.h"
#include "CutScene_Manager.h"
#include "UI_Game.h"

#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"

CNPC_Player::CNPC_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CNPC_Player::CNPC_Player(const CNPC_Player& Prototype)
    : super(Prototype)
{
}

HRESULT CNPC_Player::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Player::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    pDesc->GameObjectDesc.strName = L"NPC_Player";

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

    m_pUIGame->Set_Direction(m_iLevel);
    Call_Effects();
    return S_OK;
}

void CNPC_Player::Priority_Update(_float fTimeDelta)
{
    super::Priority_Update(fTimeDelta);
}

void CNPC_Player::Update(_float fTimeDelta)
{
    if (m_pCutSceneManager->Get_CurPlaying() != L"Dance" && m_pCutSceneManager->Get_CurPlaying() != L"Dance_Fail")
        m_isRender = false;
    else
    {
        m_isRender = true;

        if (!m_isCutSceneStart)
        {
            m_isCutSceneStart = true;
            m_pGameInstance->Camera_Transition(L"Dance_Camera", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
            m_pCineCamera->Play(L"Dance", L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
            m_pCineCamera->Pause();
            m_bInputable = true;
        }
    }

    if (!m_isRender)
        return;

    if (m_isCutSceneEnd)
    {
        if (m_pUIManager->End_FadeOut())
        {
            m_pCutSceneManager->End_CutScene(L"Dance");
            m_pCineCamera->Stop();
            CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").front());
            pPlayer->Toggle_Render(true);
            m_pUIManager->FadeIn();

            m_pUIManager->Set_DialogueUI(m_iDialogueFileNum, 1);
        }
    }
    if (!m_bInputable && !m_isCutSceneEnd)
    {
        if (m_pUIGame->Get_Success())
        {
            if (m_iLevel == 5)
            {
                m_pGameInstance->Play_Sound(L"Success.mp3", (_uint)SOUND_CHANNEL::CONVERSATION1, 0.5f);
                m_pUIManager->FadeOut();
                m_isCutSceneEnd = true;
                // 끝!
                //switch (m_iRound)
                //{
                //case 0:
                //    m_iRound++;
                //    m_iLevel = 3;
                //    m_eCurDance = ANIM_RASPUTIN;
                //    m_pUIGame->Set_Direction(m_iLevel);
                //    m_bInputable = true;
                //    break;
                //case 1:
                //    m_iRound++;
                //    m_iLevel = 3;
                //    m_eCurDance = ANIM_DANCE;
                //    m_pUIGame->Set_Direction(m_iLevel);
                //    m_bInputable = true;
                //    break;
                //case 2:
                //    m_iRound++;
                //    m_iLevel = 3;
                //    m_eCurDance = ANIM_FLAIR;
                //    m_pUIGame->Set_Direction(m_iLevel);
                //    m_bInputable = true;
                //    break;
                //case 3:
                //    // 진짜 끝
                //    m_pUIManager->FadeOut();
                //    m_isCutSceneEnd = true;
                //    break;
                //}
            }
            else
            {
                m_iLevel++;
                m_pUIGame->Set_Direction(m_iLevel);
                m_bInputable = true;
                m_pGameInstance->Play_Sound(L"Perfect.wav", (_uint)SOUND_CHANNEL::CONVERSATION1, 1.0f);
                
                if (m_eCurDance == ANIM_DANCE)
                    m_eCurDance = ANIM_RASPUTIN;
                else if (m_eCurDance == ANIM_RASPUTIN)
                    m_eCurDance = ANIM_FLAIR;
                else if (m_eCurDance == ANIM_FLAIR)
                    m_eCurDance = ANIM_SEXY;
                else if (m_eCurDance == ANIM_SEXY)
                    m_eCurDance = ANIM_DANCE;
            }
        }
        else if (m_pUIGame->Get_Fail())
        {
            // 컷씬 실행 후 레벨 초기화하고 다시
            if (m_pCutSceneManager->Get_CurPlaying() != L"Dance_Fail")
            {
                m_pCutSceneManager->Begin_CutScene(L"Dance_Fail");
                //m_pGameInstance->Camera_Transition(L"Dance_Camera", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
                m_pCineCamera->Play(L"Fail", L"Dance", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
                m_pGameInstance->Set_RawChannelVolume((_uint)SOUND_CHANNEL::BGM, 0.f);
                // 이펙트 종료
                Disable_Effects();
            }
        }
    }

    if (m_bTalk)
    {
        if ((_uint)ANIM_TALK != m_pModelCom->Get_CurrAnimationIndex())
            m_pModelCom->SetUp_Animation((_uint)ANIM_TALK, true, 0.2f);
    }
    else
    {
        if ((_uint)m_eCurDance != m_pModelCom->Get_CurrAnimationIndex())
            m_pModelCom->SetUp_Animation((_uint)m_eCurDance, true, 0.2f);
    }

    m_pModelCom->Play_Animation(fTimeDelta);

    super::Update(fTimeDelta);
}

void CNPC_Player::Late_Update(_float fTimeDelta)
{
    if (!m_isRender)
        return;

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

    super::Late_Update(fTimeDelta);
}

HRESULT CNPC_Player::Render()
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

HRESULT CNPC_Player::Render_Shadow()
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

void CNPC_Player::Show_Dialogue()
{
    m_pCineCamera->Pause();

    std::function<void()> Callback = [this]()->void
        {
            m_pCutSceneManager->Begin_CutScene(L"Dance");
            m_pCineCamera->Play(L"Dance", L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
            m_pCineCamera->Pause();
            m_pUIGame->Set_Direction(m_iLevel);
            m_bInputable = true;
            m_pGameInstance->Set_RawChannelVolume((_uint)SOUND_CHANNEL::BGM, 1.f);

            // 이펙트 소환
            Call_Effects();
        };

    m_pUIManager->Set_DialogueUI(m_iDialogueFileNum, 0, Callback);
}

void CNPC_Player::Call_Effects()
{
    if (false == m_Effects.empty())
        Disable_Effects();

    CEffectObject* pEffect = nullptr;
    _vector vZeroLook = { XMVectorSet(0.f, 0.f, 1.f, 0.f) }, vPos{}, vLook{}, vZeroUp{ XMVectorSet(0.f, 1.f, 0.f, 0.f) };
    vPos = XMVectorSet(16.58f, 5.5f, 24.72f, 1.f);
    vLook = { XMVectorSet(1.f, 0.f, 0.f, 0.f) };
    pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_FIRECRACKER, vPos, vZeroLook, false, nullptr);
    if (nullptr != pEffect)
    {
        static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vZeroUp + vLook * 0.5f);
        m_Effects.push_back(pEffect);
    }
    pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_FIRECRACKER, vPos, vZeroLook, false, nullptr);
    if (nullptr != pEffect)
    {
        static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vZeroUp - vLook * 0.5f);
        m_Effects.push_back(pEffect);
    }

    vPos = XMVectorSet(16.76f, 5.5f, 7.89f, 1.f); //16.76, 5.74, 7.89
    pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_FIRECRACKER, vPos, vZeroLook, false, nullptr);
    if (nullptr != pEffect)
    {
        static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vZeroUp + vLook * 0.5f);
        m_Effects.push_back(pEffect);
    }
    pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_FIRECRACKER, vPos, vZeroLook, false, nullptr);
    if (nullptr != pEffect)
    {
        static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vZeroUp - vLook * 0.5f);
        m_Effects.push_back(pEffect);
    }

    vPos = XMVectorSet(6.47f, 5.5f, 8.05f, 1.f); //6.47, 5.74, 8.05
    pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_FIRECRACKER, vPos, vZeroLook, false, nullptr);
    if (nullptr != pEffect)
    {
        static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vZeroUp - vLook * 0.5f);
        m_Effects.push_back(pEffect);
    }
    pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_FIRECRACKER, vPos, vZeroLook, false, nullptr);
    if (nullptr != pEffect)
    {
        static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vZeroUp + vLook * 0.5f);
        m_Effects.push_back(pEffect);
    }

}

void CNPC_Player::Disable_Effects()
{
    if (true == m_Effects.empty())
        return;

    for (auto& Effect : m_Effects)
    {
        Effect->Clear_Info(0.f);
        Effect = nullptr;
    }
    m_Effects.clear();
}

HRESULT CNPC_Player::Ready_Event()
{
    if (FAILED(Add_Event(L"Dialogue", CNPC_Player_Event_Dialogue::Create(this))))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Player::Ready_Components()
{
    /* Com_Model */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_NPC_Player",
        L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
        L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Player::Ready_PartObject()
{
    // CineCamera
    {
        CCineCamera::DESC CineCameraDesc{};

        CineCameraDesc.CameraDesc.fFovY = XMConvertToRadians(60.f);
        CineCameraDesc.CameraDesc.fNear = 0.1f;
        CineCameraDesc.CameraDesc.fFar = 1000.f;

        CineCameraDesc.strName = L"Dance_Camera";
        CineCameraDesc.pOwner = this;

        if (FAILED(Add_PartObject(L"Part_CineCamera", LEVEL_STATIC, L"Prototype_GameObject_CineCamera", &CineCameraDesc)))
            return E_FAIL;

        m_pCineCamera = static_cast<CCineCamera*>(Find_PartObject({ L"Part_CineCamera" }));

        m_pCineCamera->Add_Cinematic(L"Dance", "../Bin/DataFiles/CameraData/Club.data");
        m_pCineCamera->Add_Cinematic(L"Fail", "../Bin/DataFiles/CameraData/Club_Fail.data");
    }

    CUI_Game::DESC UIDesc{};
    {
        UIDesc.pInputable = &m_bInputable;
    }
    if (FAILED(Add_PartObject(L"Part_UI_Game", LEVEL_STATIC, L"Prototype_GameObject_UI_Game", &UIDesc)))
        return E_FAIL;

    m_pUIGame = static_cast<CUI_Game*>(Find_PartObject({ L"Part_UI_Game" }));

    return S_OK;
}

HRESULT CNPC_Player::Load_Data(DESC& Desc)
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

CNPC_Player* CNPC_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_Player* pInstance = new CNPC_Player(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNPC_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CNPC_Player::Clone(void* pArg)
{
    CNPC_Player* pInstance = new CNPC_Player(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CNPC_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_Player::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
