#include "pch.h"
#include "Level_PinBall.h"

#include "GameInstance.h"
#include "NPC_Headers.h"

#include "Sky.h"
#include "WaterShadow.h"

#include "Camera_Free.h"
#include "Player.h"

#include "MapObject.h"

#include "Boss_Roland.h"

#include "MapTrigger.h"
#include "TriggerObserverTest.h"
#include "CallbackUpdate.h"
#include "Level_Loading.h"

#include "Shell_Item.h"
#include "UI_Manager.h"
#include "PlayerManager.h"

CLevel_PinBall::CLevel_PinBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

HRESULT CLevel_PinBall::Initialize()
{
    /* 맵 오브젝트 먼저 레벨에 추가하고, 그 다음 컨트롤러 넣어야 문제가 없음 */
    if (FAILED(m_pGameInstance->Setup_PhysicsScene(_vec2(-8.f, -8.f), _vec2(60.f, 105.f), 16.f)))
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

    /* Level_Initalize 동안의 누적된 타임델타 제거 */
    m_pGameInstance->Compute_TimeDelta(L"Timer_60");

    m_pGameInstance->Play_Sound_Loop(L"Water_Travel_Loop_1.wav", (_uint)SOUND_CHANNEL::AMBIENCE_LOOP, 0.f);

    return S_OK;
}

void CLevel_PinBall::Update(_float fTimeDelta)
{
}

HRESULT CLevel_PinBall::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"핀볼 레벨 입니다.");
#endif
    return S_OK;
}

HRESULT CLevel_PinBall::Ready_Layer_Environment(const _wstring& strLayerTag)
{
    CSky::DESC SkyDesc{};
    SkyDesc.iTextureIndex = 0;
    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_Sky", LEVELID::LEVEL_PINBALL, strLayerTag, &SkyDesc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
        return E_FAIL;

    ///* for.WaterShadow */
    //{
    //    CWaterShadow::DESC Desc {};
    //    Desc.GameObjectDesc.WorldMatrix = _mat::ScalingMatrix({ 0.5f, 0.5f, 0.5f })._float4x4();
    //    Desc.GameObjectDesc.strName = L"WaterShadow";
    //    Desc.CauticsParams.fIntensity = 0.05f;

    //    if (FAILED(m_pGameInstance->
    //        Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_WaterShadow", LEVEL_PINBALL, strLayerTag, &Desc)))
    //        return E_FAIL;
    //}
    return S_OK;
}

HRESULT CLevel_PinBall::Ready_Environment()
{
    {   /* 글로벌조명 + 쉐도우라이트 + 톤매핑 + 블룸 + 안개 */
        _wstring strFilePath = L"../Bin/DataFiles/Atmosphere/PinBall.atmosphere";

        HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (INVALID_HANDLE_VALUE != hFile)
        {
            DWORD dwByte{};
            _bool bResult{};

            LIGHT_DESC LightDesc{};
            COLORCORRECTION_PARAMS ColorCorrectionParams{};
            BLOOM_PARAMS BloomParams{};
            FOG_PARAMS FogParams{};

            bResult = ReadFile(hFile, &LightDesc, sizeof(LIGHT_DESC), &dwByte, nullptr);
            bResult = ReadFile(hFile, &ColorCorrectionParams, sizeof(COLORCORRECTION_PARAMS), &dwByte, nullptr);
            bResult = ReadFile(hFile, &BloomParams, sizeof(BLOOM_PARAMS), &dwByte, nullptr);
            bResult = ReadFile(hFile, &FogParams, sizeof(FOG_PARAMS), &dwByte, nullptr);

            m_pGameInstance->Add_Light(L"Light_Global", LightDesc);
            m_pGameInstance->SetUp_ShadowLight(_vec3(LightDesc.vDirection));
            m_pGameInstance->Set_ColorCorrectionParams(ColorCorrectionParams);
            m_pGameInstance->Set_BloomParams(BloomParams);
            m_pGameInstance->Set_FogParams(FogParams);
        }

        CloseHandle(hFile);
    }

    return S_OK;
}

