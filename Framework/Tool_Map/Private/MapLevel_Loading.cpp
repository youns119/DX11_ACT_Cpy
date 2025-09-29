#include "pch.h"
#include "MapLevel_Loading.h"

#include "GameInstance.h"
#include "MapLoader.h"
#include "Level_Logo.h"
#include "MapLevel_Tool.h"

CMapLevel_Loading::CMapLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CMapLevel_Loading::Initialize(LEVELID eNextLevelID)
{
    m_eNextLevelID = eNextLevelID;

    m_pLoader = CMapLoader::Create(m_pDevice, m_pContext, eNextLevelID);
    if (nullptr == m_pLoader)
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(L"Layer_Background")))
        return E_FAIL;

    return S_OK;
}

void CMapLevel_Loading::Update(_float fTimeDelta)
{
    if (GetKeyState(VK_SPACE) < 0)
    {
        m_fCurrentProgress = m_pLoader->Get_CurrnetProgress();
        if (m_pLoader->IsFinisihed())
        {
            //m_pLoader->Get_LevelData();
            //Open_Level 에서 로딩 레벨 release하는 시점보다 Create 하는 시점이 더 앞서있음-> 로더 살아있음 -> 데이터 넘겨도 됨
            switch (m_eNextLevelID)
            {
            case LEVELID::LEVEL_LOGO:
                m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Logo::Create(m_pDevice, m_pContext));
                break;

            case LEVELID::LEVEL_TOOL:
                m_pGameInstance->Open_Level(m_eNextLevelID, CMapLevel_Tool::Create(m_pDevice, m_pContext));
                break;

            }
        }
    }
}

HRESULT CMapLevel_Loading::Render()
{
    m_pLoader->SetUp_WindowText();
    return S_OK;
}

HRESULT CMapLevel_Loading::Ready_Layer_BackGround(const _tchar* pLayerTag)
{

    return S_OK;
}

CMapLevel_Loading* CMapLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
    CMapLevel_Loading* pInstance = new CMapLevel_Loading{ pDevice, pContext };

    if (FAILED(pInstance->Initialize(eNextLevelID))) // 로딩은 initalize 해도 상관없음
    {
        MSG_BOX("Failed to Create : CMapLevel_Loading");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapLevel_Loading::Free()
{
    CLevel::Free();

    Safe_Release(m_pLoader);
}
