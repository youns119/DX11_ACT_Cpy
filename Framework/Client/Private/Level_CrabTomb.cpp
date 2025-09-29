#include "pch.h"
#include "Level_CrabTomb.h"

#include "GameInstance.h"
#include "NPC_Headers.h"

#include "Sky.h"
#include "Theme.h"
#include "Visit_Manger.h"

#include "Camera_Free.h"
#include "Player.h"

#include "MapObject.h"

#include "Boss_Inkerton.h"
#include "Boss_Petroch.h"
#include "Enemy_Cuttlefish.h"
#include "Enemy_BobbitWorm.h"

#include "MapTrigger.h"
#include "TriggerObserverTest.h"
#include "CallbackUpdate.h"
#include "Level_Loading.h"
#include "CutScene_Manager.h"

#include "UI_Manager.h"

#include "WaterShadow.h"
#include "PlayerManager.h"

#include "Shell_Item.h"
CLevel_CrabTomb::CLevel_CrabTomb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

HRESULT CLevel_CrabTomb::Initialize()
{
    /* 맵 오브젝트 먼저 레벨에 추가하고, 그 다음 컨트롤러 넣어야 문제가 없음 */
    if (FAILED(m_pGameInstance->Setup_PhysicsScene(_vec2(-8.f, -8.f), _vec2(256.f, 256.f), 16.f)))
        return E_FAIL;

    //if (FAILED(Ready_Lights()))
    //    return E_FAIL;
    if (FAILED(Ready_Environment()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Environment(TEXT("Layer_Environment"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_MapObject(TEXT("Layer_MapObject"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_InteractionObject(TEXT("Layer_InsteractionObject"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_MapTrigger(TEXT("Layer_MapTrigger"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Projectile(TEXT("Layer_Projectile"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Shell(TEXT("Layer_Shell"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_CinematicCamera(TEXT("Layer_CinematicCamera"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Manager(TEXT("Layer_Manager"))))
        return E_FAIL;

    m_pGameInstance->Play_Sound_Loop(L"Water_Travel_Loop_1.wav", (_uint)SOUND_CHANNEL::AMBIENCE_LOOP, 0.f);
    m_pGameInstance->Play_Sound_Loop(L"CrabTomb_BGM.wav", (_uint)SOUND_CHANNEL::BGM, 0.5f);

#ifdef _DEBUG
    if (FAILED(Ready_Layer_Debug(TEXT("Layer_Debug"))))
        return E_FAIL;
    if (FAILED(Ready_Layer_CameraTool(TEXT("Layer_CameraTool"))))
        return E_FAIL;
#endif // _DEBUG

    m_pGameInstance->Add_Layer_To_Lag(TEXT("Layer_Projectile"));
    m_pGameInstance->Add_Layer_To_Lag(TEXT("Layer_Shell"));
    m_pGameInstance->Add_Layer_To_Lag(TEXT("Layer_Player"));
    m_pGameInstance->Add_Layer_To_Lag(TEXT("Layer_Camera"));
    m_pGameInstance->Add_Layer_To_Lag(TEXT("Layer_Monster"));

    SetUp_CollisionGroup();

    CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    pUIManager->Add_PartObject_For_Play();
    pUIManager->FadeIn();

    if (false == CVisit_Manger::Is_Visit_FromStatic(L"crabTomp"))
    {
        pUIManager->Show_TextUI(L"게 무덤", _float4(235.f / 255.f, 215.f / 255.f, 254.f / 255.f, 1.f));
    }


    /* Level_Initalize 동안의 누적된 타임델타 제거 */
    m_pGameInstance->Compute_TimeDelta(L"Timer_60");

    return S_OK;
}

void CLevel_CrabTomb::Update(_float fTimeDelta)
{
}

HRESULT CLevel_CrabTomb::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"게무덤 레벨 입니다.");
#endif
    return S_OK;
}

HRESULT CLevel_CrabTomb::Ready_Layer_Environment(const _wstring& strLayerTag)
{
    CSky::DESC SkyDesc{};
    SkyDesc.iTextureIndex = 0;
    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_Sky", LEVELID::LEVEL_CRABTOMB, strLayerTag, &SkyDesc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
        return E_FAIL;

    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_CrabTomb_Terrain"), LEVEL_CRABTOMB, strLayerTag)))
        return E_FAIL;

    /* for.WaterShadow */
    {
        CWaterShadow::DESC Desc {};
        Desc.GameObjectDesc.WorldMatrix = _mat::ScalingMatrix({ 0.5f, 0.5f, 0.5f })._float4x4();
        Desc.GameObjectDesc.strName = L"WaterShadow";
        Desc.CauticsParams.fIntensity = 0.4f;

        if (FAILED(m_pGameInstance->
            Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_WaterShadow", LEVEL_CRABTOMB, strLayerTag, &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_CrabTomb::Ready_Environment()
{
    {   /*Point Light*/
        _wstring szFile = L"../Bin/DataFiles/Map_Light/CrabTomb.light";

        //로드하기
        HRESULT hr{};
        DWORD dwByte{};
        HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        _bool bRead = false;

        if (INVALID_HANDLE_VALUE == hFile)
        {
            MSG_BOX("Can Not Open File");
            return E_FAIL;
        }

        while (true)
        {
            _wstring strLightName;
            strLightName.resize(MAX_PATH);

            _tchar szLightName[MAX_PATH] = {};

            bRead = ReadFile(hFile, &szLightName, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);
            strLightName = szLightName;

            LIGHT_DESC LightDesc = {};

            bRead = ReadFile(hFile, &LightDesc, sizeof(LIGHT_DESC), &dwByte, nullptr);

            if (!bRead || dwByte == 0)
                break;

            m_pGameInstance->Add_Light(strLightName.c_str(), LightDesc);
        }
        CloseHandle(hFile);
    }
    
    THEME_DESC* pThemeDesc { nullptr };
    vector<_wstring> ThemeFilePaths { L"../Bin/DataFiles/Atmosphere/CrabTomb.atmosphere", };

    if (FAILED(THEME_DESC::Load_Themes(&pThemeDesc, ThemeFilePaths, 1)))
        return E_FAIL;
    pThemeDesc->Set_Atmosphere();
    Safe_Delete(pThemeDesc);


    return S_OK;
}

HRESULT CLevel_CrabTomb::Ready_Layer_MapObject(const _wstring& strLayerTag)
{
    //맵오브젝트
    {
        _wstring szFile = L"../Bin/DataFiles/Map_Object/CrabTomb.environment";

        //로드하기
        HRESULT hr{};
        DWORD dwByte{};
        HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        if (INVALID_HANDLE_VALUE == hFile)
        {
            MSG_BOX("Can Not Open File");
            return E_FAIL;
        }

        while (true)
        {
            _wstring strName;
            strName.resize(MAX_PATH);
            _tchar szName[MAX_PATH] = {};

            _float4x4 WorldMat = {};// 월드 행렬
            _float fRange = {};

            _bool bRead = false;

            bRead = ReadFile(hFile, &szName, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);
            strName = szName;
            bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);
            bRead = ReadFile(hFile, &fRange, sizeof(_float), &dwByte, nullptr);

            _float4 vBaseColor = { 1.f, 1.f, 1.f, 1.f };
            bRead = ReadFile(hFile, &vBaseColor, sizeof(_float4), &dwByte, nullptr);

            _uint iPhysicState = {};
            bRead = ReadFile(hFile, &iPhysicState, sizeof(_uint), &dwByte, nullptr);

            CMapObject::DESC Desc = {};
            Desc.GameObjectDesc.strName = strName;
            Desc.GameObjectDesc.WorldMatrix = WorldMat;
            Desc.GameObjectDesc.fSpeedPerSec = fRange;
            Desc.ePhysicsType = iPhysicState;

            if (!bRead || 0 == dwByte)
                break;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_MapObject", LEVELID::LEVEL_CRABTOMB, strLayerTag, &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
                return E_FAIL;
        }

        CloseHandle(hFile);
    }


    return S_OK;
}

HRESULT CLevel_CrabTomb::Ready_Layer_InteractionObject(const _wstring& strLayerTag)
{
    {
        _wstring szFile = L"../Bin/DataFiles/Map_InteractionObject/CrabTomb.interaction"; //다이얼로그로 가져오는 파일패스 저장할 곳
        /*다이얼로그 띄우기*/

        HRESULT hr{};
        DWORD dwByte{};
        HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        if (INVALID_HANDLE_VALUE == hFile)
        {
            MSG_BOX("Can Not Open File");
            return E_FAIL;
        }

        while (true)
        {
            _wstring strName;
            strName.resize(MAX_PATH);
            _tchar szName[MAX_PATH] = {};

            _float4x4 WorldMat = {};// 월드 행렬
            _bool bRead = false;

            bRead = ReadFile(hFile, &szName, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);
            strName = szName;
            bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);

            if (!bRead || 0 == dwByte)
                break;

            CGameObject::DESC Desc = {};
            Desc.strName = strName;
            Desc.TransformDesc.WorldMatrix = WorldMat;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_CRABTOMB, strName, LEVELID::LEVEL_CRABTOMB, strLayerTag, &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
            {
                if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, strName, LEVELID::LEVEL_CRABTOMB, strLayerTag, &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
                    return E_FAIL;
            }
        }

        CloseHandle(hFile);

    }

    {
        _wstring szFile = L"../Bin/DataFiles/Map_InteractionObject/SheleportID_CrabTomb.interaction"; //다이얼로그로 가져오는 파일패스 저장할 곳
        /*다이얼로그 띄우기*/

        HRESULT hr{};
        DWORD dwByte{};
        HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        if (INVALID_HANDLE_VALUE == hFile)
        {
            MSG_BOX("Can Not Open File");
            return E_FAIL;
        }

        while (true)
        {
            _int iSheleportID = { -1 };
            _float4x4 WorldMat = {};// 월드 행렬
            _bool bRead = false;

            bRead = ReadFile(hFile, &iSheleportID, sizeof(_int), &dwByte, nullptr);
            bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);

            if (!bRead || 0 == dwByte)
                break;

            CShellHouse::DESC Desc = {};
            Desc.iSheleportID = iSheleportID;
            Desc.GameObjectDesc.TransformDesc.WorldMatrix = WorldMat;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_ShellHouse", LEVELID::LEVEL_CRABTOMB, L"Layer_Insteraction_Environment", &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
                return E_FAIL;

        }

        CloseHandle(hFile);
    }
    return S_OK;

}

HRESULT CLevel_CrabTomb::Ready_Layer_MapTrigger(const _wstring& strLayerTag)
{
    {//여기서 트리거 파일 불러오깅 ㅠ
        _wstring szFile = L"../Bin/DataFiles/MapTrigger/CrabTombTrigger.mapTrigger";
        _bool bRead = false;
        HRESULT hr{};
        DWORD dwByte{};
        HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        if (INVALID_HANDLE_VALUE == hFile)
        {
            MSG_BOX("Can Not Open File");
            CloseHandle(hFile);
            return E_FAIL;
        }

        while (true)
        {
            _wstring strName = {};
            _tchar szName[MAX_PATH] = {};
            bRead = ReadFile(hFile, szName, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);
            strName = szName;

            _float4x4 WorldMat = {};
            bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);

            if (!bRead || dwByte == 0)
                break;
            CMapTrigger::DESC Desc = {};
            Desc.GameObjectDesc.strName = strName.c_str();
            Desc.GameObjectDesc.WorldMatrix = WorldMat;
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapTrigger", LEVEL_CRABTOMB, L"Layer_MapTrigger", &Desc)))
            {
                MSG_BOX("트리거 추가 안댐.");
                break;
            }
        }

        CloseHandle(hFile);
    }

    /*트리거 세팅*/
    CMapTrigger* pTrigger{ nullptr };
    {
        pTrigger = static_cast<CMapTrigger*>(m_pGameInstance->Find_GameObject_With_Name(LEVEL_CRABTOMB, L"Layer_MapTrigger", L"Trigger_CrabTombToField"));

        pTrigger->Add_CollisionEnter_Callback([this]()->void
            {
                CPlayerManager::Save_Transition(XMMatrixRotationY(XMConvertToRadians(14.f)) * XMMatrixTranslation(896.025f, 5.f, 515.636f));
                m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIELD));
            });
    }

    {
        pTrigger = static_cast<CMapTrigger*>(m_pGameInstance->Find_GameObject_With_Name(LEVEL_CRABTOMB, L"Layer_MapTrigger", L"Play_CrabTomb_CutScene"));

        pTrigger->Add_CollisionEnter_Callback([this]()->void
            {
                CCutScene_Manager* pCutSceneManager = static_cast<CCutScene_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CutSceneManager", 0));
                if(!pCutSceneManager->Is_CutSceneFinished(L"CrabTomb_End"))
                    pCutSceneManager->Begin_CutScene(L"CrabTomb_Start");
            });
    }

    return S_OK;
}

HRESULT CLevel_CrabTomb::Ready_Layer_Projectile(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Projectile_Manager", LEVEL_CRABTOMB, strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_CrabTomb::Ready_Layer_Shell(const _wstring& strLayerTag)
{
    {   // 쉘 설치 
        wstring szFile = L"../Bin/DataFiles/Map_Item/CrabTomb_Shell.item";
        _bool bRead = false;
        HRESULT hr{};
        DWORD dwByte{};
        HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        if (INVALID_HANDLE_VALUE == hFile)
        {
            MSG_BOX("Can Not Open File");
            CloseHandle(hFile);
            return E_FAIL;
        }

        while (true)
        {
            _int iType = {};
            bRead = ReadFile(hFile, &iType, sizeof(_int), &dwByte, nullptr);

            _float4x4 WorldMat = {};
            bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);

            if (!bRead || dwByte == 0)
                break;

            CShell_Item::DESC tShellDesc = {};
            tShellDesc.eShell = (SHELL)iType;
            tShellDesc.ItemDesc.GameObjectDesc.WorldMatrix = WorldMat;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Shell_World", LEVEL_CRABTOMB, strLayerTag, &tShellDesc)))
                return E_FAIL;
        }

        CloseHandle(hFile);
    }

    return S_OK;
}

HRESULT CLevel_CrabTomb::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    CCamera_Free::DESC CameraFreeDesc{};
    {
        CameraFreeDesc.CameraDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 10.f;
        CameraFreeDesc.CameraDesc.GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

        CameraFreeDesc.CameraDesc.vEye = _float3(0.f, 10.f, -7.f);
        CameraFreeDesc.CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
        CameraFreeDesc.CameraDesc.fFovY = XMConvertToRadians(60.f);
        CameraFreeDesc.CameraDesc.fNear = 0.1f;
        CameraFreeDesc.CameraDesc.fFar = 1000.f;

        CameraFreeDesc.fMouseSensitivity = 0.005f;
        //CameraFreeDesc.fMouseSensitivity = 0.1f;
    }

    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"), LEVEL_CRABTOMB, strLayerTag, &CameraFreeDesc)))
        return E_FAIL;

    return S_OK;

}

HRESULT CLevel_CrabTomb::Ready_Layer_CinematicCamera(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer
        (
            LEVEL_STATIC,
            TEXT("Prototype_GameObject_CinematicCamera"),
            LEVEL_CRABTOMB,
            strLayerTag
        )))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_CrabTomb::Ready_Layer_Player(const _wstring& strLayerTag)
{
    CPlayer::DESC PlayerDesc{};
    XMStoreFloat4x4(&PlayerDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(40.f, 5.f, 40.f));
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Player"), LEVEL_CRABTOMB, strLayerTag, &PlayerDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_CrabTomb::Ready_Layer_Monster(const _wstring& strLayerTag)
{
    {
        CBoss_Inkerton::Inkerton_DESC InkertonDesc{};
        InkertonDesc.bIsEncounter = false;
        InkertonDesc.EnemyDesc.enemyStats.fMaxHP = 1500.f;
        InkertonDesc.EnemyDesc.enemyStats.fMaxBP = 210.f;
        InkertonDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
        InkertonDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
        InkertonDesc.EnemyDesc.GameObjectDesc.strName = L"Boss_Inkerton";
        InkertonDesc.EnemyDesc.szName = L"게 사냥꾼 잉커톤";

        //904.f, 1.f, 536.f

        XMStoreFloat4x4(&InkertonDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationRollPitchYaw(0.f, -90.f, 0.f) * XMMatrixTranslation(157.f, 1.5f, 155.f));
        //XMStoreFloat4x4(&InkertonDesc.EnemyDesc.OriginalPos, XMMatrixRotationRollPitchYaw(0.f, -90.f, 0.f) * XMMatrixTranslation(157.f, 1.5f, 155.f));
        XMStoreFloat4x4(&InkertonDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(0.f, -1000.f, 0.f));

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_CRABTOMB, L"Prototype_GameObject_Boss_Inkerton", LEVEL_CRABTOMB, strLayerTag, &InkertonDesc)))
            return E_FAIL;
    }

    {
        CBoss_Petroch::DESC PetrochDesc{};
        PetrochDesc.EnemyDesc.enemyStats.fMaxHP = 2500.f;
        PetrochDesc.EnemyDesc.enemyStats.fMaxBP = 250.f;
        PetrochDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
        PetrochDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
        PetrochDesc.EnemyDesc.szName = L"거짓된 달 페트로크";

        XMStoreFloat4x4(&PetrochDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationY(XMConvertToRadians(-100.f)) * XMMatrixTranslation(63.9f, 1.3f, 39.4f));
        XMStoreFloat4x4(&PetrochDesc.EnemyDesc.OriginalPos, XMMatrixRotationY(XMConvertToRadians(-100.f)) * XMMatrixTranslation(63.9f, 1.3f, 39.4f));

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_CRABTOMB, L"Prototype_GameObject_Boss_Petroch", LEVEL_CRABTOMB, strLayerTag, &PetrochDesc)))
            return E_FAIL;
    }

    {
        CEnemy_Cuttlefish::DESC CuttlefishDesc{};
        CuttlefishDesc.EnemyDesc.enemyStats.fMaxHP = 300.f;
        CuttlefishDesc.EnemyDesc.enemyStats.fMaxBP = 60.f;
        CuttlefishDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
        CuttlefishDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
        
        XMStoreFloat4x4(&CuttlefishDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationRollPitchYaw(0.f, 157.f, 0.f) * XMMatrixTranslation(135.f, 0.2f, 113.f));
        XMStoreFloat4x4(&CuttlefishDesc.EnemyDesc.OriginalPos, XMMatrixRotationRollPitchYaw(0.f, 157.f, 0.f) * XMMatrixTranslation(135.f, 0.2f, 113.f));

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_CRABTOMB, L"Prototype_GameObject_Enemy_Cuttlefish", LEVEL_CRABTOMB, strLayerTag, &CuttlefishDesc)))
            return E_FAIL;
    }

    {
        CEnemy_Cuttlefish::DESC CuttlefishDesc{};
        CuttlefishDesc.EnemyDesc.enemyStats.fMaxHP = 300.f;
        CuttlefishDesc.EnemyDesc.enemyStats.fMaxBP = 60.f;
        CuttlefishDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
        CuttlefishDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
        
        XMStoreFloat4x4(&CuttlefishDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationRollPitchYaw(0.f, -124.f, 0.f) * XMMatrixTranslation(91.f, 0.2f, 130.f));
        XMStoreFloat4x4(&CuttlefishDesc.EnemyDesc.OriginalPos, XMMatrixRotationRollPitchYaw(0.f, -124.f, 0.f) * XMMatrixTranslation(91.f, 0.2f, 130.f));

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_CRABTOMB, L"Prototype_GameObject_Enemy_Cuttlefish", LEVEL_CRABTOMB, strLayerTag, &CuttlefishDesc)))
            return E_FAIL;
    }

    {
        CEnemy_Cuttlefish::DESC CuttlefishDesc{};
        CuttlefishDesc.EnemyDesc.enemyStats.fMaxHP = 300.f;
        CuttlefishDesc.EnemyDesc.enemyStats.fMaxBP = 60.f;
        CuttlefishDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
        CuttlefishDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
        
        XMStoreFloat4x4(&CuttlefishDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationRollPitchYaw(0.f, -30.f, 0.f) * XMMatrixTranslation(123.f, 0.2f, 95.f));
        XMStoreFloat4x4(&CuttlefishDesc.EnemyDesc.OriginalPos, XMMatrixRotationRollPitchYaw(0.f, -30.f, 0.f) * XMMatrixTranslation(123.f, 0.2f, 95.f));

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_CRABTOMB, L"Prototype_GameObject_Enemy_Cuttlefish", LEVEL_CRABTOMB, strLayerTag, &CuttlefishDesc)))
            return E_FAIL;
    }

    {
        CEnemy_BobbitWorm::DESC BobbitWormDesc{};
        BobbitWormDesc.EnemyDesc.enemyStats.fMaxHP = 30.f;
        BobbitWormDesc.EnemyDesc.enemyStats.fMaxBP = 30.f;
        BobbitWormDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
        BobbitWormDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
        
        XMStoreFloat4x4(&BobbitWormDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationRollPitchYaw(0.f, -90.f, 0.f) * XMMatrixTranslation(88.20f, 1.f, 108.f));
        XMStoreFloat4x4(&BobbitWormDesc.EnemyDesc.OriginalPos, XMMatrixRotationRollPitchYaw(0.f, -90.f, 0.f) * XMMatrixTranslation(88.20f, 1.f, 108.f));

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_CRABTOMB, L"Prototype_GameObject_Enemy_BobbitWorm", LEVEL_CRABTOMB, strLayerTag, &BobbitWormDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_CrabTomb::Ready_Layer_NPC(const _wstring& strLayerTag)
{
    CNPC_Inkerton_Final::DESC InkertonDesc{};
    {
        InkertonDesc.szFileName = L"../Bin/DataFiles/NPC_Data/CrabTomb_Inkerton.npc";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Inkerton_Final", LEVEL_CRABTOMB, strLayerTag, &InkertonDesc)))
        return E_FAIL;

    CNPC_Roland_Final::DESC RolandDesc{};
    {
        RolandDesc.szFileName = L"../Bin/DataFiles/NPC_Data/CrabTomb_Roland.npc";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Roland_Final", LEVEL_CRABTOMB, strLayerTag, &RolandDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_CrabTomb::Ready_Layer_Manager(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_CRABTOMB, L"Prototype_GameObject_Enemy_Manager", LEVEL_CRABTOMB, strLayerTag, nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_CrabTomb::Ready_Layer_Effect(const _wstring& strLayerTag)
{
    CSpriteEffect::DESC SpriteDesc{};
    CMeshEffect::DESC   MeshDesc{};
    CMeshInstEffect::DESC MeshInstDesc{};

    // 환경 이펙트
    HANDLE hFile = CreateFile(L"../Bin/DataFiles/Effects/Bubbles.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_BUBBLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 16, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Env_GlowInsects.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_GLOWINSECTS, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 1, &SpriteDesc)))
        return E_FAIL;

    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_GLOWINSECTS, XMVectorSet(128.f, 0.f, 128.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), false, nullptr);

    // ENV_MAPTRIGGER_INK
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/MapTrigger_Ink.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_MAPTRIGGER_INK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 1, &SpriteDesc)))
        return E_FAIL;

    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_MAPTRIGGER_INK, XMVectorSet(20.f, 0.f, 20.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), false, nullptr);

    // 골레포트 이펙트(활성화)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Goleport/Goleport_Off_Wind_Yellow.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::GOLEPORT_MESH_WIND_WAIT00, LEVEL_STATIC, L"Prototype_Effect_Mesh_ActivateShellWind", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Goleport/Goleport_Off_Wind_Purple.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::GOLEPORT_MESH_WIND_WAIT01, LEVEL_STATIC, L"Prototype_Effect_Mesh_ActivateShellWind", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Goleport/Goleport_Off_CenterCircle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::GOLEPORT_SPRT_CIRCLE_WAIT, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 8, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Goleport/Goleport_Activate_MoonShell.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::GOLEPORT_MESH_SHELLHOUSE, LEVEL_STATIC, L"Prototype_Effect_Mesh_ActivateShell", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Goleport/Goleport_Activate_MoonBrick.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::GOLEPORT_MESH_SHELLBRICK, LEVEL_STATIC, L"Prototype_Effect_Mesh_ActivateShell", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Goleport/Goleport_Activate_Barnacle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::GOLEPORT_MESH_BARNACLE, LEVEL_STATIC, L"Prototype_Effect_Mesh_ActivateShell", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Goleport/Goleport_Activate_Wind.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::GOLEPORT_MESH_WIND_ACTIVATED, LEVEL_STATIC, L"Prototype_Effect_Mesh_ActivateShellWind", 2, &MeshDesc)))
        return E_FAIL;

    // 골레포트 이펙트(상시)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Goleport/Goleport_On_Wind00.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::GOLEPORT_MESH_WIND00, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Goleport/Goleport_On_Wind01.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::GOLEPORT_MESH_WIND01, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 8, &MeshDesc)))
        return E_FAIL;

    // 모래 이펙트
    hFile = CreateFile(L"../Bin/DataFiles/Effects/SandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PTCL_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 32, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/JumpSandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPSANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/EvadeSandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_EVADESANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    // 피격 이펙트(파티클)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/HitSparkPtcl.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PTCL_HITSPARK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 16, &SpriteDesc)))
        return E_FAIL;

    // 피격 이펙트(플레이어)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Ptcl_Hit.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HIT, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Sprite_Hit.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_SPRITE_HIT, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Damaged/Player_Damaged_Ptcl_Blood.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_DAMAGED_BLOOD, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Damaged/Player_Damaged_Ptcl_RedSpark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_DAMAGED_REDSPARK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Damaged/Player_Damaged_Sprt_RedSpark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_SPRT_DAMAGED_REDSPARK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    // 피격 이펙트(등껍질)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/ShellBreak/ShellBreak_MeshInst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshInstEffect(hFile, MeshInstDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::SHELL_MESHINST_EXPLODE, LEVEL_STATIC, L"Prototype_Effect_MeshInst_Once", 4, &MeshInstDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Damaged/Player_Damaged_Ptcl_ShellDefend.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_DAMAGED_SHELLDEFEND, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    // 버블 이펙트
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/SwimHandBubbles.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_SWIMBUBBLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 16, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/JumpBubble.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPBUBBLE, LEVEL_STATIC, L"Prototype_Effect_Change_ExplToRiseW", 4, &SpriteDesc)))
        return E_FAIL;

    // 차징 이펙트
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/ChargingAtt.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_CHARGING_ATT, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/ChargingSeq00.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_CHARGING_SEQ00, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/ChargingSeq01.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_CHARGING_SEQ01, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/ChargingSeq02.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_CHARGING_SEQ02, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    // 대쉬 이펙트
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Ptcl_DashBefore.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_DASH_BEFORE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Sprite_DashAttack.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_SPRITE_DASH_ATT, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    // 패링 이펙트
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Parry/ParryPtclBlue.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PARRY_BLUE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Parry/ParryPtclYellow.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PARRY_YELLOW, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Parry/ParryPtclRed.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PARRY_RED, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Parry/ParryPtclRedBig.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PARRY_REDBIGONE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    // 공격 이펙트(메쉬)
    // hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Trail_AlphaBlend_Needit.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Trail_WeightBlend_Needit.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_ATT00, LEVEL_STATIC, L"Prototype_Effect_PlayerAttNormal", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Trail_JumpAttack.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_SWIPE_ATT, LEVEL_STATIC, L"Prototype_Effect_PlayerAttSwipe", 8, &MeshDesc)))
        return E_FAIL;

    // 공격 이펙트(파티클)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Plunge/Plunge_Ptcl_Spark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_SPARK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Plunge/Plunge_Ptcl_BigDiamond.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PLUNGE_BIGDIAMOND, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Plunge/Plunge_Ptcl_Diamond.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PLUNGE_DIAMOND, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    // 공격 이펙트(해머)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Hammer/Hammer_Dark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_DARK, LEVEL_STATIC, L"Prototype_Effect_PlayerHammerOnce", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Hammer/Hammer_White.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_LIGHT, LEVEL_STATIC, L"Prototype_Effect_PlayerHammerOnce", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Hammer/Hammer_Dark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_TWICEDARK, LEVEL_STATIC, L"Prototype_Effect_PlayerAttSwipe", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Hammer/Hammer_White.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_TWICELIGHT, LEVEL_STATIC, L"Prototype_Effect_PlayerAttSwipe", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Hammer/Hammer_Dark_360.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_JUMPDARK, LEVEL_STATIC, L"Prototype_Effect_PlayerHammerOnce", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Hammer/Hammer_White_360.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_JUMPLIGHT, LEVEL_STATIC, L"Prototype_Effect_PlayerHammerOnce", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Hammer/Hammer_Mesh_Lightning.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_LIGHTNING, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Hammer/Hammer_Ptcl_Dash_SandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_DASH_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Hammer/Hammer_Ptcl_SandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Hammer/Hammer_Ptcl_Spark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_SPARK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Hammer/Hammer_Charging.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_ELECTRIC_CHARGING, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Hammer/Hammer_Ptcl_ElectricSpread.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_ELECTRIC_SPREAD, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Hammer/Hammer_Ptcl_Charging.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_CHARGING, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Hammer/Hammer_Ptcl_Dash_Bubble.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_DASH_BUBBLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    // 스킬 이펙트(맨티스 펀치)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Player/Mantis_Ptcl_Charging.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_MANTISPUNCH_CHARGING, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Player/Mantis_Ptcl_HoldCircle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_MANTISPUNCH_HOLDCIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Player/Mantis_Ptcl_ShrinkCircle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_MANTISPUNCH_SHRINKCIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Player/Mantis_Ptcl_SandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_MANTISPUNCH_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Player/Mantis_Ptcl_Teleport.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_MANTISPUNCH_TELE_START, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Player/Mantis_Ptcl_Teleport.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_MANTISPUNCH_TELE_END, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Player/Mantis_Mesh_Teleport.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_MANTISPUNCH_TELEPORT, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Player/Mantis_Mesh_WindImpact.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_MANTISPUNCH_WINDIMPACT, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Player/Mantis_Ptcl_DarkImpact.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_MANTISPUNCH_DARKIMPACT, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Player/Mantis_MeshInst_DimensionBreak.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshInstEffect(hFile, MeshInstDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESHINST_MANTIS_DIMENSIONBREAK, LEVEL_STATIC, L"Prototype_Effect_MeshInst_Once", 4, &MeshInstDesc)))
        return E_FAIL;

    // 스킬 이펙트(맨티스 펀치 - 토포다)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Topoda/Mantis_Ptcl_Charging.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::TOPODA_PTCL_MANTISPUNCH_CHARGING, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Topoda/Mantis_Ptcl_HoldCircle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::TOPODA_PTCL_MANTISPUNCH_HOLDCIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Topoda/Mantis_Ptcl_ShrinkCircle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::TOPODA_PTCL_MANTISPUNCH_SHRINKCIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Topoda/Mantis_Ptcl_SandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::TOPODA_PTCL_MANTISPUNCH_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Topoda/Mantis_Ptcl_Teleport.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::TOPODA_PTCL_MANTISPUNCH_TELE_START, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Topoda/Mantis_Ptcl_Teleport.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::TOPODA_PTCL_MANTISPUNCH_TELE_END, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Topoda/Mantis_Mesh_Teleport.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::TOPODA_MESH_MANTISPUNCH_TELEPORT, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Topoda/Mantis_Mesh_WindImpact.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::TOPODA_MESH_MANTISPUNCH_WINDIMPACT, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Topoda/Mantis_Ptcl_DarkImpact.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::TOPODA_PTCL_MANTISPUNCH_DARKIMPACT, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Topoda/Mantis_MeshInst_DimensionBreak.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshInstEffect(hFile, MeshInstDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::TOPODA_MESHINST_MANTISPUNCH_DIMENSIONBREAK, LEVEL_STATIC, L"Prototype_Effect_MeshInst_Once", 2, &MeshInstDesc)))
        return E_FAIL;

    // 스킬 이펙트(맨티스 펀치 - 공통)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Mantis_MeshInst_GlassBreak.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshInstEffect(hFile, MeshInstDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::MANTISPUNCH_MESHINST_GLASSBREAK, LEVEL_STATIC, L"Prototype_Effect_MeshInst_Once", 2, &MeshInstDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Mantis_Ptcl_HitBubble.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::MANTISPUNCH_PTCL_HITBUBBLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/MantisPunch/Mantis_Ptcl_HitSpark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::MANTISPUNCH_PTCL_HITSPARK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;


    // 공격 이펙트(스킬)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Umami/Skill_Fizzle_Blue.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FIZZLE_BLUE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 100, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Umami/Skill_Fizzle_Purple.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FIZZLE_PURPLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 100, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Umami/Skill_Fizzle_Burst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FIZZLE_BURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 100, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Umami/Skill_Fizzle_Flash.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FIZZLE_FLASH, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 10, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Umami/Skill_Fizzle_SandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FIZZLE_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 10, &SpriteDesc)))
        return E_FAIL;

    // 공격 이펙트(스킬 - 롤링)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Rolling/Mesh_MainRoll.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_ROLL_MAINROLL, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Rolling/Mesh_SubRoll.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_ROLL_SUBROLL, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Rolling/Mesh_WindRoll.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_ROLL_WINDROLL, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Rolling/Ptcl_Roll_SandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_ROLL_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 16, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Rolling/Ptcl_Roll_Spark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_ROLL_SPARK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 4, &SpriteDesc)))
        return E_FAIL;

    // 공격 이펙트(스킬 - 성게 폭탄)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Mesh_ChargingGround_Tornado_ED.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_CHARGING_TORNADO_ED, LEVEL_STATIC, L"Prototype_Effect_PlayerAttNormal", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Mesh_ChargingGround_Tornado_Red.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_CHARGING_TORNADO_RED, LEVEL_STATIC, L"Prototype_Effect_PlayerAttNormal", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Mesh_BombingUrchin.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_CHARGING_URCHIN, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Mesh_ChargingSphere.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_CHARGING_WIND, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Mesh_ChargingRing.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_CHARGING_RING, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Mesh_ChargingSphere_Distortion.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_CHARGING_DISTORTION, LEVEL_STATIC, L"Prototype_Effect_Mesh_Increase", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Ptcl_ChargingParticle_Lv0.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_PTCL_CHARGING_LIGHT_LV0, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Ptcl_ChargingParticle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_PTCL_CHARGING_LIGHT_LV1, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Ptcl_WindCircle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_PTCL_CHARGING_SMOKE_LV1, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 16, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Mesh_ChargingSphere_B.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_EXPLODE_SPHERE, LEVEL_STATIC, L"Prototype_Effect_Mesh_Increase", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Mesh_ExplodeGround.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_EXPLODE_GROUNDBURST, LEVEL_STATIC, L"Prototype_Effect_Mesh_Increase", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Mesh_ExplodeBlast_Lv0.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_EXPLODE_BACKBLAST_LV0, LEVEL_STATIC, L"Prototype_Effect_Mesh_Increase", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Mesh_ExplodeBlast_Lv1.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_EXPLODE_BACKBLAST_LV1, LEVEL_STATIC, L"Prototype_Effect_Mesh_Increase", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Mesh_ExplodeBlast.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_EXPLODE_BACKBLAST_LV2, LEVEL_STATIC, L"Prototype_Effect_Mesh_Increase", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Ptcl_ExplodeParticle_Lv0.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_PTCL_EXPLODE_SMOKE_LV0, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 16, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Ptcl_ExplodeParticle_Lv1.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_PTCL_EXPLODE_SMOKE_LV1, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 16, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/UrchinBomb/UrchinBomb_Ptcl_ExplodeParticle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::URCHINBOMB_PTCL_EXPLODE_SMOKE_LV2, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 16, &SpriteDesc)))
        return E_FAIL;

    // 공격 이펙트(피니쉬)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Finish/Ptcl_Blood.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FINISH_BLOOD, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Finish/Ptcl_Blood_Up.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FINISH_BLOOD_UP, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Finish/MeshInst_Execution.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshInstEffect(hFile, MeshInstDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESHINST_FINISH_EXECUTEICON, LEVEL_STATIC, L"Prototype_Effect_MeshInst_Once", 2, &MeshInstDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Finish/MeshInst_Circle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshInstEffect(hFile, MeshInstDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESHINST_FINISH_CIRCLE, LEVEL_STATIC, L"Prototype_Effect_MeshInst_Once", 2, &MeshInstDesc)))
        return E_FAIL;

    // 포크 이펙트(획득, 업그레이드)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Fork/Fork_Ptcl_Circle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FORK_DIAMOND, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Fork/Fork_Ptcl_Diamond.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FORK_CIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Fork/Fork_Ptcl_Spread.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FORK_SPREAD, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    // 회복 이펙트(하트켈프)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/HeartKelp/Kelp_Ptcl_Diamond.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HEAL_DIAMOND, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 1, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/HeartKelp/Kelp_Ptcl_Circle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HEAL_CIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 1, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/HeartKelp/Kelp_Ptcl_Spread.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HEAL_SPREAD, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 1, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/HeartKelp/Kelp_Sprt_Spread.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_SPRT_HEAL_SPREAD, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 1, &SpriteDesc)))
        return E_FAIL;

    //// 로프 액션 이펙트
    //hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/RopeAction/Trail_Trail.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    //ReadFile_MeshEffect(hFile, MeshDesc);
    //if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_ROPEACTION, LEVEL_STATIC, L"Prototype_Effect_PlayerAttNormal", 8, &MeshDesc)))
    //    return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/RopeAction/RopePtclBlueBig.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_ROPEACTION_BLUEBIG, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/RopeAction/RopePtclDiamond.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_ROPEACTION_DIAMOND, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/RopeAction/MeshInst_DamageImpact.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshInstEffect(hFile, MeshInstDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESHINST_ROPEACTION_DAMAGEIMPACT, LEVEL_STATIC, L"Prototype_Effect_MeshInst_Once", 4, &MeshInstDesc)))
        return E_FAIL;

    // 디스토션
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Radial_Distortion.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, LEVEL_STATIC, L"Prototype_Effect_RadialDistortion", 16, &MeshDesc)))
        return E_FAIL;


    // 몬스터 : 잉커톤 이펙트
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton_Squid.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENEMY_SPRITE_SQUID, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    //--- Mixup Charging
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_08_Charging.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE00_PTCL_CHARGING, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_00_Charge.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE00_PTCL_CHARGE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_01_Circle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE01_PTCL_CIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_02_Flower.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE02_PTCL_FLOWER, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_03_Diamond.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE03_PTCL_DIAMOND, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_04_Circle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE04_PTCL_CIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_05_InnerCircle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE05_PTCL_INNERCIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_06_ShrinkDiamond.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE06_PTCL_SHIRINKDIAMOND, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_07_FixedDiamond.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE07_PTCL_FIXEDDIAMOND, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    //---MixupHeavy

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/MixupHeavy/Inkerton_Mesh_00_MainBlade.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_HEAVY00_MESH_MAINBLADE, LEVEL_STATIC, L"Prototype_Effect_InkertonIncrease", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/MixupHeavy/Inkerton_Mesh_01_LightLineBlade.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_HEAVY01_MESH_LIGHTLINEBLADE, LEVEL_STATIC, L"Prototype_Effect_InkertonIncrease", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/MixupHeavy/Inkerton_Mesh_02_DarkTrail.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_HEAVY02_MESH_DARKTRAIL, LEVEL_STATIC, L"Prototype_Effect_InkertonIncrease", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/MixupHeavy/Inkerton_Mesh_03_Glow.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_HEAVY03_MESH_GLOW, LEVEL_STATIC, L"Prototype_Effect_InkertonIncrease", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/MixupHeavy/Inkerton_Mesh_04_Wind.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_HEAVY04_MESH_WIND, LEVEL_STATIC, L"Prototype_Effect_InkertonScalingY", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/MixupHeavy/Inkerton_Mesh_05_Small_RingMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_HEAVY05_MESH_SMALLRING, LEVEL_STATIC, L"Prototype_Effect_RolandBigSwipe", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/MixupHeavy/Inkerton_Mesh_06_Big_RingMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_HEAVY06_MESH_BIGRING, LEVEL_STATIC, L"Prototype_Effect_RolandBigSwipe", 4, &MeshDesc)))
        return E_FAIL;

    //---AimFire

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Celebrate_Fire/Inkerton_Gunshot_00_Diamond.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT00_PTCL_DIAMOND, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Celebrate_Fire/Inkerton_Gunshot_01_BigSpreadDiamond.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT01_PTCL_BIGSPREADDIAMOND, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Celebrate_Fire/Inkerton_Gunshot_02_ShrinkCircle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT02_PTCL_SHRINKCIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Celebrate_Fire/Inkerton_Gunshot_03_InnerCircle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT03_PTCL_INNERCIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Celebrate_Fire/Inkerton_Gunshot_04_ExpandCircle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT04_PTCL_EXPANDCIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Celebrate_Fire/Inkerton_Gunshot_05_Circle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT05_PTCL_CIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Celebrate_Fire/Inkerton_Gunshot_06_ChargingLaser.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT06_MESH_CHARGINGLASER, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Celebrate_Fire/Inkerton_Gunshot_07_Laser.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT07_MESH_LASER, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Celebrate_Fire/Inkerton_Gunshot_08_LaserImpact.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT08_MESH_LASERIMPACT, LEVEL_STATIC, L"Prototype_Effect_InkertonIncrease", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Celebrate_Fire/Inkerton_Gunshot_09_Celebrate.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT09_PTCL_CELEBRATE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    //--- Combo1

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Combo_1/Inkerton_Mesh_00_MainBlade.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO1_MESH_MAINBLADE, LEVEL_STATIC, L"Prototype_Effect_InkertonIncrease", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Combo_1/Inkerton_Mesh_01_LightLineBlade.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO1_MESH_LIGHTBLADE, LEVEL_STATIC, L"Prototype_Effect_InkertonIncrease", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Combo_1/Inkerton_Mesh_02_DarkTrail.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO1_MESH_DARKBLADE, LEVEL_STATIC, L"Prototype_Effect_InkertonIncrease", 2, &MeshDesc)))
        return E_FAIL;

    //--- Combo2

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Combo_2_2/Inkerton_Mesh_00_MainBlade.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO2_MESH_MAINBLADE, LEVEL_STATIC, L"Prototype_Effect_InkertonIncrease", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Combo_2_2/Inkerton_Mesh_01_LightLineBlade.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO2_MESH_LIGHTBLADE, LEVEL_STATIC, L"Prototype_Effect_InkertonIncrease", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Combo_2_2/Inkerton_Mesh_02_DarkTrail.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO2_MESH_DARKBLADE, LEVEL_STATIC, L"Prototype_Effect_InkertonIncrease", 2, &MeshDesc)))
        return E_FAIL;

    //--- Combo3

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Combo_3/Inkerton_Mesh_00_MainBlade.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO3_MESH_MAINBLADE, LEVEL_STATIC, L"Prototype_Effect_InkertonIncrease", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Combo_3/Inkerton_Mesh_01_LightLineBlade.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO3_MESH_LIGHTBLADE, LEVEL_STATIC, L"Prototype_Effect_InkertonIncrease", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/Combo_3/Inkerton_Mesh_02_DarkTrail.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO3_MESH_DARKBLADE, LEVEL_STATIC, L"Prototype_Effect_InkertonIncrease", 2, &MeshDesc)))
        return E_FAIL;

    //--- NutCracker

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/NutCracker/Inkerton_Ptcl_00_Splash.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_NUT00_PTCL_SPLASH, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/NutCracker/Inkerton_Ptcl_01_SandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_NUT01_PTCL_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/NutCracker/Inkerton_Ptcl_02_Bubbles.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_NUT02_PTCL_BUBBLES, LEVEL_STATIC, L"Prototype_Effect_Change_ExplToRiseW", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/NutCracker/Inkerton_MeshInst_03_Pieces.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshInstEffect(hFile, MeshInstDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_NUT03_MESHINST_PIECES, LEVEL_STATIC, L"Prototype_Effect_MeshInst_Once", 4, &MeshInstDesc)))
        return E_FAIL;

    // 보스 : 미믹 이펙트
    //--- 차징
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Charge/Ptcl_Charge_Charging.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CHARGE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 8, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Charge/Ptcl_Charge_Hold.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_SPRT_CHARGE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 8, &SpriteDesc)))
        return E_FAIL;

    //--- 내려찍기

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Ptcl_Pounce.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_POUNCE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Ptcl_SandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    //--- 크리스탈 설치

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Crystal/Ptcl_Install_Smoke.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CRYSTAL_INSALL_SMOKE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 32, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Crystal/Ptcl_Install_SmokeFloor.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CRYSTAL_INSALL_SMOKEFLOOR, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 32, &SpriteDesc)))
        return E_FAIL;

    //--- 크리스탈 폭발

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Shards/Ptcl_PreExplode_Electric.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CRYSTAL_PREEXPLODE_ELECTRIC, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 32, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Shards/Ptcl_Explode_Ash.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CRYSTAL_EXPLODE_ASH, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 32, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Shards/Ptcl_Explode_Diamond.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CRYSTAL_EXPLODE_DIAMOND, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 32, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Shards/MeshInst_Explode_Crystal.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshInstEffect(hFile, MeshInstDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_MESHINST_CRYSTAL_EXPLODE_0, LEVEL_STATIC, L"Prototype_Effect_MeshInst_Once", 32, &MeshInstDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Shards/MeshInst_Explode_Crystal1.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshInstEffect(hFile, MeshInstDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_MESHINST_CRYSTAL_EXPLODE_1, LEVEL_STATIC, L"Prototype_Effect_MeshInst_Once", 32, &MeshInstDesc)))
        return E_FAIL;

    //--- 크리스탈 성장

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Ptcl_Increase_Electric.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CRYSTAL_INCREASE_ELECTRIC, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 32, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Ptcl_IncreaseMax_Electric.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CRYSTAL_INCREASE_MAX_ELECTRIC, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 32, &SpriteDesc)))
        return E_FAIL;

    //--- 롤링

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Rolling/Mesh_Rolling_Light.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_MESH_ROLLING_LIGHT, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Rolling/Mesh_Rolling_Line.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_MESH_ROLLING_LINE, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Rolling/Mesh_Rolling_Distortion.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_MESH_ROLLING_DISTORTION, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Rolling/Ptcl_Rolling_SandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_ROLLING_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 64, &SpriteDesc)))
        return E_FAIL;

    //--- 방어
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Petroch/Defend/Defend_Mesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshInstEffect(hFile, MeshInstDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PETROCH_MESH_DEFEND, LEVEL_STATIC, L"Prototype_Effect_MeshInst_Once", 8, &MeshInstDesc)))
        return E_FAIL;

    // 웜 이펙트
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Field/Worm/Ptcl_Expl_SandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::WORM_PTCL_SANDBURST_EXPL, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Field/Worm/Ptcl_ExplSlowDown_SandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::WORM_PTCL_SANDBURST_EXPLSLOWDOWN, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    // 갑오징어 이펙트
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/CrabTomb/CuttleFish/Projectile_Ptcl_Charge.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::CUTTLEFISH_PTCL_PROJ_CHARGE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/CrabTomb/CuttleFish/Projectile_MeshInst_Sphere.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshInstEffect(hFile, MeshInstDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::CUTTLEFISH_MESHINST_PROJ_SPHERE, LEVEL_STATIC, L"Prototype_Effect_MeshInst_Once", 16, &MeshInstDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/CrabTomb/CuttleFish/Projectile_Ptcl_Circle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::CUTTLEFISH_PTCL_PROJ_CIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 16, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/CrabTomb/CuttleFish/Projectile_Ptcl_Trail.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::CUTTLEFISH_PTCL_PROJ_TRAIL, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 160, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/CrabTomb/CuttleFish/Projectile_Ptcl_Explode.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::CUTTLEFISH_PTCL_PROJ_EXPLODE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 16, &SpriteDesc)))
        return E_FAIL;

    // Cuttlefish_Ptcl_Ink
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/CrabTomb/CuttleFish/Cuttlefish_Ptcl_Ink.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::CUTTLEFISH_PTCL_INK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG
HRESULT CLevel_CrabTomb::Ready_Layer_Debug(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_ObjectInfo", LEVEL_CRABTOMB, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_Character", LEVEL_CRABTOMB, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_Level", LEVEL_CRABTOMB, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_PostProcess", LEVEL_CRABTOMB, strLayerTag)))
        return E_FAIL;

    return S_OK;
}
HRESULT CLevel_CrabTomb::Ready_Layer_CameraTool(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer
    (
        LEVEL_STATIC,
        L"Prototype_GameObject_CameraTool",
        LEVEL_CRABTOMB,
        strLayerTag
    )))
        return E_FAIL;

    return S_OK;
}
#endif