HRESULT CLevel_PinBall::Ready_Layer_MapObject(const _wstring& strLayerTag)
{
    //맵오브젝트
    {
        _wstring szFile = L"../Bin/DataFiles/Map_Object/PinBall.environment";

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

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_MapObject", LEVELID::LEVEL_PINBALL, strLayerTag, &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
                return E_FAIL;
        }

        CloseHandle(hFile);
    }


    return S_OK;
}

HRESULT CLevel_PinBall::Ready_Layer_InteractionObject(const _wstring& strLayerTag)
{
    {
        _wstring szFile = L"../Bin/DataFiles/Map_InteractionObject/PinBall.interaction"; //다이얼로그로 가져오는 파일패스 저장할 곳
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

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_PINBALL, strName, LEVELID::LEVEL_PINBALL, strLayerTag, &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
                return E_FAIL;
        }

        CloseHandle(hFile);

    }
    return S_OK;
}

HRESULT CLevel_PinBall::Ready_Layer_MapTrigger(const _wstring& strLayerTag)
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_MapTrigger");
#endif
    {//여기서 트리거 파일 불러오깅 ㅠ
        _wstring szFile = L"../Bin/DataFiles/MapTrigger/PinBallTrigger.mapTrigger";
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
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapTrigger", LEVEL_PINBALL, L"Layer_MapTrigger", &Desc)))
            {
                MSG_BOX("트리거 추가 안댐.");
                break;
            }
        }

        CloseHandle(hFile);
    }


    /*트리거 세팅*/
    CMapTrigger* pTrigger = { nullptr };
    {
        pTrigger = static_cast<CMapTrigger*>(m_pGameInstance->Find_GameObject_With_Name(LEVEL_PINBALL, L"Layer_MapTrigger", L"Trigger_PinBallToClub"));
        //pTrigger->Add_CollisionEnter_Callback([pTrigger]()->void
        //    {
        //        CGameInstance::GetInstance()->Delete_GameObject(LEVEL_FIELD, L"Layer_MapTrigger", pTrigger);
        //    });

        pTrigger->Add_CollisionEnter_Callback([this]()->void
            {
                CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

                pUIManager->FadeOut();

                CCallbackUpdate* pCallbackUpdate = static_cast<CCallbackUpdate*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CallbackUpdate"));

                pCallbackUpdate->Add_Update_Callback([pUIManager, this](_float fElapsed, _float fTimeDelta)->_bool
                    {
                        _bool bFadeEnd = pUIManager->End_FadeOut();
                        if (bFadeEnd)
                            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CLUB));

                        return bFadeEnd;
                    });
            });
    }

    return S_OK;
}

HRESULT CLevel_PinBall::Ready_Layer_Projectile(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Projectile_Manager", LEVEL_PINBALL, strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_PinBall::Ready_Layer_Shell(const _wstring& strLayerTag)
{ 
    // 쉘 설치 
    {
        wstring szFile = L"../Bin/DataFiles/Map_Item/PinBall_Shell.item";
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

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Shell_World", LEVEL_PINBALL, strLayerTag, &tShellDesc)))
                return E_FAIL;
        }

        CloseHandle(hFile);
    }

    return S_OK;
}

