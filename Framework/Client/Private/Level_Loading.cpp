#include "pch.h"
#include "Level_Loading.h"

#include "GameInstance.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_Village.h"
#include "Level_Field.h"
#include "Level_PinBall.h"
#include "Level_CrabTomb.h"
#include "Level_Beach.h"
#include "Level_Club.h"
#include "Level_Tutorial.h"

#include "UI_Manager.h"
#include "UI_Texture.h"
#include "UI_Recycle.h"
#include "UI_Loading.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID)
{
    CUIObject::Resize(m_pContext);

    m_eNextLevelID = eNextLevelID;

    m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
    if (nullptr == m_pLoader)
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(L"Layer_Background")))
        return E_FAIL;

    if (FAILED(Ready_Effect()))
        return E_FAIL;

    m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    m_pUIManager->Add_PartObject_For_Static();
    m_pUIManager->FadeIn();

    m_pUIManager->Hide_Mouse();

    return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
    m_fCurrentProgress = m_pLoader->Get_CurrnetProgress();

    if (m_pLoader->IsFinisihed())
    {
        m_pUIManager->FadeOut();
        //m_pLoader->Get_LevelData();
        //Open_Level 에서 로딩 레벨 release하는 시점보다 Create 하는 시점이 더 앞서있음-> 로더 살아있음 -> 데이터 넘겨도 됨

        if (m_pUIManager->End_FadeOut())
        {
            switch (m_eNextLevelID)
            {
            case LEVELID::LEVEL_LOGO:
                m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Logo::Create(m_pDevice, m_pContext));
                break;
    
            case LEVELID::LEVEL_VILLAGE:
                m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Village::Create(m_pDevice, m_pContext));
                break;
    
            case LEVELID::LEVEL_FIELD:
                m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Field::Create(m_pDevice, m_pContext));
                break;

            case LEVELID::LEVEL_PINBALL:
                m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_PinBall::Create(m_pDevice, m_pContext));
                break;

            case LEVELID::LEVEL_CRABTOMB:
                m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_CrabTomb::Create(m_pDevice, m_pContext));
                break;

            case LEVELID::LEVEL_BEACH:
                m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Beach::Create(m_pDevice, m_pContext));
                break;

            case LEVELID::LEVEL_CLUB:
                m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Club::Create(m_pDevice, m_pContext));
                break;

            case LEVELID::LEVEL_TUTORIAL:
                m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Tutorial::Create(m_pDevice, m_pContext));
                break;
            }
        }
    }

    m_fFrame += fTimeDelta;

    if (m_fFrame >= 0.5f)
    {
        m_fFrame = 0.f;
        ++m_iCurIndex %= 4;
        lstrcpy(m_szText, m_strLoadingText[m_iCurIndex].c_str());
    }
}