void CLevel_CrabTomb::SetUp_CollisionGroup()
{
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::MONSTER, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::MONSTER_HIT, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::NPC, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::ITEM, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::MAP_INTERACTION, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::GRAPPLENEEDLE, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::SPONGE, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::MAGNET, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::URCHIN, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::HEARTKELP, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::SHELL, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::PORTAL, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::TRIGGER, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER_LOCKON, (_uint)COLLISION_GROUP::MONSTER, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER_WEAPON, (_uint)COLLISION_GROUP::MONSTER, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER_WEAPON, (_uint)COLLISION_GROUP::SHELL, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER_PROJECTILE, (_uint)COLLISION_GROUP::MONSTER, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER_GRAPPLE, (_uint)COLLISION_GROUP::GRAPPLENEEDLE, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER_GRAPPLE, (_uint)COLLISION_GROUP::MONSTER, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER_GRAPPLE, (_uint)COLLISION_GROUP::SHELL, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER_SKILL, (_uint)COLLISION_GROUP::MONSTER, true);

}

void CLevel_CrabTomb::ReadFile_MeshEffect(HANDLE hFile, CMeshEffect::DESC& Desc)
{
    _tchar szName[MAX_PATH]{};
    _uint iEffectType{}, iTextureIndex{}, iMeshType{};
    DWORD dwByte{};
    _bool bRead{};
    bRead = ReadFile(hFile, &iEffectType, sizeof(_int), &dwByte, nullptr);
    bRead = ReadFile(hFile, &iTextureIndex, sizeof(_uint), &dwByte, nullptr);
    bRead = ReadFile(hFile, &iMeshType, sizeof(_uint), &dwByte, nullptr);

    bRead = ReadFile(hFile, &Desc.vTranslation, sizeof(_float3), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.vScale, sizeof(_float3), &dwByte, nullptr);
    bRead = ReadFile(hFile, szName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
    Desc.strModelTag = szName;
    bRead = ReadFile(hFile, szName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
    Desc.strTextureTag = szName;

    bRead = ReadFile(hFile, &Desc.bIsLoop, sizeof(_bool), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.iMeshAnimOption, sizeof(_uint), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.fLifeTime, sizeof(_float), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.vCenter, sizeof(_float3), &dwByte, nullptr);

    bRead = ReadFile(hFile, &Desc.bUVAnim, sizeof(_bool), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.vUVStart, sizeof(_float2), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.vUVSpeed, sizeof(_float2), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.vUVTileCount, sizeof(_float2), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.fMaxTrailLength, sizeof(_float), &dwByte, nullptr);

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

void CLevel_CrabTomb::ReadFile_SpriteEffect(HANDLE hFile, CSpriteEffect::DESC& Desc)
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
    Desc.EffectDesc.bIsLoop = Desc.BufferDesc.ParticleDesc.isLoop;
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

void CLevel_CrabTomb::ReadFile_MeshInstEffect(HANDLE hFile, CMeshInstEffect::DESC& Desc)
{
    _tchar szName[MAX_PATH]{};
    _uint iEffectType{}, iTextureIndex{}, iMeshType{};
    DWORD dwByte{};
    _bool bRead{};

    bRead = ReadFile(hFile, &iEffectType, sizeof(_uint), &dwByte, nullptr);
    bRead = ReadFile(hFile, &iTextureIndex, sizeof(_uint), &dwByte, nullptr);
    bRead = ReadFile(hFile, &iMeshType, sizeof(_uint), &dwByte, nullptr);

    Desc.BufferDesc.eLevelID = LEVEL_STATIC;
    bRead = ReadFile(hFile, &Desc.vOriginLocation, sizeof(_float3), &dwByte, nullptr);
    bRead = ReadFile(hFile, szName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
    Desc.BufferDesc.strModelTag = szName;
    bRead = ReadFile(hFile, szName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
    Desc.strTextureTag = szName;

    bRead = ReadFile(hFile, &Desc.EffectDesc.bIsLoop, sizeof(_bool), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.fLifeTime, sizeof(_float), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.bUVAnim, sizeof(_bool), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.vUVStart, sizeof(_float2), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.vUVSpeed, sizeof(_float2), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.vUVTileCount, sizeof(_float2), &dwByte, nullptr);

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

    bRead = ReadFile(hFile, &Desc.BufferDesc.bIsLoop, sizeof(_bool), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.iPattern, sizeof(_int), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.iNumInstance, sizeof(_int), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.vRange, sizeof(_float3), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.vCenter, sizeof(_float3), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.vPivot, sizeof(_float3), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.vSpeed, sizeof(_float2), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.vLifeTime, sizeof(_float2), &dwByte, nullptr);

    bRead = ReadFile(hFile, &Desc.BufferDesc.vScalingSize, sizeof(_float3), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.fFinalSize, sizeof(_float), &dwByte, nullptr);

    bRead = ReadFile(hFile, &Desc.BufferDesc.iRotationAxis, sizeof(_int), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.fRotationSpeed, sizeof(_float), &dwByte, nullptr);
    bRead = ReadFile(hFile, &Desc.BufferDesc.fRotationFinalSpeed, sizeof(_float), &dwByte, nullptr);

    CloseHandle(hFile);
}

CLevel_CrabTomb* CLevel_CrabTomb::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new CLevel_CrabTomb(pDevice, pContext);
}

void CLevel_CrabTomb::Free()
{
    super::Free();
}
