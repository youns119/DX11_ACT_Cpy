#include "pch.h"
#include "Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "UI_Manager.h"
#include "UI_Texture.h"
#include "UI_MainMenu.h"

#include "Remains.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
    m_pGameInstance->Play_Sound_Loop(L"Water_Travel_Loop_1.wav", (_uint)SOUND_CHANNEL::AMBIENCE_LOOP, 0.f);
    m_pGameInstance->Play_Sound_Loop(L"Logo.wav", (_uint)SOUND_CHANNEL::BGM, 0.2f);

    if (FAILED(Ready_Layer_UI(_T("Layer_UI"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Remains(_T("Layer_Remains"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Managers()))
        return E_FAIL;

#ifdef _DEBUG
    if (FAILED(Ready_Layer_Debug(_T("Layer_Debug"))))
        return E_FAIL;
#endif // _DEBUG

    CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    pUIManager->FadeIn();
    pUIManager->Show_Mouse();

    return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(KEY::ENTER))
    {
        m_pGameInstance->Open_Level(LEVELID::LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_BEACH));
    }

    if (m_pGameInstance->Key_Down(KEY::NUM_1))
    {
        m_pGameInstance->Open_Level(LEVELID::LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_VILLAGE));
    }

    if (m_pGameInstance->Key_Down(KEY::NUM_2))
    {
        m_pGameInstance->Open_Level(LEVELID::LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_FIELD));
    }

    if (m_pGameInstance->Key_Down(KEY::NUM_3))
    {
        m_pGameInstance->Open_Level(LEVELID::LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_PINBALL));
    }

    if (m_pGameInstance->Key_Down(KEY::NUM_4))
    {
        m_pGameInstance->Open_Level(LEVELID::LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_CLUB));
    }

    if (m_pGameInstance->Key_Down(KEY::NUM_5))
    {
        m_pGameInstance->Open_Level(LEVELID::LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_CRABTOMB));
    }

    if (m_pGameInstance->Key_Down(KEY::NUM_6))
    {
        m_pGameInstance->Open_Level(LEVELID::LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_TUTORIAL));
    }
}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(const _tchar* pLayerTag)
{
    CUI_Texture::DESC TextureDesc{};
    {
        TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Background.dat";
        TextureDesc.UIDesc.eLevelID = LEVEL_LOGO;
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Texture"), LEVELID::LEVEL_LOGO, pLayerTag, &TextureDesc)))
        return E_FAIL;

    CUI_MainMenu::DESC      MainMenuDesc{};

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_LOGO, TEXT("Prototype_GameObject_UI_MainMenu"), LEVELID::LEVEL_LOGO, pLayerTag, &MainMenuDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Remains(const _tchar* pLayerTag)
{
    CRemains::DESC RemainsDesc{};

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Remains"), LEVELID::LEVEL_STATIC, pLayerTag, &RemainsDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Managers()
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_ThemeManager", LEVEL_STATIC, L"Layer_ThemeManager")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_VisitManager", LEVEL_STATIC, L"Layer_VisitManager")))
        return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG
HRESULT CLevel_Logo::Ready_Layer_Debug(const _tchar* pLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_Level", LEVEL_LOGO, pLayerTag)))
        return E_FAIL;

    return S_OK;
}
#endif // _DEBUG

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Logo* pInstance = new CLevel_Logo{ pDevice, pContext };

    //if (FAILED(pInstance->Initialize())) // 이제 여기서 level initialize 하면 안됨
    //{
    //    MSG_BOX("Failed to Create : CLevel_Logo");
    //    Safe_Release(pInstance);
    //}

    return pInstance;
}

void CLevel_Logo::Free()
{
    CLevel::Free();
}
