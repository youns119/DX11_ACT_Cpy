#include "pch.h"
#include "../Public/MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "Imgui_Manager.h"
#include "UI_Manager.h"
#include "CutScene_Manager.h"
#include "NPC_Manager.h"

#include "UI_Headers.h"
#include "Debug_ObjectInfo.h"
#include "Debug_Character.h"
#include "Debug_Level.h"
#include "Debug_PostProcess.h"

#include "FileData_Locator.h"
#include "FData.h"

#include "Effect_Sprite_Loop.h"

#include "CallbackUpdate.h"

#include "PlayerManager.h"

CMainApp::CMainApp()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
    ENGINE_DESC EngineDesc {};

    EngineDesc.hInstance = g_hInst;
    EngineDesc.hWnd = g_hWnd;
    EngineDesc.iNumLevels = (_uint)LEVELID::LEVEL_END;
    EngineDesc.isWindowed = TRUE;
    EngineDesc.iViewportWidth = g_iWindowSizeX;
    EngineDesc.iViewportHeight = g_iWindowSizeY;

    if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
        return E_FAIL;

#ifdef _DEBUG
    if (FAILED(CImgui_Manager::Initialize(m_pDevice, m_pContext)))
        return E_FAIL;
#endif

    if (FAILED(Ready_Prototype_For_Static()))
        return E_FAIL;

    if (FAILED(Ready_GameObject_For_Static()))
        return E_FAIL;

    if (FAILED(Ready_Font()))
        return E_FAIL;

    if (FAILED(Ready_Sound()))
        return E_FAIL;

    if (FAILED(Start_Level(LEVELID::LEVEL_LOGO)))
        return E_FAIL;
    
    SetCursor(NULL);

    return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
#ifdef _DEBUG
    CImgui_Manager::Update();
#ifdef PROFILING
    CImgui_Manager::Update_Console();
#endif
#endif

    m_pGameInstance->Update_Engine(fTimeDelta);

    m_fTimerAcc += fTimeDelta;
#ifdef _DEBUG
#endif
}

HRESULT CMainApp::Render()
{
    if (FAILED(m_pGameInstance->Render_Begin(_float4(0.f, 0.f, 1.f, 1.f))))
        return E_FAIL;

    m_pGameInstance->Bind_DefaultRenderTarget();  // temp. for imgui docking 
    m_pGameInstance->Draw();


    ++m_iDrawCount;

    if (1.f <= m_fTimerAcc)
    {
        wsprintf(m_szFPS, TEXT("fps : %d"), m_iDrawCount);
        m_fTimerAcc = 0.f;
        m_iDrawCount = 0;
    }
#ifdef _DEBUG
    //m_pGameInstance->Render_Font(TEXT("Font_Cafe_20"), m_szFPS, _float2(1150.f, 0.f));

    //POINT ptMouse;
    //GetCursorPos(&ptMouse);
    //ScreenToClient(g_hWnd, &ptMouse);

    //_wstring strMousePos = to_wstring((_int)ptMouse.x) + L", " + to_wstring((_int)ptMouse.y);
    //m_pGameInstance->Render_Font(L"Font_Cafe_20", strMousePos.c_str(), {0.f, 30.f});

    CImgui_Manager::Render(m_pDevice, m_pContext);
#else
    SetWindowText(g_hWnd, m_szFPS);
#endif

    m_pGameInstance->Render_End();

    return S_OK;
}

HRESULT CMainApp::Start_Level(LEVELID eStartLevel)
{
    if (LEVELID::LEVEL_LOADING == eStartLevel)
        return E_FAIL;

    if (FAILED(m_pGameInstance->Open_Level(LEVELID::LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevel))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainApp::Ready_Prototype_For_Static()
{
#pragma region SHADER
    /* For.Prototype_Component_Shader_VtxPosTex */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    /* For.Prototype_Component_Shader_VtxPosTex_UI */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex_UI.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    /* For.Prototype_Component_Shader_VtxPoint_Particle */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPoint_Particle"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPoint_Particle.hlsl"), VTXPOINT_PARTICLE::Elements, VTXPOINT_PARTICLE::iNumElements))))
        return E_FAIL;

