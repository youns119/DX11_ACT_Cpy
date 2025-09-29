#include "pch.h"
#include "Level_Village.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Camera_Free.h"
#include "Sky.h"
#include "Theme_Manager.h"
#include "Visit_Manger.h"
#include "Light_Player.h"

#include "Terrain.h"
#include "WaterShadow.h"
#include "Player.h"
#include "Dummy_NonAnim.h"
#include "Dummy_Anim.h"
#include "Boss_Petroch.h"
#include "Boss_Roland.h"
#include "Boss_Inkerton.h"
#include "Enemy_Sardine.h"
#include "Enemy_Bowman.h"
#include "Enemy_Cuttlefish.h"
#include "Enemy_Frogfish.h"
#include "Enemy_Splitter.h"
#include "Enemy_Bobbitworm.h"

#include "MapTrigger.h"
#include "CallbackUpdate.h"

/*minjee*/
#include "MapObject.h"

// 임시
#include "Item_Trash.h"
#include "Shell_Item.h"
#include "Remains.h"

#include "UI_Manager.h"
#include "NPC_Headers.h"

#include "FileData_Locator.h"
#include "Service_Dialogue.h"

#include "PlayerManager.h"

//_bool CLevel_Village::s_First = true;

CLevel_Village::CLevel_Village(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Village::Initialize()
{
    /* 맵 오브젝트 먼저 레벨에 추가하고, 그 다음 컨트롤러 넣어야 문제가 없음 */
    if (FAILED(m_pGameInstance->Setup_PhysicsScene(_vec2(-16.f, -16.f), _vec2(512.f, 512.f), 8.f)))
        return E_FAIL;

    if (FAILED(Ready_Environment()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Environment(TEXT("Layer_Environment"))))
        return E_FAIL;


    //if (s_First)
    //{
    //    m_pGameInstance->Play_Sound(L"Splash_Ambience.wav", (_uint)SOUND_CHANNEL::, 0.2f);
    //    s_First = false;
    //}

    if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_MapObject(TEXT("Layer_MapObject"))))
        return E_FAIL;

    m_pGameInstance->Play_Sound_Loop(L"Water_Travel_Loop_1.wav", (_uint)SOUND_CHANNEL::AMBIENCE_LOOP, 0.08f);

    m_pGameInstance->Play_Sound_Loop(L"Open_Ocean_BGM.wav", (_uint)SOUND_CHANNEL::BGM, 0.4f);

    if (FAILED(Ready_Layer_InteractionObject(TEXT("Layer_InteractionObject"))))
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

    if (FAILED(Ready_Layer_Lights(L"Layer_Lights")))
        return E_FAIL;

#ifdef _DEBUG
    if (FAILED(Ready_Layer_Debug(TEXT("Layer_Debug"))))
        return E_FAIL;
    if (FAILED(Ready_Layer_CameraTool(TEXT("Layer_CameraTool"))))
        return E_FAIL;
#endif // _DEBUG

    m_pGameInstance->Add_Layer_To_Lag(TEXT("Layer_Projectile"));
    m_pGameInstance->Add_Layer_To_Lag(TEXT("Layer_Player"));
    m_pGameInstance->Add_Layer_To_Lag(TEXT("Layer_Camera"));
    m_pGameInstance->Add_Layer_To_Lag(TEXT("Layer_Monster"));
    m_pGameInstance->Add_Layer_To_Lag(TEXT("Layer_Shell"));
    m_pGameInstance->Add_Layer_To_Lag(TEXT("Layer_NPC"));

    SetUp_CollisionGroup();


#ifdef _DEBUG
    SetWindowText(g_hWnd, L"UI_Manager");
#endif

    CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    pUIManager->Add_PartObject_For_Play();
    pUIManager->FadeIn();

    if (false == CVisit_Manger::Is_Visit_FromStatic(L"village")) 
    {
		pUIManager->Show_TextUI(L"뉴 카르시니아", _float4(235.f / 255.f, 215.f / 255.f, 254.f / 255.f, 1.f));
    }


    /* Level_Initalize 동안의 누적된 타임델타 제거 */
    m_pGameInstance->Compute_TimeDelta(L"Timer_60");


    return S_OK;
}

void CLevel_Village::Update(_float fTimeDelta)
{
    /* 예시 코드임 이제 리스너는 플레이어 위치가 될거고 플레이어 업데이트에서 해주면 좋을 듯 */
    /* 아래 보시고해보세묘 */

    ///* player update */
    //m_pGameInstance->Set_ListenerPositoin(_vec3(m_pGameInstance->Get_CamPosition()));

    ///* pipe */
    //m_pGameInstance->Set_ChannelVolume_With_SpatialDesc((_uint)SOUND_CHANNEL::BGM, _vec3(0.f, 0.f, 0.f), _vec2(1.f, 256.f), _vec2(0.f, 1.f));
}

