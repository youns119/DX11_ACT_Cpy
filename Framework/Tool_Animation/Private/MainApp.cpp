#include "pch.h"
#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Imgui_Manager.h"

CMainApp::CMainApp()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Init()
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

    if (FAILED(IMGUI->Init(m_pDevice, m_pContext)))
        return E_FAIL;

#endif

    if (FAILED(Start_Level(LEVELID::LEVEL_TOOL)))
        return E_FAIL;

    return S_OK;
}

void CMainApp::Pre_Update(_float _fTimeDelta)
{
#ifdef _DEBUG

    IMGUI->Pre_Update(_fTimeDelta);

#endif
}

void CMainApp::Update(_float _fTimeDelta)
{
    m_pGameInstance->Update_Engine(_fTimeDelta);

#ifdef _DEBUG

    IMGUI->Update(_fTimeDelta);

#endif
}

void CMainApp::Post_Update(_float _fTimeDelta)
{
#ifdef _DEBUG

    IMGUI->Post_Update(_fTimeDelta);

#endif
}

HRESULT CMainApp::Render()
{
    if (FAILED(m_pGameInstance->Render_Begin(_float4(0.f, 0.f, 1.f, 1.f))))
        return E_FAIL;

    m_pGameInstance->Bind_DefaultRenderTarget();
    m_pGameInstance->Draw();

#ifdef _DEBUG

    IMGUI->Render();

#endif

    m_pGameInstance->Render_End();

    return S_OK;
}

HRESULT CMainApp::Start_Level(LEVELID _eStartLevel)
{
    if (_eStartLevel == LEVELID::LEVEL_LOADING)
        return E_FAIL;

    if (FAILED(m_pGameInstance->Open_Level(LEVELID::LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, _eStartLevel))))
        return E_FAIL;

    return S_OK;
}

CMainApp* CMainApp::Create()
{
    CMainApp* pInstance = new CMainApp();

    if (FAILED(pInstance->Init()))
    {
        MSG_BOX("Failed to Create : CMainApp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMainApp::Free()
{
    __super::Free();

#ifdef _DEBUG

    IMGUI->DestroyInstance();

#endif

    m_pGameInstance->Release_Engine();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}