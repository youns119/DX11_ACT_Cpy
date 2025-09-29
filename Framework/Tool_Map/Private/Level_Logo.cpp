#include "pch.h"
#include "Level_Logo.h"

#include "GameInstance.h"
#include "MapLevel_Loading.h"

#include "BackGround.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
    CUIObject::Resize(m_pContext);

    if (FAILED(Ready_Layer_BackGround(_T("Layer_BackGround"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
    if (GetKeyState(VK_RETURN) < 0)
    {
        m_pGameInstance->Open_Level(LEVELID::LEVEL_LOADING, CMapLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_TOOL));
    }
}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
    CBackGround::BACKGROUND_DESC		Desc{};

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_LOGO, _T("Prototype_GameObject_BackGround"), LEVELID::LEVEL_LOGO, pLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

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
