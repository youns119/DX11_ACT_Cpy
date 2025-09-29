#include "pch.h"
#include "Level_Tutorial.h"

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
CLevel_Tutorial::CLevel_Tutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

HRESULT CLevel_Tutorial::Initialize()
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
    //m_pGameInstance->Play_Sound_Loop(L"CrabTomb_BGM.wav", (_uint)SOUND_CHANNEL::BGM, 0.5f);

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
        pUIManager->Show_TextUI(L"투토리얼........", _float4(235.f / 255.f, 215.f / 255.f, 254.f / 255.f, 1.f));
    }


    /* Level_Initalize 동안의 누적된 타임델타 제거 */
    m_pGameInstance->Compute_TimeDelta(L"Timer_60");

    return S_OK;
}

void CLevel_Tutorial::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Tutorial::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"게무덤 레벨 입니다.");
#endif
    return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_Environment(const _wstring& strLayerTag)
{
    CSky::DESC SkyDesc{};
    SkyDesc.iTextureIndex = 0;
    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_Sky", LEVELID::LEVEL_TUTORIAL, strLayerTag, &SkyDesc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
        return E_FAIL;

    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer(LEVELID::LEVEL_TUTORIAL, TEXT("Prototype_GameObject_Tutorial_Terrain"), LEVEL_TUTORIAL, strLayerTag)))
        return E_FAIL;

    /* for.WaterShadow */
    {
        CWaterShadow::DESC Desc{};
        Desc.GameObjectDesc.WorldMatrix = _mat::ScalingMatrix({ 0.5f, 0.5f, 0.5f })._float4x4();
        Desc.GameObjectDesc.strName = L"WaterShadow";
        Desc.CauticsParams.fIntensity = 0.4f;

        if (FAILED(m_pGameInstance->
            Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_WaterShadow", LEVEL_TUTORIAL, strLayerTag, &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Environment()
{
    THEME_DESC* pThemeDesc{ nullptr };
    vector<_wstring> ThemeFilePaths{ L"../Bin/DataFiles/Atmosphere/Village.atmosphere", };

    if (FAILED(THEME_DESC::Load_Themes(&pThemeDesc, ThemeFilePaths, 1)))
        return E_FAIL;
    pThemeDesc->Set_Atmosphere();
    Safe_Delete(pThemeDesc);


    return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_MapObject(const _wstring& strLayerTag)
{
    ////맵오브젝트
    //{
    //    _wstring szFile = L"../Bin/DataFiles/Map_Object/CrabTomb.environment";

    //    //로드하기
    //    HRESULT hr{};
    //    DWORD dwByte{};
    //    HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    //    if (INVALID_HANDLE_VALUE == hFile)
    //    {
    //        MSG_BOX("Can Not Open File");
    //        return E_FAIL;
    //    }

    //    while (true)
    //    {
    //        _wstring strName;
    //        strName.resize(MAX_PATH);
    //        _tchar szName[MAX_PATH] = {};

    //        _float4x4 WorldMat = {};// 월드 행렬
    //        _float fRange = {};

    //        _bool bRead = false;

    //        bRead = ReadFile(hFile, &szName, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);
    //        strName = szName;
    //        bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);
    //        bRead = ReadFile(hFile, &fRange, sizeof(_float), &dwByte, nullptr);

    //        _float4 vBaseColor = { 1.f, 1.f, 1.f, 1.f };
    //        bRead = ReadFile(hFile, &vBaseColor, sizeof(_float4), &dwByte, nullptr);

    //        _uint iPhysicState = {};
    //        bRead = ReadFile(hFile, &iPhysicState, sizeof(_uint), &dwByte, nullptr);

    //        CMapObject::DESC Desc = {};
    //        Desc.GameObjectDesc.strName = strName;
    //        Desc.GameObjectDesc.WorldMatrix = WorldMat;
    //        Desc.GameObjectDesc.fSpeedPerSec = fRange;
    //        Desc.ePhysicsType = iPhysicState;

    //        if (!bRead || 0 == dwByte)
    //            break;

    //        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_MapObject", LEVELID::LEVEL_TUTORIAL, strLayerTag, &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
    //            return E_FAIL;
    //    }

    //    CloseHandle(hFile);
    //}


    return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_InteractionObject(const _wstring& strLayerTag)
{
    return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_MapTrigger(const _wstring& strLayerTag)
{
    return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_Projectile(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Projectile_Manager", LEVEL_TUTORIAL, strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_Shell(const _wstring& strLayerTag)
{
    //{   // 쉘 설치 
    //    wstring szFile = L"../Bin/DataFiles/Map_Item/CrabTomb_Shell.item";
    //    _bool bRead = false;
    //    HRESULT hr{};
    //    DWORD dwByte{};
    //    HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    //    if (INVALID_HANDLE_VALUE == hFile)
    //    {
    //        MSG_BOX("Can Not Open File");
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }

    //    while (true)
    //    {
    //        _int iType = {};
    //        bRead = ReadFile(hFile, &iType, sizeof(_int), &dwByte, nullptr);

    //        _float4x4 WorldMat = {};
    //        bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);

    //        if (!bRead || dwByte == 0)
    //            break;

    //        CShell_Item::DESC tShellDesc = {};
    //        tShellDesc.eShell = (SHELL)iType;
    //        tShellDesc.ItemDesc.GameObjectDesc.WorldMatrix = WorldMat;

    //        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Shell_World", LEVEL_TUTORIAL, strLayerTag, &tShellDesc)))
    //            return E_FAIL;
    //    }

    //    CloseHandle(hFile);
    //}

    return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_Camera(const _wstring& strLayerTag)
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
        Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"), LEVEL_TUTORIAL, strLayerTag, &CameraFreeDesc)))
        return E_FAIL;

    return S_OK;

}

HRESULT CLevel_Tutorial::Ready_Layer_CinematicCamera(const _wstring& strLayerTag)
{
   /* if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer
        (
            LEVEL_STATIC,
            TEXT("Prototype_GameObject_CinematicCamera"),
            LEVEL_TUTORIAL,
            strLayerTag
        )))
        return E_FAIL;*/

    return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_Player(const _wstring& strLayerTag)
{
    CPlayer::DESC PlayerDesc{};
    XMStoreFloat4x4(&PlayerDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(100.f, 5.f, 100.f));
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Player"), LEVEL_TUTORIAL, strLayerTag, &PlayerDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_Monster(const _wstring& strLayerTag)
{
    //{
    //    CEnemy_Cuttlefish::DESC CuttlefishDesc{};
    //    CuttlefishDesc.EnemyDesc.enemyStats.fMaxHP = 300.f;
    //    CuttlefishDesc.EnemyDesc.enemyStats.fMaxBP = 60.f;
    //    CuttlefishDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
    //    CuttlefishDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;

    //    XMStoreFloat4x4(&CuttlefishDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationRollPitchYaw(0.f, -30.f, 0.f) * XMMatrixTranslation(123.f, 0.2f, 95.f));
    //    XMStoreFloat4x4(&CuttlefishDesc.EnemyDesc.OriginalPos, XMMatrixRotationRollPitchYaw(0.f, -30.f, 0.f) * XMMatrixTranslation(123.f, 0.2f, 95.f));

    //    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_TUTORIAL, L"Prototype_GameObject_Enemy_Cuttlefish", LEVEL_TUTORIAL, strLayerTag, &CuttlefishDesc)))
    //        return E_FAIL;
    //}

    //{
    //    CEnemy_BobbitWorm::DESC BobbitWormDesc{};
    //    BobbitWormDesc.EnemyDesc.enemyStats.fMaxHP = 30.f;
    //    BobbitWormDesc.EnemyDesc.enemyStats.fMaxBP = 30.f;
    //    BobbitWormDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
    //    BobbitWormDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;

    //    XMStoreFloat4x4(&BobbitWormDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationRollPitchYaw(0.f, -90.f, 0.f) * XMMatrixTranslation(88.20f, 1.f, 108.f));
    //    XMStoreFloat4x4(&BobbitWormDesc.EnemyDesc.OriginalPos, XMMatrixRotationRollPitchYaw(0.f, -90.f, 0.f) * XMMatrixTranslation(88.20f, 1.f, 108.f));

    //    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_TUTORIAL, L"Prototype_GameObject_Enemy_BobbitWorm", LEVEL_TUTORIAL, strLayerTag, &BobbitWormDesc)))
    //        return E_FAIL;
    //}

    return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_NPC(const _wstring& strLayerTag)
{
    return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_Manager(const _wstring& strLayerTag)
{
    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_TUTORIAL, L"Prototype_GameObject_Enemy_Manager", LEVEL_TUTORIAL, strLayerTag, nullptr)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_Effect(const _wstring& strLayerTag)
{
    CSpriteEffect::DESC SpriteDesc{};
    CMeshEffect::DESC   MeshDesc{};
    CMeshInstEffect::DESC MeshInstDesc{};

    // 환경 이펙트
    HANDLE hFile = CreateFile(L"../Bin/DataFiles/Effects/Bubbles.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_BUBBLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 16, &SpriteDesc)))
        return E_FAIL;

    //hFile = CreateFile(L"../Bin/DataFiles/Effects/Env_GlowInsects.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    //ReadFile_SpriteEffect(hFile, SpriteDesc);
    //if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_GLOWINSECTS, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 1, &SpriteDesc)))
    //    return E_FAIL;

    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_GLOWINSECTS, XMVectorSet(128.f, 0.f, 128.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), false, nullptr);

    //// ENV_MAPTRIGGER_INK
    //hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/MapTrigger_Ink.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    //ReadFile_SpriteEffect(hFile, SpriteDesc);
    //if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_MAPTRIGGER_INK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 1, &SpriteDesc)))
    //    return E_FAIL;

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
HRESULT CLevel_Tutorial::Ready_Layer_Debug(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_ObjectInfo", LEVEL_TUTORIAL, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_Character", LEVEL_TUTORIAL, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_Level", LEVEL_TUTORIAL, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_PostProcess", LEVEL_TUTORIAL, strLayerTag)))
        return E_FAIL;

    return S_OK;
}
HRESULT CLevel_Tutorial::Ready_Layer_CameraTool(const _wstring& strLayerTag)
{
    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer
    //(
    //    LEVEL_STATIC,
    //    L"Prototype_GameObject_CameraTool",
    //    LEVEL_TUTORIAL,
    //    strLayerTag
    //)))
    //    return E_FAIL;

    return S_OK;
}
#endif

void CLevel_Tutorial::SetUp_CollisionGroup()
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

void CLevel_Tutorial::ReadFile_MeshEffect(HANDLE hFile, CMeshEffect::DESC& Desc)
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

void CLevel_Tutorial::ReadFile_SpriteEffect(HANDLE hFile, CSpriteEffect::DESC& Desc)
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

void CLevel_Tutorial::ReadFile_MeshInstEffect(HANDLE hFile, CMeshInstEffect::DESC& Desc)
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

CLevel_Tutorial* CLevel_Tutorial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new CLevel_Tutorial(pDevice, pContext);
}

void CLevel_Tutorial::Free()
{
    super::Free();
}