HRESULT CLevel_Loading::Render()
{
#ifdef _DEBUG
    m_pLoader->SetUp_WindowText();
#endif

    m_pGameInstance->Render_Font(L"Font_Main_40", m_szText, { 50.f, 650.f }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 0.8f);

    return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
    /* Texture */
    /*CUI_Texture::DESC TextureDesc{};
    {
        TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Black.dat";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", LEVEL_LOADING, pLayerTag, &TextureDesc)))
        return E_FAIL;

    CUI_Recycle::DESC RecycleDesc{};
    {
        RecycleDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Recycle.dat";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_UI_Recycle", LEVEL_LOADING, pLayerTag, &RecycleDesc)))
        return E_FAIL;*/

    CUI_Texture::DESC TextureDesc{};
    {
        TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_BlackUp.dat";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", LEVEL_LOADING, pLayerTag, &TextureDesc)))
        return E_FAIL;

    {
        TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_BlackDown.dat";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", LEVEL_LOADING, pLayerTag, &TextureDesc)))
        return E_FAIL;

    CUI_Recycle::DESC RecycleDesc{};
    {
        RecycleDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Recycle_Final.dat";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_UI_Recycle", LEVEL_LOADING, pLayerTag, &RecycleDesc)))
        return E_FAIL;

    CUI_Loading::DESC LoadingDesc{};
    {
        LoadingDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Loading.dat";
        LoadingDesc.fSpeed = 10.f;

        LoadingDesc.pNextLevelID = &m_eNextLevelID;
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_UI_Loading", LEVEL_LOADING, pLayerTag, &LoadingDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Loading::Ready_Effect()
{
    CSpriteEffect::DESC SpriteDesc{};

    //// 환경 이펙트
    //HANDLE hFile = CreateFile(L"../Bin/DataFiles/Effects/Bubbles.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    //ReadFile_SpriteEffect(hFile, SpriteDesc);
    //if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_BUBBLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 1, &SpriteDesc)))
    //    return E_FAIL;

    //m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_BUBBLE, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), false, nullptr);

    return S_OK;
}


void CLevel_Loading::ReadFile_SpriteEffect(HANDLE hFile, CSpriteEffect::DESC& Desc)
{
    _tchar szName[MAX_PATH]{};
    _uint iEffectType{}, iTextureIndex{};
    DWORD dwByte{};
    _bool bRead{};
    bRead = ReadFile(hFile, &iEffectType, sizeof(_int), &dwByte, nullptr);
    bRead = ReadFile(hFile, &iTextureIndex, sizeof(_uint), &dwByte, nullptr);
    bRead = ReadFile(hFile, szName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
    Desc.strTextureTag = szName;
    bRead = ReadFile(hFile, &Desc.BufferDesc.ParticleDesc.iNumInstance, sizeof(_int), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.ParticleDesc.vRange, sizeof(_float3), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.ParticleDesc.vCenter, sizeof(_float3), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.ParticleDesc.vSize, sizeof(_float2), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.ParticleDesc.vSpeed, sizeof(_float2), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.ParticleDesc.vPivot, sizeof(_float3), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.ParticleDesc.vLifeTime, sizeof(_float2), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.ParticleDesc.isLoop, sizeof(_bool), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.iPattern, sizeof(_int), &dwByte, nullptr);
    Desc.BufferDesc.ParticleDesc.iPattern = Desc.iPattern;
    Desc.fMaxLifeTime = Desc.BufferDesc.ParticleDesc.vLifeTime.y;

    _int3 vNumAtlas{};
    bRead = ReadFile(hFile, &vNumAtlas, sizeof(_int3), &dwByte, nullptr);
    Desc.vNumAtlas = _int2(vNumAtlas.x, vNumAtlas.y);
    Desc.iNumAtlas = vNumAtlas.z;
    bRead = ReadFile(hFile, &Desc.vUVInterval, sizeof(_float2), &dwByte, nullptr);

    bRead = ReadFile(hFile, &Desc.EffectDesc.bBlendEnable, sizeof(_bool), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.EffectDesc.fActiveDelay, sizeof(_float), &dwByte, nullptr);
    Desc.EffectDesc.iEffectType = iEffectType;

    bRead = ReadFile(hFile, &Desc.EffectDesc.fFinalSize, sizeof(_float), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.EffectDesc.vMulColor, sizeof(_float3), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.EffectDesc.vAddColor, sizeof(_float3), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.EffectDesc.iShadingOption, sizeof(_uint), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.EffectDesc.iMaskImageNum, sizeof(_uint), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.EffectDesc.iNoiseImageNum, sizeof(_uint), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.EffectDesc.iDissolveImageNum, sizeof(_uint), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.EffectDesc.iEmissiveImageNum, sizeof(_uint), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.EffectDesc.iDistortionImageNum, sizeof(_uint), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.EffectDesc.vDissolveStart, sizeof(_float2), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.EffectDesc.fDistortionDissolveStart, sizeof(_float), &dwByte, nullptr);

    CloseHandle(hFile);
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
    CLevel_Loading* pInstance = new CLevel_Loading{ pDevice, pContext };

    if (FAILED(pInstance->Initialize(eNextLevelID))) // 로딩은 initalize 해도 상관없음
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