HRESULT CLevel_Village::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"뉴 카르시니아");
#endif

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Environment(const _wstring& strLayerTag)
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_Environment");
#endif

    CSky::DESC SkyDesc {};
    SkyDesc.iTextureIndex = 0;
    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_Sky", LEVELID::LEVEL_VILLAGE, strLayerTag, &SkyDesc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
        return E_FAIL;

    //if (FAILED(m_pGameInstance->
    //    Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Terrain"), LEVEL_VILLAGE, strLayerTag)))
    //    return E_FAIL;

	/* for.WaterShadow */
    {
        CWaterShadow::DESC Desc {};
        Desc.GameObjectDesc.WorldMatrix = _mat::ScalingMatrix({ 0.5f, 0.5f, 0.5f })._float4x4();
        Desc.GameObjectDesc.strName = L"WaterShadow";

        if (FAILED(m_pGameInstance->
            Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_WaterShadow", LEVEL_VILLAGE, strLayerTag, &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_MapObject(const _wstring& strLayerTag)
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_MapObject");
#endif
    //임시
    {
        _wstring szFile = L"../Bin/DataFiles/Map_Object/Village.environment"; //다이얼로그로 가져오는 파일패스 저장할 곳

        //로드하기
        HRESULT hr {};
        DWORD dwByte {};
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


            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_MapObject", LEVELID::LEVEL_VILLAGE, strLayerTag, &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
                return E_FAIL;

            _uint iObjListSize = (_uint)(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), strLayerTag)).size();
            CGameObject* pNewGameObject = m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), strLayerTag, iObjListSize - 1);
            if (!(fabs(vBaseColor.x - 1.f) < 0.01f &&
                fabs(vBaseColor.y - 1.f) < 0.01f &&
                fabs(vBaseColor.z - 1.f) < 0.01f &&
                fabs(vBaseColor.w - 1.f) < 0.01f))
            {
                static_cast<CMapObject*>(pNewGameObject)->Set_BaseColor(vBaseColor);
                CModel* pModel = static_cast<CModel*>(pNewGameObject->Find_Component(L"Com_Model"));

                for (_uint i = 0; i < (_uint)pModel->Get_NumMeshes(); i++) // 모델에 있는 메쉬 개수 만큼 돌아여
                {
                    _string strName = pModel->Get_MeshName(i);
                    if (strName == "Cylinder.001"
                        || strName == "gin_01_LOD1.002" || strName == "gin_01_LOD2.002"
                        || strName == "CoralTree_05_LOD0_A" || strName == "CoralTree_05_LOD1_A" || strName == "CoralTree_05_LOD2_A"
                        || strName == "CoralTree_05_LOD0_A.001" || strName == "CoralTree_05_LOD1_A.001"
                        || strName == "CoralTree_04_LOD0_A" || strName == "CoralTree_04_LOD1_A"
                        || strName == "CoralTree_04_LOD0_A.001" || strName == "CoralTree_04_LOD1_A.001"
                        || strName == "CoralTree_02_LOD0_A" || strName == "CoralTree_02_LOD1_A"
                        || strName == "CoralTree_02_LOD0_A.001" || strName == "CoralTree_02_LOD1_A.001"
                        || strName == "CoralTree_03_LOD0_A.001" || strName == "CoralTree_03_LOD1_A.001"
                        || strName == "CoralTree_03_LOD0_A.002" || strName == "CoralTree_03_LOD1_A.002"
                        || strName == "ShelfCoral_01_LOD0_A"
                        || strName == "ShelfCoral_02_LOD0_A"
                        || strName == "ShelfCoral_04_LOD0_A" || strName == "ShelfCoral_04_LOD1_A"
                        || strName == "ShelfCoral_05_LOD0_A" || strName == "ShelfCoral_05_LOD0_A.001"
                        || strName == "ShelfCoral_03_LOD0_A" || strName == "ShelfCoral_03_LOD0_A.003"
                        || strName == "ShelfCoral_03_LOD0_A.001"
                        || strName == "ShelfCoral_03_LOD0_A.002" || strName == "ShelfCoral_03_LOD1_A.002"
                        || strName == "CoralTree_02_LOD0_A.002" || strName == "CoralTree_02_LOD1_A.001"
                        || strName == "WormCoral_LOD0")
                    {
                        //m_iCurMeshIdx = i; // 동일하면 인덱스 넣어줘여
                        _uint iCurMaterialIdx = pModel->Get_MaterialIndex_In_Mesh(i); //메쉬안에 메테리얼 인덱스찾어
                        pModel->Set_BaseColor(iCurMaterialIdx, vBaseColor);
                        break;
                    }
                }
            }
        }
        CloseHandle(hFile);
    }

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_InteractionObject(const _wstring& strLayerTag)
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_InteractionObject");
#endif
    {
        _wstring szFile = L"../Bin/DataFiles/Map_InteractionObject/SheleportID_Village.interaction"; //다이얼로그로 가져오는 파일패스 저장할 곳
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

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_ShellHouse", LEVELID::LEVEL_VILLAGE, L"Layer_Insteraction_Environment", &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
                return E_FAIL;
        }

        CloseHandle(hFile);

    }

    {
        _wstring szFile = L"../Bin/DataFiles/Map_InteractionObject/Village.interaction"; //다이얼로그로 가져오는 파일패스 저장할 곳
        /*다이얼로그 띄우기*/

        HRESULT hr {};
        DWORD dwByte {};
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

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, strName, LEVELID::LEVEL_VILLAGE, strLayerTag, &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
                return E_FAIL;
        }

        CloseHandle(hFile);

    }

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_MapTrigger(const _wstring& strLayerTag)
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_MapTrigger");
#endif
    {//여기서 트리거 파일 불러오깅 ㅠ
        _wstring szFile = L"../Bin/DataFiles/MapTrigger/VillageTrigger.mapTrigger";
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
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapTrigger", LEVEL_VILLAGE, L"Layer_MapTrigger", &Desc)))
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
        pTrigger = static_cast<CMapTrigger*>(m_pGameInstance->Find_GameObject_With_Name(LEVEL_VILLAGE, L"Layer_MapTrigger", L"Trigger_VillageToField"));

        pTrigger->Add_CollisionEnter_Callback([this]()->void
            {
                CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

                pUIManager->FadeOut();

                CCallbackUpdate* pCallbackUpdate = static_cast<CCallbackUpdate*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CallbackUpdate"));

                pCallbackUpdate->Add_Update_Callback([pUIManager, this](_float fElapsed, _float fTimeDelta)->_bool
                    {
                        _bool bFadeEnd = pUIManager->End_FadeOut();
                        if (bFadeEnd)
                        {
                            CPlayerManager::Save_Transition(XMMatrixRotationY(XMConvertToRadians(100.f)) * XMMatrixTranslation(121.418f, 10.f, 348.692f));
                            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIELD));
                        }

                        return bFadeEnd;
                    });
            });
    }

	/* Theme */
    CMapTrigger::Create_InversedMapTrigger(LEVEL_VILLAGE, L"Trigger_SetThemeToField", L"Trigger_SetThemeToFieldInverse", _vec3(0.f, 0.f, -5.f));

    CMapTrigger::Resist_MapTrigger_ThemeChanger(LEVEL_VILLAGE, L"Trigger_JumpToBridge", m_Themes[(_uint)THEME::BRIDGE], 2.f);
    CMapTrigger::Resist_MapTrigger_ThemeChanger(LEVEL_VILLAGE, L"Trigger_SetThemeToVillage", m_Themes[(_uint)THEME::VILLAGE], 6.f);
    CMapTrigger::Resist_MapTrigger_ThemeChanger(LEVEL_VILLAGE, L"Trigger_SetThemeToField", m_Themes[(_uint)THEME::FIELD], 2.f);
    CMapTrigger::Resist_MapTrigger_ThemeChanger(LEVEL_VILLAGE, L"Trigger_SetThemeToFieldInverse", m_Themes[(_uint)THEME::VILLAGE], 2.f);

    CMapTrigger::Resist_MapTrigger_CallingEffect(LEVEL_VILLAGE, L"Trigger_SetThemeToVillage", (_uint)EFFECT_TYPE::ENV_GODRAY, XMVectorSet(160.f, 25.f, 220.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f));

    CMapTrigger::Resist_MapTrigger_CallingEffect(LEVEL_VILLAGE, L"Trigger_SetThemeToVillage", (_uint)EFFECT_TYPE::ENV_BUTTERFLY_FISH, XMVectorSet(104.51, 75.26f, 265.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f));
    CMapTrigger::Resist_MapTrigger_CallingEffect(LEVEL_VILLAGE, L"Trigger_SetThemeToVillage", (_uint)EFFECT_TYPE::ENV_BUTTERFLY_FISH, XMVectorSet(-6.51, 127.26f, 252.f, 1.f), XMVectorSet(1.f, 0.25f, -0.5f, 0.f));
    CMapTrigger::Resist_MapTrigger_CallingEffect(LEVEL_VILLAGE, L"Trigger_SetThemeToVillage", (_uint)EFFECT_TYPE::ENV_BUTTERFLY_FISH, XMVectorSet(208.51, 115.26f, 289.f, 1.f), XMVectorSet(-1.f, 0.1f, 0.f, 0.f));
    CMapTrigger::Resist_MapTrigger_CallingEffect(LEVEL_VILLAGE, L"Trigger_SetThemeToVillage", (_uint)EFFECT_TYPE::ENV_BUTTERFLY_FISH, XMVectorSet(110.51, 100.26f, 125.f, 1.f), XMVectorSet(0.f, -0.5f, -1.f, 0.f));
    CMapTrigger::Resist_MapTrigger_CallingEffect(LEVEL_VILLAGE, L"Trigger_SetThemeToVillage", (_uint)EFFECT_TYPE::ENV_SURGEON_FISH, XMVectorSet(125.39f, -157.4f, 191.91f, 1.f), XMVectorSet(0.f, 0.5f, 1.f, 0.f));
    CMapTrigger::Resist_MapTrigger_CallingEffect(LEVEL_VILLAGE, L"Trigger_SetThemeToVillage", (_uint)EFFECT_TYPE::ENV_SURGEON_FISH, XMVectorSet(170.39f, -65.4f, 221.91f, 1.f), XMVectorSet(1.f, 0.1f, 0.f, 0.f));
    CMapTrigger::Resist_MapTrigger_CallingEffect(LEVEL_VILLAGE, L"Trigger_SetThemeToVillage", (_uint)EFFECT_TYPE::ENV_SURGEON_FISH, XMVectorSet(132.39f, -87.4f, 186.91f, 1.f), XMVectorSet(-0.5f, -0.4f, 0.f, 0.f));
    CMapTrigger::Resist_MapTrigger_CallingEffect(LEVEL_VILLAGE, L"Trigger_SetThemeToVillage", (_uint)EFFECT_TYPE::ENV_SURGEON_FISH, XMVectorSet(192.84, -107.4f, 171.91f, 1.f), XMVectorSet(0.f, -0.1f, -1.f, 0.f));

    CMapTrigger::Resist_MapTrigger_SoundChanger(LEVEL_VILLAGE, L"Trigger_SetThemeToVillage", L"Village_BGM.wav", 1.f, 0.5f);
    CMapTrigger::Resist_MapTrigger_SoundChanger(LEVEL_VILLAGE, L"Trigger_SetThemeToField", L"Field_BGM.wav", 1.f, 0.5f);
    CMapTrigger::Resist_MapTrigger_SoundChanger(LEVEL_VILLAGE, L"Trigger_SetThemeToFieldInverse", L"Village_BGM.wav", 1.f, 0.5f);

    return S_OK;

}