HRESULT CLevel_PinBall::Ready_Layer_Camera(const _wstring& strLayerTag)
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
        Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"), LEVEL_PINBALL, strLayerTag, &CameraFreeDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_PinBall::Ready_Layer_CinematicCamera(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer
        (
            LEVEL_STATIC,
            TEXT("Prototype_GameObject_CinematicCamera"),
            LEVEL_PINBALL,
            strLayerTag
        )))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_PinBall::Ready_Layer_Player(const _wstring& strLayerTag)
{
    CPlayer::DESC PlayerDesc{};
    CPlayerManager::Save_Transition(XMMatrixRotationY(XMConvertToRadians(-166.09f)) * XMMatrixTranslation(25.87f, 41.73f, 48.99f));
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Player"), LEVEL_PINBALL, strLayerTag, &PlayerDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_PinBall::Ready_Layer_Monster(const _wstring& strLayerTag)
{
    CBoss_Roland::DESC RolandDesc{};
    {
        RolandDesc.EnemyDesc.enemyStats.fMaxHP = 1200.f;
        RolandDesc.EnemyDesc.enemyStats.fMaxBP = 300.f;
        RolandDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
        RolandDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
        RolandDesc.EnemyDesc.szName = L"벤처 투자게 홀란드";
        RolandDesc.EnemyDesc.GameObjectDesc.strName = L"Roland";

        XMStoreFloat4x4(&RolandDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(0.f, 0.f, 0.f));
        //XMStoreFloat4x4(&RolandDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(24.86f, 50.f, 12.28f));
        XMStoreFloat4x4(&RolandDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(24.86f, 50.f, 12.28f));
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_PINBALL, L"Prototype_GameObject_Boss_Roland", LEVEL_PINBALL, strLayerTag, &RolandDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_PinBall::Ready_Layer_NPC(const _wstring& strLayerTag)
{
    CNPC_Fighter::DESC Fighter{};
    {
        Fighter.szFileName = L"../Bin/DataFiles/NPC_Data/PinBall_Chitan.npc";
        Fighter.szFightFileName = L"../Bin/DataFiles/NPC_Data/Fight_Chitan.npc";

        Fighter.strModelTag = L"Prototype_Component_Model_NPC_Chitan";
        Fighter.strFightModelTag = L"Prototype_Component_Model_NPC_Chitan_Fight";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Fighter", LEVEL_PINBALL, strLayerTag, &Fighter)))
        return E_FAIL;

    {
        Fighter.szFileName = L"../Bin/DataFiles/NPC_Data/PinBall_Inkerton.npc";
        Fighter.szFightFileName = L"../Bin/DataFiles/NPC_Data/Fight_Inkerton.npc";

        Fighter.strModelTag = L"Prototype_Component_Model_NPC_Inkerton";
        Fighter.strFightModelTag = L"Prototype_Component_Model_NPC_Inkerton_Fight";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Fighter", LEVEL_PINBALL, strLayerTag, &Fighter)))
        return E_FAIL;

    CNPC_Roland::DESC RolandDesc{};
    {
        RolandDesc.szFileName = L"../Bin/DataFiles/NPC_Data/PinBall_Roland.npc";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Roland", LEVEL_PINBALL, strLayerTag, &RolandDesc)))
        return E_FAIL;

    CNPC_Roland_Death::DESC RolandDeathDesc{};
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Roland_Death", LEVEL_PINBALL, strLayerTag, &RolandDeathDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_PinBall::Ready_Layer_Manager(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_PINBALL, L"Prototype_GameObject_Enemy_Manager", LEVEL_PINBALL, strLayerTag, nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_PinBall::Ready_Layer_Effect(const _wstring& strLayerTag)
{
    CSpriteEffect::DESC SpriteDesc{};
    CMeshEffect::DESC   MeshDesc{};
    CMeshInstEffect::DESC MeshInstDesc{};

    // 환경 이펙트
    HANDLE hFile = CreateFile(L"../Bin/DataFiles/Effects/Bubbles.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_BUBBLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 16, &SpriteDesc)))
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

    // 디스토션
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Radial_Distortion.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, LEVEL_STATIC, L"Prototype_Effect_RadialDistortion", 16, &MeshDesc)))
        return E_FAIL;

    // 롤랜드 이펙트(펀치)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Punch_Seq00_Thwack_0.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_SPRT_PUNCH_SEQ00_0, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Punch_Seq00_Thwack_1.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_SPRT_PUNCH_SEQ00_1, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Punch_Seq00_Thwack_2.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_SPRT_PUNCH_SEQ00_2, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Punch_Seq00_Thwack_3.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_SPRT_PUNCH_SEQ00_3, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Punch_Seq01_DiamondGlow.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_SPRT_PUNCH_SEQ01, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Punch_Seq02_MiddleGlow.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_MESH_PUNCH_SEQ02, LEVEL_STATIC, L"Prototype_Effect_RolandIncrease", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Punch_Seq03_WindDome.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_MESH_PUNCH_SEQ03, LEVEL_STATIC, L"Prototype_Effect_RolandIncrease", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Punch_Seq04_CrossDots.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_SPRT_PUNCH_SEQ04, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Punch_Seq05_WimdImpact.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_MESH_PUNCH_SEQ05, LEVEL_STATIC, L"Prototype_Effect_RolandIncrease", 2, &MeshDesc)))
        return E_FAIL;


    // 롤랜드 이펙트(찌르기)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Thrust/Thrust_Seq00_WindMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_THRUST_MESH_SEQ00_WIND, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Thrust/Thrust_Seq01_DarkWindMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_THRUST_MESH_SEQ01_DARKWIND, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Thrust/Thrust_Seq02_ExplodeGlowMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_THRUST_MESH_SEQ02_EXGLOW, LEVEL_STATIC, L"Prototype_Effect_RolandIncrease", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Thrust/Thrust_Seq03_LinesPtcl.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_THRUST_PTCL_SEQ03_LINES, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Thrust/Thrust_Seq04_LongDotsPtcl.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_THRUST_PTCL_SEQ04_LONGDOTS, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Thrust/Thrust_Seq05_AfterWindMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_THRUST_MESH_SEQ05_AFTERWIND, LEVEL_STATIC, L"Prototype_Effect_RolandIncrease", 4, &MeshDesc)))
        return E_FAIL;

    // 롤랜드 이펙트(X자 베기)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Slash/Slash_LineMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_SLASH_MESH_LINE, LEVEL_STATIC, L"Prototype_Effect_RolandSlash", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Slash/Slash_DarkMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_SLASH_MESH_DARK, LEVEL_STATIC, L"Prototype_Effect_RolandSlash", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Slash/Slash_LightMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_SLASH_MESH_LIGHT, LEVEL_STATIC, L"Prototype_Effect_RolandSlash", 4, &MeshDesc)))
        return E_FAIL;

    // 롤랜드 이펙트(골프)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Golf/Golf_HitPtcl_Dark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_GOLFHIT_PTCL_DARK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Golf/Golf_HitPtcl_Light.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_GOLFHIT_PTCL_LIGHT, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Golf/Golf_HitPtcl_SandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_GOLFHIT_PTCL_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Golf/Golf_HitPtcl_Spark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_GOLFHIT_PTCL_SPARK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Golf/Golf_AttackMesh_Dark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_GOLF_MESH_DARK, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Golf/Golf_AttackMesh_Light.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_GOLF_MESH_LIGHT, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Golf/Golf_AttackMesh_Line.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_GOLF_MESH_LINE, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 2, &MeshDesc)))
        return E_FAIL;

    // 롤랜드 이펙트(내려찍기)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Wallop/Wallop_Mesh_Dark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_WALLOP_MESH_DARK, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Wallop/Wallop_Mesh_Light.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_WALLOP_MESH_LIGHT, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 4, &MeshDesc)))
        return E_FAIL;
    
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Wallop/Wallop_Ptcl_Dark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_WALLOP_PTCL_DARK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Wallop/Wallop_Ptcl_Light.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_WALLOP_PTCL_LIGHT, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Wallop/Wallop_Ptcl_SandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_WALLOP_PTCL_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    // 롤랜드 이펙트(재날림)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Roland_AshFlow.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_ASHFLOW, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    // 롤랜드 이펙트(빅콤보)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/BigCombo/BigCombo_LineMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_BIGCOMBO_MESH_LINE, LEVEL_STATIC, L"Prototype_Effect_RolandBigCombo", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/BigCombo/BigCombo_LightMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_BIGCOMBO_MESH_LIGHT, LEVEL_STATIC, L"Prototype_Effect_RolandBigCombo", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/BigCombo/BigCombo_DarkMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_BIGCOMBO_MESH_DARK, LEVEL_STATIC, L"Prototype_Effect_RolandBigCombo", 8, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/BigCombo/BigSwipe_LineMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_BIGSWIPE_MESH_LINE, LEVEL_STATIC, L"Prototype_Effect_RolandBigSwipe", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/BigCombo/BigSwipe_LightMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_BIGSWIPE_MESH_LIGHT, LEVEL_STATIC, L"Prototype_Effect_RolandBigSwipe", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/BigCombo/BigSwipe_DarkMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_BIGSWIPE_MESH_DARK, LEVEL_STATIC, L"Prototype_Effect_RolandBigSwipe", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/BigCombo/BigCombo_Small_RingMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_BIGSWIPE_MESH_SMALLRING, LEVEL_STATIC, L"Prototype_Effect_RolandBigSwipe", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/BigCombo/BigCombo_Big_RingMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_BIGSWIPE_MESH_BIGRING, LEVEL_STATIC, L"Prototype_Effect_RolandBigSwipe", 2, &MeshDesc)))
        return E_FAIL;

    // 롤랜드 이펙트(롤링)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Roll/Roll_MainMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_ROLL_MESH_MAIN, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Roll/Roll_SmokeWindMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_ROLL_MESH_SMOKEWIND, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 2, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Roland/Roll/Roll_WindMesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ROLAND_ROLL_MESH_WIND, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 2, &MeshDesc)))
        return E_FAIL;

    // 롤랜드 이펙트(드롭다운)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Splitter/Splitter_Guillotine_HitOverhead.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_SLAMSHOCK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Splitter/Splitter_Guillotine_HitOverhead_Sandburst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;


    // 잉커톤(NPC)
    // 먹물
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton_Squid_CutScene.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENEMY_SPRITE_SQUID, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 1, &SpriteDesc)))
        return E_FAIL;
    //--- Mixup Charging
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_08_Charging.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE00_PTCL_CHARGING, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 1, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_00_Charge.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE00_PTCL_CHARGE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 1, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_01_Circle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE01_PTCL_CIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 1, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_02_Flower.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE02_PTCL_FLOWER, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 1, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_03_Diamond.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE03_PTCL_DIAMOND, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 1, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_04_Circle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE04_PTCL_CIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 1, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_05_InnerCircle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE05_PTCL_INNERCIRCLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 1, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_06_ShrinkDiamond.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE06_PTCL_SHIRINKDIAMOND, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 1, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Inkerton/ChargeAttack/Inkerton_ChargingSeq_07_FixedDiamond.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE07_PTCL_FIXEDDIAMOND, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 1, &SpriteDesc)))
        return E_FAIL;

    // 컷씬 전투용
    hFile = CreateFile(L"../Bin/DataFiles/Effects/CutScene/Chitan/Ptcl_Damage_FractionSpark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::CUTSCENE_PTCL_CHITAN_FRACTIONSPARK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/CutScene/Chitan/Ptcl_Damage_ExplSlowDownSpark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::CUTSCENE_PTCL_CHITAN_EXPLSLOWDOWNSPARK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/CutScene/Chitan/Ptcl_Damage_DownLoop.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::CUTSCENE_PTCL_CHITAN_DOWNLOOP, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/CutScene/Chitan/Ptcl_Damage_UpLoop.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::CUTSCENE_PTCL_CHITAN_UPLOOP, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 4, &SpriteDesc)))
        return E_FAIL;

    return S_OK;
}