#pragma endregion

#pragma region MODEL
    /* For.Prototype_Component_VIBuffer_Rect */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion

#pragma region TEXTURE
    /* For.Prototype_Component_Texture_Square */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Square",
        CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Square.png", 1))))
        return E_FAIL;

    /* For.Prototype_Component_Texture_Dissolve */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Button/Dissolve.dds"), 1))))

        return E_FAIL;
    /* For.Prototype_Component_Texture_Cursor */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cursor"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Cursor/Cursor_%d.dds"), 2))))
        return E_FAIL;

    /* For.Prototype_Component_Texture_Loading */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading/Loading_%d.png"), 7))))
        return E_FAIL;

    /* For.Prototype_Component_Texture_Mask */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mask"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Mask.png"), 1))))
        return E_FAIL;

    /* For.Prototype_Component_Texture_Recycle */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Recycle"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Recycle/Recycle_%d.dds"), 10))))
        return E_FAIL;

    /* For.Prototype_Component_Texture_Wave */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wave"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Wave.png"), 1))))
        return E_FAIL;

    /* For.Prototype_Component_Texture_BubbleOne */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_Texture_BubbleOne",
        CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/FXT_bubblesMany_001.png", 1))))
        return E_FAIL;
#pragma endregion

#pragma region SHADING TEX
    /* For.Prototype_Component_Texture_EffMask */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_Texture_EffMask",
        CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Mask/Mask%02d.png", 31))))
        return E_FAIL;

    /* For.Prototype_Component_Texture_EffNoise */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_Texture_EffNoise",
        CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Noise/Noise%02d.png", 35))))
        return E_FAIL;

    /* For.Prototype_Component_Texture_EffEmissive */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_Texture_EffEmissive",
        CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Emissive/Emissive%03d.png", 133))))
        return E_FAIL;
#pragma endregion

#pragma region VIBUFFER
    /* For.Prototype_Component_VIBuffer_Particle_Point */
    CVIBuffer_InstancingParticle::DESC Desc{};
    Desc.iNumInstance = 300;
    Desc.vCenter = _float3(0.f, 2.f, 0.f);
    Desc.vRange = _float3(0.3f, 0.3f, 0.3f);
    Desc.vSize = _float2(0.02f, 0.05f);
    Desc.vSpeed = _float2(5.f, 8.f);
    Desc.vLifeTime = _float2(0.2f, 0.3f);
    Desc.isLoop = true;
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_VIBuffer_Particle_Point",
        CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, Desc))))
        return E_FAIL;
#pragma endregion

#pragma region GAMEOBJECT

    /*For. Prototype_GameObject_NPC_Manager*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_NPC_Manager"),
        CNPC_Manager::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /*For. Prototype_GameObject_CutScene_Manager*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_CutScene_Manager"),
        CCutScene_Manager::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /*For. Prototype_GameObject_UI_Manager*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Manager"),
        CUI_Manager::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /*For. Prototype_GameObject_UI_CopyTexture*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_CopyTexture"),
        CUI_CopyTexture::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /*For. Prototype_GameObject_UI_TextTexture*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_TextTexture"),
        CUI_TextTexture::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /*For. Prototype_GameObject_UI_Text*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Text"),
        CUI_Text::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /*For. Prototype_GameObject_UI_Fade*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Fade"),
        CUI_Fade::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /*For. Prototype_GameObject_UI_Mouse*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Mouse"),
        CUI_Mouse::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /*For. Prototype_GameObject_UI_Texture*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Texture"),
        CUI_Texture::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /*For. Prototype_GameObject_UI_Loading*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Loading"),
        CUI_Loading::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /*For. Prototype_GameObject_UI_Recycle*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Recycle"),
        CUI_Recycle::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /*For. Prototype_GameObject_UI_CutScene*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_CutScene"),
        CUI_CutScene::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_Effect_Sprite_Loop */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Effect_Sprite_Loop"),
        CEffect_Sprite_Loop::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_CallbackUpdate */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_CallbackUpdate"),
        CCallbackUpdate::Create(m_pDevice, m_pContext))))
        return E_FAIL;

