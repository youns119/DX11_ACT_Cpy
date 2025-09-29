#include "pch.h"
#include "Level_Loading.h"

#include "GameInstance.h"
#include "Loader.h"
#include "Level_Tool.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CLevel{ _pDevice, _pContext }
{
}

HRESULT CLevel_Loading::Init(LEVELID _eNextLevelID)
{
    m_eNextLevelID = _eNextLevelID;

    m_pLoader = CLoader::Create(m_pDevice, m_pContext, _eNextLevelID);
    if (nullptr == m_pLoader)
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(L"Layer_Background")))
        return E_FAIL;

    return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
    m_fCurrentProgress = m_pLoader->Get_CurrnetProgress();

    if (m_pLoader->GetFinished())
    {
        switch (m_eNextLevelID)
        {
        case LEVELID::LEVEL_TOOL:
            m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Tool::Create(m_pDevice, m_pContext));
            break;
        default:
            break;
        }
    }
}

HRESULT CLevel_Loading::Render()
{
    m_pLoader->SetUp_WindowText();

    return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
    return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVELID _eNextLevelID)
{
    CLevel_Loading* pInstance = new CLevel_Loading{ _pDevice, _pContext };

    if (FAILED(pInstance->Init(_eNextLevelID))) // 로딩은 initalize 해도 상관없음
    {
        MSG_BOX("Failed to Create : CLevel_Loading");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Loading::Free()
{
    CLevel::Free();

    Safe_Release(m_pLoader);
}