#include "pch.h"
#include "NPC_Roland.h"

#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"

#include "UI_Manager.h"
#include "CutScene_Manager.h"

#include "CineCamera.h"

#include "NPC_Roland_Stop.h"

CNPC_Roland::CNPC_Roland(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CNPC_Roland::CNPC_Roland(const CNPC_Roland& Prototype)
    : super(Prototype)
{
}

HRESULT CNPC_Roland::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Roland::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    pDesc->GameObjectDesc.fSpeedPerSec = 0.1f;

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

    m_pModelCom->SetUp_Animation((_uint)ANIM_TALK, true, 0.2f);

    m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    
    m_pCutSceneManager = static_cast<CCutScene_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CutSceneManager", 0));
    m_pCutSceneManager->Begin_CutScene(L"PinBall_Start");
    m_pGameInstance->Camera_Transition(L"Roland_Camera", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
    m_pCineCamera->Play(L"PinBall_Start", L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
    m_pGameInstance->Play_Sound_Loop(L"Roland_FirstCutSceneMusic.wav", (_uint)SOUND_CHANNEL::BGM, 0.7f);

    m_vecSoundKeys.push_back(L"PinBall_Roland_00.wav");
    m_vecSoundKeys.push_back(L"PinBall_Roland_01.wav");
    m_vecSoundKeys.push_back(L"PinBall_Roland_02.wav");
    m_vecSoundKeys.push_back(L"PinBall_Roland_03.wav");
    m_vecSoundKeys.push_back(L"PinBall_Roland_04.wav");
    m_vecSoundKeys.push_back(L"PinBall_Roland_05.wav");
    m_vecSoundKeys.push_back(L"PinBall_Inkerton_00.wav");
    //m_vecSoundKeys.push_back(L"PinBall_Roland_06.wav");
    //m_vecSoundKeys.push_back(L"PinBall_Roland_07.wav");

    m_vecCaptions.push_back(L"롤랜드: 자, 우리 문명인에게는 규칙이 있습니다");
    m_vecCaptions.push_back(L"롤랜드: 얼룩덜룩하고 하찮은 당신네 무리가 마음에 들지 않긴 하지만, 정당한 이유도 없이 해치우는 건 신사답지 못하겠죠.");
    m_vecCaptions.push_back(L"롤랜드: 하지만 여러분은 다들 방금 제 사유지에 무단 침입했습니다");
    m_vecCaptions.push_back(L"롤랜드: 그리고 제가 사는 곳에서는, 이는 무단 침입이라 불리는 범죄입니다");
    m_vecCaptions.push_back(L"롤랜드: 무단 침입자에게 어떤 조치를 취하는진 알고 계시리라 생각합니다");
    m_vecCaptions.push_back(L"롤랜드: 잉커톤? 저녀석을 처치해");
    m_vecCaptions.push_back(L"잉커톤: 기꺼이");
    //m_vecCaptions.push_back(L"롤랜드: 그리고 당신은...");
    //m_vecCaptions.push_back(L"롤랜드: 내가 직접 죽여주마, 이 납작벌레 자식!");

    return S_OK;
}

void CNPC_Roland::Priority_Update(_float fTimeDelta)
{
    super::Priority_Update(fTimeDelta);
}

void CNPC_Roland::Update(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    if (m_pGameInstance->Key_Down(KEY::EQUALS))
    {
        //m_pGameInstance->Stop_Sound_All();
        //m_pCutSceneManager->Begin_CutScene(L"PinBall_Start");
        //m_pGameInstance->Play_Sound_Loop(L"Roland_FirstCutSceneMusic.wav", (_uint)SOUND_CHANNEL::BGM, 0.7f);
        //m_iCurPlaySoundIndex = 0;

        m_pCutSceneManager->End_CutScene(L"PinBall_End");
        m_pGameInstance->FadeSwap_Sound(L"PinBall_BGM.wav", (_uint)SOUND_CHANNEL::BGM, 1.f, 0.5f);

        CBoss_Roland* pRoland = static_cast<CBoss_Roland*>(m_pGameInstance->Find_GameObject_With_Name(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Monster"), TEXT("Roland")));
        CTransform* pTransform = static_cast<CTransform*>(pRoland->Find_Component(g_strTransformTag));


        pTransform->Set_State(CTransform::STATE::LOOK, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::LOOK)));
        pTransform->Set_State(CTransform::STATE::UP, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::UP)));
        pTransform->Set_State(CTransform::STATE::RIGHT, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::RIGHT)));
        pRoland->Locate(m_pTransformCom->Get_State(CTransform::STATE::POSITION));

        m_isActive = false;
        m_pCineCamera->Stop();
    }

    if(!m_bAnimStop)
        m_pModelCom->Play_Animation(fTimeDelta);

    if (m_iCurPlaySoundIndex < 5)
    {
        if (m_pModelCom->Get_CurrAnimationIndex() != (_uint)ANIM_TALK)
            m_pModelCom->SetUp_Animation((_uint)ANIM_TALK, true, 0.2f);
    }
    else if (m_iCurPlaySoundIndex == 6)
    {
        if (m_pModelCom->Get_CurrAnimationIndex() != (_uint)ANIM_DAMN)
            m_pModelCom->SetUp_Animation((_uint)ANIM_DAMN, false, 0.2f);
    }

    if (!m_pGameInstance->IsPlaying((_uint)SOUND_CHANNEL::CONVERSATION1))
    {

        // 그래들리가 끝나고!
        if (m_iCurPlaySoundIndex == 7 && !m_pCutSceneManager->Is_CutSceneFinished(L"PinBall_Fight"))
        {
            m_pCutSceneManager->Begin_CutScene(L"PinBall_Fight");
            if (!m_bInkertonGo)
            {
                m_pGameInstance->Play_Sound(L"Inkerton_Go.wav", (_uint)SOUND_CHANNEL::NPC, 0.7f);
                m_bInkertonGo = true;
            }
        }

        if(m_iCurPlaySoundIndex < m_vecSoundKeys.size() && m_pCutSceneManager->Get_CurPlaying() != L"PinBall_Fight")
        {
            m_pGameInstance->Play_Sound(m_vecSoundKeys[m_iCurPlaySoundIndex], (_uint)SOUND_CHANNEL::CONVERSATION1, 1.f);
            m_pUIManager->Set_CutSceneUI_Caption(m_vecCaptions[m_iCurPlaySoundIndex].c_str());

            m_iCurPlaySoundIndex++;
        }
    }
    

    if (m_pCutSceneManager->Is_CutSceneFinished(L"PinBall_Fight"))
    {
        m_pCutSceneManager->End_CutScene(L"PinBall_End");
        m_pGameInstance->FadeSwap_Sound(L"PinBall_BGM.wav", (_uint)SOUND_CHANNEL::BGM, 1.f, 0.5f);

        CBoss_Roland* pRoland = static_cast<CBoss_Roland*>(m_pGameInstance->Find_GameObject_With_Name(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Monster"), TEXT("Roland")));
        CTransform* pTransform = static_cast<CTransform*>(pRoland->Find_Component(g_strTransformTag));

        pTransform->Set_State(CTransform::STATE::LOOK, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::LOOK)));
        pTransform->Set_State(CTransform::STATE::UP, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::UP)));
        pTransform->Set_State(CTransform::STATE::RIGHT, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::RIGHT)));
        pRoland->Locate(m_pTransformCom->Get_State(CTransform::STATE::POSITION));

        m_isActive = false;
        m_pCineCamera->Stop();
    }

    super::Update(fTimeDelta);
}

