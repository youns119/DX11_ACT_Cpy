#include "pch.h"
#include "MainTool.h"

#include "GameInstance.h"
#include "MapLevel_Loading.h"
#include "Imgui_Manager.h"

CMainTool::CMainTool()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CMainTool::Initialize()
{
    ENGINE_DESC EngineDesc{};

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

    if (FAILED(Gara_Function()))
        return E_FAIL;

    if (FAILED(Start_Level(LEVELID::LEVEL_LOGO)))
        return E_FAIL;

    return S_OK;
}

void CMainTool::Update(_float fTimeDelta)
{
    PROFILE_SCOPE_TAG; //이 함수 안 시간 체크를 해주는 함수

#ifdef _DEBUG
    CImgui_Manager::Update();
#endif

    m_pGameInstance->Update_Engine(fTimeDelta);

#ifdef _DEBUG
    m_fTimerAcc += fTimeDelta;
#endif
}

HRESULT CMainTool::Render()
{
    //PROFILE_SCOPE(L"Render");  
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
    //m_pGameInstance->Render_Font(L"Font_IBM3270_20", strMousePos.c_str(), {0.f, 30.f});

    CImgui_Manager::Render(m_pDevice, m_pContext);
#else
    SetWindowText(g_hWnd, m_szFPS);
#endif

    m_pGameInstance->Render_End();

    return S_OK;
}

HRESULT CMainTool::Start_Level(LEVELID eStartLevel)
{
    if (LEVELID::LEVEL_LOADING == eStartLevel)
        return E_FAIL;

    //CLevel_Loading 넣어주기
    if (FAILED(m_pGameInstance->Open_Level(LEVELID::LEVEL_LOADING, CMapLevel_Loading::Create(m_pDevice, m_pContext, eStartLevel))))
        return E_FAIL;


    return S_OK;
}

HRESULT CMainTool::Ready_Prototype_For_Static()
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

    /* For.Prototype_Component_Texture_Logo */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Logo"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/LogoTextV1.png"), 1))))
        return E_FAIL;
#pragma endregion

#pragma region GAMEOBJECT
#pragma endregion

    if (FAILED(m_pGameInstance->Add_Font(L"Font_Cafe_10", L"../Bin/Resources/Fonts/OTFB10.spritefont")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Font(L"Font_Cafe_15", L"../Bin/Resources/Fonts/OTFB15.spritefont")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Font(L"Font_Cafe_20", L"../Bin/Resources/Fonts/OTFB20.spritefont")))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_Font(L"Font_Main_40", L"../Bin/Resources/Fonts/MainFont40.spritefont")))
        return E_FAIL;

    return S_OK;
    return S_OK;

}

HRESULT CMainTool::Gara_Function()
{
    return S_OK;
}

CMainTool* CMainTool::Create()
{
    CMainTool* pInstance = new CMainTool();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Create : CMainTool");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMainTool::Free()
{
    /* 부모의 멤버를 정리하라고 이야기한다. */
    super::Free();

#ifdef _DEBUG
    CImgui_Manager::Release();
#endif

    m_pGameInstance->Release_Engine();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