HRESULT CLevel_Village::Ready_Layer_Projectile(const _wstring& strLayerTag)
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_Projectile");
#endif
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Projectile_Manager", LEVEL_VILLAGE, strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Shell(const _wstring& strLayerTag)
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_Shell");
#endif

    // 쉘 설치 
    {
        wstring szFile = L"../Bin/DataFiles/Map_Item/Village_Shell.item";
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
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Shell_World", LEVEL_VILLAGE, strLayerTag, &tShellDesc)))
                return E_FAIL;
        }

        CloseHandle(hFile);
    }


    //// MirrorBall 등껍질 Test용
    //CShell_Item::DESC tShellDesc{};
    //XMStoreFloat4x4(&tShellDesc.ItemDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(110.f, 15.f, -120.f));
    //tShellDesc.eShell = SHELL::SHELL_MIRRORBALL;
    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Shell_World"), LEVEL_VILLAGE, strLayerTag, &tShellDesc)))
    //    return E_FAIL;

    //// TennisBall 등껍질 Test용
    //XMStoreFloat4x4(&tShellDesc.ItemDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(110.f, 15.f, -130.f));
    //tShellDesc.eShell = SHELL::SHELL_TENNISBALL;
    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Shell_World"), LEVEL_VILLAGE, strLayerTag, &tShellDesc)))
    //    return E_FAIL;

    //// SodaCan 등껍질 Test용
    //XMStoreFloat4x4(&tShellDesc.ItemDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(120.f, 15.f, -120.f));
    //tShellDesc.eShell = SHELL::SHELL_SODACAN;
    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Shell_World"), LEVEL_VILLAGE, strLayerTag, &tShellDesc)))
    //    return E_FAIL;

    //// Duck 등껍질 Test용
    //XMStoreFloat4x4(&tShellDesc.ItemDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(120.f, 15.f, -130.f));
    //tShellDesc.eShell = SHELL::SHELL_DUCK;
    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Shell_World"), LEVEL_VILLAGE, strLayerTag, &tShellDesc)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Camera(const _wstring& strLayerTag)
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_Camera");
#endif
    CCamera_Free::DESC CameraFreeDesc {};
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
        Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"), LEVEL_VILLAGE, strLayerTag, &CameraFreeDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_CinematicCamera(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer
        (
            LEVEL_STATIC, 
            TEXT("Prototype_GameObject_CinematicCamera"), 
            LEVEL_VILLAGE, 
            strLayerTag
        )))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Player(const _wstring& strLayerTag)
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_Player");
#endif
    CPlayer::DESC PlayerDesc {};
    XMStoreFloat4x4(&PlayerDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(102.43f, 26.21f, -387.13f));
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Player"), LEVEL_VILLAGE, strLayerTag, &PlayerDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Monster(const _wstring& strLayerTag)
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_Monster");
#endif

    //CEnemy_Bowman::DESC BowmanDesc {};
     //{
     //    BowmanDesc.EnemyDesc.enemyStats.fMaxHP = 50.f;
     //    BowmanDesc.EnemyDesc.enemyStats.fMaxBP = 10.f;
     //    BowmanDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
     //    BowmanDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
     //    XMStoreFloat4x4(&BowmanDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(118.46f, 5.f, -72.21f));
     //    XMStoreFloat4x4(&BowmanDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(118.46f, 5.f, -72.21f));
     //}
     //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_VILLAGE, L"Prototype_GameObject_Enemy_Bowman", LEVEL_VILLAGE, strLayerTag, &BowmanDesc)))
     //    return E_FAIL;

    //CEnemy_Cuttlefish::DESC CuttlefishDesc{};
    //{
    //    CuttlefishDesc.EnemyDesc.enemyStats.fMaxHP = 30.f;
    //    CuttlefishDesc.EnemyDesc.enemyStats.fMaxBP = 30.f;
    //    CuttlefishDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
    //    CuttlefishDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;

    //    XMStoreFloat4x4(&CuttlefishDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(118.46f, 5.f, -82.21f));
    //    XMStoreFloat4x4(&CuttlefishDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(118.46f, 5.f, -82.21f));
    //}
    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_VILLAGE, L"Prototype_GameObject_Enemy_Cuttlefish", LEVEL_VILLAGE, strLayerTag, &CuttlefishDesc)))
    //    return E_FAIL;

    //CEnemy_Frogfish::DESC FrogfishDesc{};
    //{
    //    FrogfishDesc.EnemyDesc.enemyStats.fMaxHP = 50.f;
    //    FrogfishDesc.EnemyDesc.enemyStats.fMaxBP = 10.f;
    //    FrogfishDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
    //    FrogfishDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
    //    XMStoreFloat4x4(&FrogfishDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(118.46f, 5.f, -72.21f));
    //    XMStoreFloat4x4(&FrogfishDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(118.46f, 5.f, -72.21f));
    //}

    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_VILLAGE, L"Prototype_GameObject_Enemy_Frogfish", LEVEL_VILLAGE, strLayerTag, &FrogfishDesc)))
    //    return E_FAIL;

    //{
    //    CEnemy_Frogfish::DESC FrogfishDesc{};
    //    FrogfishDesc.EnemyDesc.enemyStats.fMaxHP = 50.f;
    //    FrogfishDesc.EnemyDesc.enemyStats.fMaxBP = 10.f;
    //    FrogfishDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
    //    FrogfishDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
    //    XMStoreFloat4x4(&FrogfishDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(118.46f, 5.f, -72.21f));
    //    XMStoreFloat4x4(&FrogfishDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(118.46f, 5.f, -72.21f));
    //    vector<_vector> PatrolPos{};
    //    PatrolPos.push_back(XMVectorSet(501.f, 1.f, 336.f, 1.f));
    //    FrogfishDesc.EnemyDesc.PatrolPoints = PatrolPos;
    //    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_VILLAGE, L"Prototype_GameObject_Enemy_Sardine", LEVEL_VILLAGE, strLayerTag, &FrogfishDesc)))
    //        return E_FAIL;
    //}

    //CEnemy_Splitter::DESC SpliterDesc{};
    //{
    //    SpliterDesc.EnemyDesc.enemyStats.fMaxHP = 100.f;
    //    SpliterDesc.EnemyDesc.enemyStats.fMaxBP = 50.f;
    //    SpliterDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
    //    SpliterDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;

    //    XMStoreFloat4x4(&SpliterDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(123.46f, 5.f, -62.21f));
    //    XMStoreFloat4x4(&SpliterDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(123.46f, 5.f, -72.21f));
    //}
    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_VILLAGE, L"Prototype_GameObject_Enemy_Splitter", LEVEL_VILLAGE, strLayerTag, &SpliterDesc)))
    //    return E_FAIL;

 /*   CBoss_Inkerton::Inkerton_DESC InkertonDesc {};
    {
        InkertonDesc.bIsEncounter = false;
        InkertonDesc.EnemyDesc.enemyStats.fMaxHP = 1000.f;
        InkertonDesc.EnemyDesc.enemyStats.fMaxBP = 30.f;
        InkertonDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
        InkertonDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;        
        XMStoreFloat4x4(&InkertonDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(123.46f, 5.f, -62.21f));
        XMStoreFloat4x4(&InkertonDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(123.46f, 5.f, -72.21f));
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_VILLAGE, L"Prototype_GameObject_Boss_Inkerton", LEVEL_VILLAGE, strLayerTag, &InkertonDesc)))
        return E_FAIL;*/

    //CBoss_Petroch::DESC PetrochDesc {};
    //{
    //    PetrochDesc.EnemyDesc.enemyStats.fMaxHP = 100.f;
    //    PetrochDesc.EnemyDesc.enemyStats.fMaxBP = 30.f;
    //    PetrochDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
    //    PetrochDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;

    //    XMStoreFloat4x4(&PetrochDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(123.46f, 5.f, -62.21f));
    //    XMStoreFloat4x4(&PetrochDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(123.46f, 5.f, -72.21f));
    //}
    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_VILLAGE, L"Prototype_GameObject_Boss_Petroch", LEVEL_VILLAGE, strLayerTag, &PetrochDesc)))
    //    return E_FAIL;

    //CBoss_Roland::DESC RolandDesc{};
    //{
    //    RolandDesc.EnemyDesc.enemyStats.fMaxHP = 100.f;
    //    RolandDesc.EnemyDesc.enemyStats.fMaxBP = 30.f;
    //    RolandDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
    //    RolandDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
    //    XMStoreFloat4x4(&RolandDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(123.46f, 5.f, -62.21f));
    //    XMStoreFloat4x4(&RolandDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(123.46f, 5.f, -72.21f));
    //}
    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_VILLAGE, L"Prototype_GameObject_Boss_Roland", LEVEL_VILLAGE, strLayerTag, &RolandDesc)))
    //    return E_FAIL;

    //{
        //CEnemy_BobbitWorm::DESC BobbitWormDesc{};
    //    BobbitWormDesc.EnemyDesc.enemyStats.fMaxHP = 30.f;
    //    BobbitWormDesc.EnemyDesc.enemyStats.fMaxBP = 30.f;
    //    BobbitWormDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
    //    BobbitWormDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;

    //    //904.f, 1.f, 536.f
    //    XMStoreFloat4x4(&BobbitWormDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationRollPitchYaw(0.f, -90.f, 0.f)* XMMatrixTranslation(118.46f, 5.f, -93.21f));
    //    XMStoreFloat4x4(&BobbitWormDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(118.46f, 5.f, -93.21f));

    //    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_VILLAGE, L"Prototype_GameObject_Enemy_BobbitWorm", LEVEL_VILLAGE, strLayerTag, &BobbitWormDesc)))
    //        return E_FAIL;
    //}