void CNPC_Roland::Late_Update(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

    super::Late_Update(fTimeDelta);
}

HRESULT CNPC_Roland::Render()
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

HRESULT CNPC_Roland::Render_Shadow()
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

void CNPC_Roland::Stop()
{
    m_bAnimStop = true;
}

HRESULT CNPC_Roland::Load_Data(DESC& Desc)
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

HRESULT CNPC_Roland::Ready_Components()
{
    /* Com_Model */
    CModel::DESC ModelDesc{};
    ModelDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_NPC_Roland",
        L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &ModelDesc)))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
        L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Roland::Ready_PartObject()
{
    // CineCamera
    {
        CCineCamera::DESC CineCameraDesc{};

        CineCameraDesc.CameraDesc.fFovY = XMConvertToRadians(60.f);
        CineCameraDesc.CameraDesc.fNear = 0.1f;
        CineCameraDesc.CameraDesc.fFar = 1000.f;

        CineCameraDesc.strName = L"Roland_Camera";
        CineCameraDesc.pOwner = this;

        if (FAILED(Add_PartObject(L"Part_CineCamera", LEVEL_STATIC, L"Prototype_GameObject_CineCamera", &CineCameraDesc)))
            return E_FAIL;

        m_pCineCamera = static_cast<CCineCamera*>(Find_PartObject({ L"Part_CineCamera" }));
        m_pCineCamera->Add_Cinematic(L"PinBall_Start", "../Bin/DataFiles/CameraData/PinBall.data");
    }

    return S_OK;
}

HRESULT CNPC_Roland::Ready_Event()
{
    if (FAILED(Add_Event(L"Stop", CNPC_Roland_Stop::Create(this))))
        return E_FAIL;

    return S_OK;
}

CNPC_Roland* CNPC_Roland::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_Roland* pInstance = new CNPC_Roland(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNPC_Roland");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CNPC_Roland::Clone(void* pArg)
{
    CNPC_Roland* pInstance = new CNPC_Roland(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CNPC_Roland");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_Roland::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