#ifdef _DEBUG

    /* For.Prototype_GameObject_Debug_ObjectInfo */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Debug_ObjectInfo"),
        CDebug_ObjectInfo::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Debug_Chracter */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Debug_Character"),
        CDebug_Character::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Debug_Level */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Debug_Level"),
        CDebug_Level::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Debug_PostProcess */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Debug_PostProcess"),
        CDebug_PostProcess::Create(m_pDevice, m_pContext))))
        return E_FAIL;

#endif // _DEBUG

#pragma endregion

    return S_OK;
}

HRESULT CMainApp::Ready_GameObject_For_Static()
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_CallbackUpdate", LEVEL_STATIC, L"Layer_CallbackUpdate")))
        return E_FAIL;

    CNPC_Manager::DESC NPCManagerDesc{};
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Manager", LEVEL_STATIC, L"Layer_NPCManager", &NPCManagerDesc)))
        return E_FAIL;

    CCutScene_Manager::DESC CutSceneManagerDesc{};
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_CutScene_Manager", LEVEL_STATIC, L"Layer_CutSceneManager", &CutSceneManagerDesc)))
        return E_FAIL;

    CUI_Manager::DESC UIManagerDesc{};
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_UI_Manager", LEVEL_STATIC, L"Layer_UIManager", &UIManagerDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainApp::Ready_Font()
{
    if (FAILED(m_pGameInstance->Add_Font(L"Font_Cafe_10", L"../Bin/Resources/Fonts/OTFB10.spritefont")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Font(L"Font_Cafe_15", L"../Bin/Resources/Fonts/OTFB15.spritefont")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Font(L"Font_Cafe_20", L"../Bin/Resources/Fonts/OTFB20.spritefont")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Font(L"Font_Main_40", L"../Bin/Resources/Fonts/MainFont40.spritefont")))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainApp::Ready_Sound()
{
    // Sound_BGM
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/BGM/");

    // Sound_CutScene
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/CutScene/Beach/");
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/CutScene/Field/");
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/CutScene/PinBall/");
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/CutScene/CrabTomb/");

    // Sound_Player
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/Player/");

    // Sound_Enemy
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/Enemy/Bobbitworm/");
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/Enemy/Bowman/");
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/Enemy/Cuttlefish/");
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/Enemy/Inkerton/");
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/Enemy/Petroch/");
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/Enemy/Roland/");
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/Enemy/Sardine/");
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/Enemy/Splitter/");
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/Enemy/Pagurus/");
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/Enemy/Defualt/");

    // Sound_MapObject
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/MapObject/");

    // Sound_UI
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/UI/");

    // Sound_MiniGame
    m_pGameInstance->LoadSoundFile("../../Client/Bin/Resources/Sound/MiniGame/");
    return S_OK;
}

CMainApp* CMainApp::Create()
{
    CMainApp* pInstance = new CMainApp();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Create : CMainApp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMainApp::Free()
{
    /* 부모의 멤버를 정리하라고 이야기한다. */
    //__super::Free();
    super::Free();
    
    CFileData_Locator<FDialogueData>::Release_Service();
    CFileData_Locator<FItemData>::Release_Service();
    CFileData_Locator<FSkillData>::Release_Service();
    CFileData_Locator<FSheleportData>::Release_Service();
    CFileData_Locator<FShellDescData>::Release_Service();

    CPlayerManager::Release();

#ifdef _DEBUG
    CImgui_Manager::Release();
#endif
    m_pGameInstance->Release_Engine();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