//    XMStoreFloat4x4(&PlayerDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(118.46f, 5.f, -102.21f));
    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_NPC(const _wstring& strLayerTag)
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_NPC");
#endif

    {/*더미 NPC*/
        HRESULT hr{};
        DWORD dwByte{};
        HANDLE hFile = CreateFile(L"../Bin/DataFiles/NPC_Data/Village_NPCs.npc", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        _bool bRead = false;

        if (INVALID_HANDLE_VALUE == hFile)
        {
            MSG_BOX("Can Not Open File");
        }

        while (true)
        {
            _float4x4 WorldMat = {};;

            bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);
            if (!bRead || dwByte == 0)
                break;

            _wstring strModelTag = L"Prototype_Component_Model_NPC" + to_wstring(CUtils::Compute_Random(0, 5));

            CNPC_NonInteractive::DESC Desc = {};
            {
                Desc.GameObjectDesc.WorldMatrix = WorldMat;
                Desc.strModelTag = strModelTag;
                Desc.iIdleAnimNumber = 0;
            }
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_NonInteractive", LEVEL_VILLAGE, strLayerTag, &Desc)))
                return E_FAIL;
        }

        CloseHandle(hFile);
    }

    CNPC_Bablo::DESC BabloDesc{};
    {
        BabloDesc.szFileName = L"../Bin/DataFiles/NPC_Data/Bablo.npc";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Bablo", LEVEL_VILLAGE, strLayerTag, &BabloDesc)))
        return E_FAIL;

    CNPC_Urchin::DESC UrchinDesc {};
    {
        UrchinDesc.szFileName = L"../Bin/DataFiles/NPC_Data/Urchin.npc";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Urchin", LEVEL_VILLAGE, strLayerTag, &UrchinDesc)))
        return E_FAIL;

    CNPC_Tortellini::DESC TortelliniDesc {};
    {
        TortelliniDesc.szFileName = L"../Bin/DataFiles/NPC_Data/Tortellini.npc";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Tortellini", LEVEL_VILLAGE, strLayerTag, &TortelliniDesc)))
        return E_FAIL;

    CNPC_Children::DESC ChildrenDesc{};
    {
        ChildrenDesc.strModelTag = L"Prototype_Component_Model_NPC_Sessi";
        ChildrenDesc.fAngleOffset = 0.f;
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Children", LEVEL_VILLAGE, strLayerTag, &ChildrenDesc)))
        return E_FAIL;

    {
        ChildrenDesc.strModelTag = L"Prototype_Component_Model_NPC_Pollip";
        ChildrenDesc.fAngleOffset = 0.5f;
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Children", LEVEL_VILLAGE, strLayerTag, &ChildrenDesc)))
        return E_FAIL;

    CNPC_Interactive::DESC RolandDesc{};
    {
        RolandDesc.szFileName = L"../Bin/DataFiles/NPC_Data/Roland_Village.npc";

        RolandDesc.strModelTag = L"Prototype_Component_Model_NPC_Roland";
        RolandDesc.vColliderExtents = {350.f, 350.f, 350.f};
        RolandDesc.fChatBubbleHeight = 5.f;

        RolandDesc.iIdleAnimNumber = 1;
        RolandDesc.iTalkAnimNumber = 2;

        RolandDesc.iDialogueFileNum = 3;
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Interactive", LEVEL_VILLAGE, strLayerTag, &RolandDesc)))
        return E_FAIL;

    CNPC_NonInteractive::DESC InkertonDesc{};
    {
        InkertonDesc.szFileName = L"../Bin/DataFiles/NPC_Data/Inkerton_Village.npc";

        InkertonDesc.strModelTag = L"Prototype_Component_Model_NPC_Inkerton";
        InkertonDesc.iIdleAnimNumber = 0;
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_NonInteractive", LEVEL_VILLAGE, strLayerTag, &InkertonDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Effect(const _wstring& strLayerTag)
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_Effect");
#endif
    CSpriteEffect::DESC SpriteDesc{};
    CMeshEffect::DESC   MeshDesc{};
    CMeshInstEffect::DESC MeshInstDesc{};

    // 환경 이펙트
    HANDLE hFile = CreateFile(L"../Bin/DataFiles/Effects/Bubbles.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_BUBBLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 20, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/GodRay.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_GODRAY, LEVEL_STATIC, L"Prototype_Effect_GodRay", 1, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Env/MeshPtcl_ButterflyFish_Few.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshInstEffect(hFile, MeshInstDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_BUTTERFLY_FISH, LEVEL_STATIC, L"Prototype_Effect_MeshInst_NonBlend", 16, &MeshInstDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Env/MeshPtcl_SurgeonFish_Few.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshInstEffect(hFile, MeshInstDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_SURGEON_FISH, LEVEL_STATIC, L"Prototype_Effect_MeshInst_NonBlend", 16, &MeshInstDesc)))
        return E_FAIL;

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

    // 아이템 이펙트(리스폰)
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Item/ItemRespawnParticle.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ITEM_PTCL_RESPAWN, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 32, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Item/ItemRespawnSprite.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ITEM_SPRITE_RESPAWN, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 32, &SpriteDesc)))
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

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Sponge/Spong_Ptcl_SandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_SPONG_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    // 버블 이펙트
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/SwimHandBubbles.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_SWIMBUBBLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 20, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/JumpBubble.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPBUBBLE, LEVEL_STATIC, L"Prototype_Effect_Change_ExplToRiseW", 10, &SpriteDesc)))
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
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PARRY_BLUE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Parry/ParryPtclYellow.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PARRY_YELLOW, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Parry/ParryPtclRed.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PARRY_RED, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Parry/ParryPtclRedBig.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PARRY_REDBIGONE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    // 공격 이펙트(메쉬)
    // hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Trail_AlphaBlend_Needit.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Trail_WeightBlend_Needit.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_ATT00, LEVEL_STATIC, L"Prototype_Effect_PlayerAttNormal", 12, &MeshDesc)))
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
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Hammer/Hammer_Ptcl_SwapDiamond.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_SWAPDIAMOND, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Hammer/Hammer_Ptcl_SwapSpark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_SWAPSPARK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

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


    // 공격 이펙트(스킬 - 피즐)
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

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_PhysicsTest(const _wstring& strLayerTag)
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_PhysicsTest");
#endif

    //CDummy_NonAnim::DESC DummyDesc{};
    ////DummyDesc.strModelTag = L"Prototype_Component_Model_ForkLift";
    //DummyDesc.strModelTag = L"Prototype_Component_Model_Map_Bridge";
    //DummyDesc.eLevelID = LEVEL_VILLAGE;
    ////DummyDesc.eLevelID = LEVEL_STATIC;
    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_VILLAGE, L"Prototype_GameObject_Dummy_NonAnim", LEVEL_VILLAGE, strLayerTag, &DummyDesc)))
    //    return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG
HRESULT CLevel_Village::Ready_Layer_Debug(const _wstring& strLayerTag)
{
    {
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_ObjectInfo", LEVEL_VILLAGE, strLayerTag)))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_Character", LEVEL_VILLAGE, strLayerTag)))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_Level", LEVEL_VILLAGE, strLayerTag)))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_PostProcess", LEVEL_VILLAGE, strLayerTag)))
            return E_FAIL;

        //CItem_Passenger::DESC PassengerDesc{};

        //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATI C, L"Prototype_GameObject_Item_Passenger", LEVEL_VILLAGE, strLayerTag, &PassengerDesc)))
        //    return E_FAIL;

        /*CRemains::DESC RemainsDesc{};

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Remains", LEVEL_VILLAGE, strLayerTag, &RemainsDesc)))
            return E_FAIL;*/
    }

    return S_OK;
}
HRESULT CLevel_Village::Ready_Layer_CameraTool(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer
    (
        LEVEL_STATIC, 
        L"Prototype_GameObject_CameraTool", 
        LEVEL_VILLAGE, 
        strLayerTag
    )))
        return E_FAIL;

    return S_OK;
}
#endif

HRESULT CLevel_Village::Ready_Environment()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Environment");
#endif

    {   /*Point Light*/
        _wstring szFile = L"../Bin/DataFiles/Map_Light/Village.light";

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


    /* Load all Data */
    vector<_wstring> ThemeFilePaths
    {
        L"../Bin/DataFiles/Atmosphere/Village_Jump.atmosphere",             /* JUMPMAP */
        L"../Bin/DataFiles/Atmosphere/Village.atmosphere",                  /* VILLAGE */
        L"../Bin/DataFiles/Atmosphere/Village_Bridge_Dark.atmosphere",      /* BRIDGE */
        L"../Bin/DataFiles/Atmosphere/Village_Field.atmosphere",            /* FIELD */
    };

    if (FAILED(THEME_DESC::Load_Themes(m_Themes.data(), ThemeFilePaths, THEME_END)))
        return E_FAIL;

    if (false == CTheme_Manager::Set_Atmospher_With_Pending_FromStatic())
        m_Themes[(_uint)THEME::JUMPMAP]->Set_Atmosphere();

    return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Lights(const _wstring& strLayerTag)
{
    {
        CLight_Player::DESC Desc {};
        Desc.pPlayer = m_pGameInstance->Find_GameObject(LEVEL_VILLAGE, L"Layer_Player");
        Desc.GameObjectDesc.strName = L"lightPlayer";

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Light_Player", LEVEL_VILLAGE, strLayerTag, &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

void CLevel_Village::SetUp_CollisionGroup()
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

void CLevel_Village::ReadFile_MeshEffect(HANDLE hFile, CMeshEffect::DESC& Desc)
{
    _tchar szName[MAX_PATH] {};
    _uint iEffectType {}, iTextureIndex {}, iMeshType {};
    DWORD dwByte {};
    _bool bRead {};
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

void CLevel_Village::ReadFile_SpriteEffect(HANDLE hFile, CSpriteEffect::DESC& Desc)
{
    _tchar szName[MAX_PATH] {};
    _uint iEffectType {}, iTextureIndex {};
    DWORD dwByte {};
    _bool bRead {};
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

    _int3 vNumAtlas {};
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

void CLevel_Village::ReadFile_MeshInstEffect(HANDLE hFile, CMeshInstEffect::DESC& Desc)
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

CLevel_Village* CLevel_Village::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Village* pInstance = new CLevel_Village { pDevice, pContext };

    //if (FAILED(pInstance->Initialize()))
    //{
    //    MSG_BOX("Failed to Create : CLevel_Village");
    //    Safe_Release(pInstance);
    //}

    return pInstance;
}

void CLevel_Village::Free()
{
    CLevel::Free();

    for (auto& pTheme : m_Themes)
        Safe_Delete(pTheme);
}