#ifdef _DEBUG
HRESULT CLevel_PinBall::Ready_Layer_Debug(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_ObjectInfo", LEVEL_PINBALL, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_Character", LEVEL_PINBALL, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_Level", LEVEL_PINBALL, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_PostProcess", LEVEL_PINBALL, strLayerTag)))
        return E_FAIL;

    return S_OK;
}
HRESULT CLevel_PinBall::Ready_Layer_CameraTool(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer
    (
        LEVEL_STATIC,
        L"Prototype_GameObject_CameraTool",
        LEVEL_PINBALL,
        strLayerTag
    )))
        return E_FAIL;

    return S_OK;
}
#endif

void CLevel_PinBall::SetUp_CollisionGroup()
{
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::MONSTER, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::MONSTER_HIT, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::NPC, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::ITEM, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::MAP_INTERACTION, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::GRAPPLENEEDLE, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::SPONGE, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::MAGNET, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::PINBALL, true);
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
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::MONSTER, (_uint)COLLISION_GROUP::PINBALL, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::PINBALL, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::NPC, (_uint)COLLISION_GROUP::PINBALL, true);
}

void CLevel_PinBall::ReadFile_MeshEffect(HANDLE hFile, CMeshEffect::DESC& Desc)
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

void CLevel_PinBall::ReadFile_SpriteEffect(HANDLE hFile, CSpriteEffect::DESC& Desc)
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

void CLevel_PinBall::ReadFile_MeshInstEffect(HANDLE hFile, CMeshInstEffect::DESC& Desc)
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

CLevel_PinBall* CLevel_PinBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new CLevel_PinBall(pDevice, pContext);
}

void CLevel_PinBall::Free()
{
    super::Free();
}
