#include "pch.h"
#include "Level_Field.h"

#include "GameInstance.h"

#include "Theme_Manager.h"
#include "Visit_Manger.h"
#include "Sky.h"
#include "Light_Player.h"

#include "Camera_Free.h"
#include "Player.h"

#include "Boss_Inkerton.h"
#include "Boss_Petroch.h"
#include "Boss_Pagurus.h"
#include "Enemy_Sardine.h"
#include "Enemy_Normie.h"
#include "Enemy_Gumpounder.h"
#include "Enemy_Bowman.h"
#include "Enemy_Splitter.h"
#include "Enemy_Bobbitworm.h"
#include "Enemy_Cuttlefish.h"
#include "NPC_Headers.h"
#include "NPC_Manager.h"

#include "MapObject.h"
#include "Instancing_SeaWeeds.h"
#include "Instancing_KelpBig.h"
#include "Instancing_SeaGrass.h"
#include "Instancing_KelpGround.h"
#include "Terrain.h"

#include "MapTrigger.h"
#include "CallbackUpdate.h"
#include "Level_Loading.h"

#include "Item_Trash.h"
#include "Item_BottleCap.h"
#include "Shell_Item.h"
#include "UI_Manager.h"

#include "Dummy_NonAnim.h"
#include "WaterShadow.h"

#include "PlayerManager.h"

#include "EffectType.h"

#include "Arrow.h"

CLevel_Field::CLevel_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

HRESULT CLevel_Field::Initialize()
{
    /* 맵 오브젝트 먼저 레벨에 추가하고, 그 다음 컨트롤러 넣어야 문제가 없음 */
    if (FAILED(m_pGameInstance->Setup_PhysicsScene(_vec2(-4.f, -4.f), _vec2(1024.f, 1024.f), 8.f)))
        return E_FAIL;

#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_Environment");
#endif

    if (FAILED(Ready_Environment()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Environment(TEXT("Layer_Environment"))))
        return E_FAIL;

#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_Effect");
#endif

    if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
        return E_FAIL;

#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_MapObject");
    SetWindowText(g_hWnd, L"Ready_Layer_MapObject");
#endif

    if (FAILED(Ready_Layer_MapObject(TEXT("Layer_MapObject"))))
        return E_FAIL;

#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_InteractionObject");
#endif

    if (FAILED(Ready_Layer_InteractionObject(TEXT("Layer_InteractionObject"))))
        return E_FAIL;

#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_MapTrigger");
#endif

    if (FAILED(Ready_Layer_MapItem(TEXT("Layer_MapItem"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_MapTrigger(TEXT("Layer_MapTrigger"))))
        return E_FAIL;

#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_Projectile");
#endif

    if (FAILED(Ready_Layer_Projectile(TEXT("Layer_Projectile"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Shell(TEXT("Layer_Shell"))))
        return E_FAIL;

#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_Player");
#endif

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
        return E_FAIL;

#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_Camera");
#endif

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_CinematicCamera(TEXT("Layer_CinematicCamera"))))
        return E_FAIL;

#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_Monster");
#endif

    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
        return E_FAIL;

#ifdef _DEBUG
    SetWindowText(g_hWnd, L"Ready_Layer_NPC");
#endif

    if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Lights(L"Layer_Lights")))
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

#ifdef _DEBUG
    SetWindowText(g_hWnd, L"UI_Manager");
#endif


    CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    pUIManager->Add_PartObject_For_Play();
    pUIManager->FadeIn();


    m_pGameInstance->Play_Sound_Loop(L"Water_Travel_Loop_1.wav", (_uint)SOUND_CHANNEL::AMBIENCE_LOOP, 0.f);
    m_pGameInstance->Play_Sound_Loop(L"Field_BGM.wav", (_uint)SOUND_CHANNEL::BGM, 0.5f);

    if (false == CVisit_Manger::Is_Visit_FromStatic(L"field"))
        pUIManager->Show_TextUI(L"틈새의 모래밭", _float4(235.f / 255.f, 215.f / 255.f, 254.f / 255.f, 1.f));

    /* Level_Initalize 동안의 누적된 타임델타 제거 */
    m_pGameInstance->Compute_TimeDelta(L"Timer_60");

    return S_OK;
}

void CLevel_Field::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Field::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, L"필드 레벨 입니다.");
#endif
    return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Environment(const _wstring& strLayerTag)
{
    CSky::DESC SkyDesc {};
    SkyDesc.iTextureIndex = 0;
    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_Sky", LEVELID::LEVEL_FIELD, strLayerTag, &SkyDesc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
        return E_FAIL;

    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer(LEVEL_FIELD, TEXT("Prototype_GameObject_Terrain"), LEVEL_FIELD, strLayerTag)))
        return E_FAIL;
    //CTerrain* pTerrain = static_cast<CTerrain*>(m_pGameInstance->Find_GameObject(LEVEL_FIELD, strLayerTag, 1)); // Terrain

    //if (FAILED(pTerrain->Load_HeightMap(TEXT("../Bin/Resources/Textures/Terrain/MakeHeightMap.bmp"))))
    //    MSG_BOX("Height_Load_Failed!!!!!!!");


    /* for.WaterShadow */
    {
        CWaterShadow::DESC Desc {};
		Desc.GameObjectDesc.WorldMatrix = _mat::ScalingMatrix({0.5f, 0.5f, 0.5f})._float4x4();
        Desc.GameObjectDesc.strName = L"WaterShadow";

        if (FAILED(m_pGameInstance->
            Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_WaterShadow", LEVEL_FIELD, strLayerTag, &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_MapObject(const _wstring& strLayerTag)
{
    //인스턴싱
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Instancing_SeaWeeds"), LEVELID::LEVEL_FIELD, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Instancing_KelpBig"), LEVELID::LEVEL_FIELD, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Instancing_SeaGrass00"), LEVELID::LEVEL_FIELD, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Instancing_KelpGround"), LEVELID::LEVEL_FIELD, strLayerTag)))
        return E_FAIL;

    //맵오브젝트
    {
        _wstring szFile = L"../Bin/DataFiles/Map_Object/Field00.environment";

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

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_MapObject", LEVELID::LEVEL_FIELD, strLayerTag, &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
                return E_FAIL;

            _uint iObjListSize = (_uint)(m_pGameInstance->Get_GameObjects_In_Layer(LEVELID::LEVEL_FIELD, strLayerTag)).size();
            CGameObject* pNewGameObject = m_pGameInstance->Find_GameObject(LEVELID::LEVEL_FIELD, strLayerTag, iObjListSize - 1);
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
                        || strName == "CoralTree_05_LOD0_A.001"
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
                        || strName == "WormCoral_LOD0") {
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

HRESULT CLevel_Field::Ready_Layer_InteractionObject(const _wstring& strLayerTag)
{
    { // 골레포트
        _wstring szFile = L"../Bin/DataFiles/Map_InteractionObject/SheleportID_Field.interaction"; //다이얼로그로 가져오는 파일패스 저장할 곳
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

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_ShellHouse", LEVELID::LEVEL_FIELD, L"Layer_Insteraction_Environment", &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
                return E_FAIL;

        }

        CloseHandle(hFile);
    }

    { // 인터렉션 오브젝트
        _wstring szFile = L"../Bin/DataFiles/Map_InteractionObject/Field00.interaction"; //다이얼로그로 가져오는 파일패스 저장할 곳
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

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, strName, LEVELID::LEVEL_FIELD, strLayerTag, &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
            {
                if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_FIELD, strName, LEVELID::LEVEL_FIELD, strLayerTag, &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
                    return E_FAIL;
            }
        }

        CloseHandle(hFile);

    }



    return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_MapItem(const _wstring& strLayerTag)
{
    {// 아이템

        _wstring szFile = L"../Bin/DataFiles/Map_Item/Field_Item.item"; //다이얼로그로 가져오는 파일패스 저장할 곳
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
            _float4x4 WorldMat = {};// 월드 행렬
            _bool bRead = false;

            bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);

            if (!bRead || 0 == dwByte)
                break;

            CItem_Trash::DESC TrashDesc{};
            TrashDesc.ItemDesc.GameObjectDesc.WorldMatrix = WorldMat;
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Item_Trash", LEVEL_FIELD, strLayerTag, &TrashDesc)))
                return E_FAIL;
        }
        CloseHandle(hFile);
    }

    CNPC_Manager* pNPCManager = static_cast<CNPC_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_NPCManager", 0));
    if (pNPCManager->Load_State(L"Urchin") < 3)
    {
        CItem_BottleCap::DESC BottleCap{};
        {
            XMStoreFloat4x4(&BottleCap.ItemDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(201.92f, 0.00f, 350.64f));
        }
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Item_BottleCap", LEVEL_FIELD, strLayerTag, &BottleCap)))
            return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_MapTrigger(const _wstring& strLayerTag)
{
    {//여기서 트리거 파일 불러오깅 ㅠ
        _wstring szFile = L"../Bin/DataFiles/MapTrigger/FieldTrigger.mapTrigger";
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
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapTrigger", LEVEL_FIELD, L"Layer_MapTrigger", &Desc)))
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
        pTrigger = static_cast<CMapTrigger*>(m_pGameInstance->Find_GameObject_With_Name(LEVEL_FIELD, L"Layer_MapTrigger", L"Trigger_FieldToPinBall"));

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
                            CPlayerManager::Save_Transition(XMMatrixRotationY(XMConvertToRadians(-170.f)) * XMMatrixTranslation(25.302f, 40.f, 43.751f));
                            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_PINBALL));
                        }

                        return bFadeEnd;
                    });
            });
    }

    pTrigger = { nullptr };
    {
        pTrigger = static_cast<CMapTrigger*>(m_pGameInstance->Find_GameObject_With_Name(LEVEL_FIELD, L"Layer_MapTrigger", L"Trigger_FieldToVillage"));

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
                            CPlayerManager::Save_Transition(XMMatrixRotationY(XMConvertToRadians(-150.f)) * XMMatrixTranslation(257.025f, 16.f, 392.909f));
                            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_VILLAGE));
                        }

                        return bFadeEnd;
                    });

                static_cast<CTheme_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_ThemeManager"))
                    ->Set_PendingThemeFilePath(L"../Bin/DataFiles/Atmosphere/Village_Field.atmosphere");
            });

    }

    pTrigger = { nullptr };
    {
        pTrigger = static_cast<CMapTrigger*>(m_pGameInstance->Find_GameObject_With_Name(LEVEL_FIELD, L"Layer_MapTrigger", L"Trigger_FieldInkerton"));

		pTrigger->Add_CollisionEnter_Callback([this]()->void
			{
				if (false == CVisit_Manger::Is_Visit_FromStatic(L"FieldInkerton"))
				{
					static_cast<CBoss_Inkerton*>(CGameInstance::GetInstance()->Find_GameObject_With_Name(LEVEL_FIELD, L"Layer_Monster", L"Inkerton"))->Ambush();
				}
			});

    }

    pTrigger = { nullptr };
    {
        pTrigger = static_cast<CMapTrigger*>(m_pGameInstance->Find_GameObject_With_Name(LEVEL_FIELD, L"Layer_MapTrigger", L"Trigger_FieldToCrabTomb"));


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
                            CPlayerManager::Save_Transition(XMMatrixRotationY(XMConvertToRadians(40.f))* XMMatrixTranslation(41.305f, 5.f, 38.956f));
                            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CRABTOMB));
                        }
                        return bFadeEnd;
                    });
            });
    }

    CMapTrigger::Create_InversedMapTrigger(LEVEL_FIELD, L"Trigger_SetThemeToAzit", L"Trigger_SetThemeToAzitInverse", _vec3(3.5f, 0.f, -4.5f));

    CMapTrigger::Resist_MapTrigger_ThemeChanger(LEVEL_FIELD, L"Trigger_SetThemeToAzit", m_Themes[(_uint)THEME::AZIT], 1.f);
    CMapTrigger::Resist_MapTrigger_ThemeChanger(LEVEL_FIELD, L"Trigger_SetThemeToAzitInverse", m_Themes[(_uint)THEME::FIELD], 1.f);
    CMapTrigger::Resist_MapTrigger_ThemeChanger(LEVEL_FIELD, L"Trigger_SetThemeToMagnet", m_Themes[(_uint)THEME::MAGNET], 1.f);
    CMapTrigger::Resist_MapTrigger_ThemeChanger(LEVEL_FIELD, L"Trigger_SetThemeToPinBall", m_Themes[(_uint)THEME::PINBALL], 1.f);
    CMapTrigger::Resist_MapTrigger_ThemeChanger(LEVEL_FIELD, L"Trigger_SetThemeToCrabTomb", m_Themes[(_uint)THEME::CRABTOMB], 1.f);

    CMapTrigger::Resist_MapTrigger_SoundChanger(LEVEL_FIELD, L"Trigger_SetThemeToAzit", L"Azit_BGM.wav", 1.f, 0.5f);
    CMapTrigger::Resist_MapTrigger_SoundChanger(LEVEL_FIELD, L"Trigger_SetThemeToAzitInverse", L"Field_BGM.wav", 1.f, 0.5f);
    CMapTrigger::Resist_MapTrigger_SoundChanger(LEVEL_FIELD, L"Trigger_SetThemeToMagnet", L"Magnet_BGM.wav", 1.f, 0.5f);
    CMapTrigger::Resist_MapTrigger_SoundChanger(LEVEL_FIELD, L"Trigger_SetThemeToCrabTomb", L"CrabTomb_BGM.wav", 1.f, 0.5f);

    return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Projectile(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Projectile_Manager", LEVEL_FIELD, strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Shell(const _wstring& strLayerTag)
{
    // 쉘 설치 
    {
        wstring szFile = L"../Bin/DataFiles/Map_Item/Field_Shell.item";
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
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Shell_World", LEVEL_FIELD, strLayerTag, &tShellDesc)))
                return E_FAIL;
        }

        CloseHandle(hFile);
    }

    return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Camera(const _wstring& strLayerTag)
{
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
        Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"), LEVEL_FIELD, strLayerTag, &CameraFreeDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_CinematicCamera(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer
        (
            LEVEL_STATIC,
            TEXT("Prototype_GameObject_CinematicCamera"),
            LEVEL_FIELD,
            strLayerTag
        )))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Player(const _wstring& strLayerTag)
{
    CPlayer::DESC PlayerDesc{};
    XMStoreFloat4x4(&PlayerDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(178.34f, 100.f, 345.32f));
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Player"), LEVEL_FIELD, strLayerTag, &PlayerDesc)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Monster(const _wstring& strLayerTag)
{
    //중간 지점 원
    {
        CEnemy_Sardine::DESC SardineDesc{};
        SardineDesc.EnemyDesc.enemyStats.fMaxHP = 30.f;
        SardineDesc.EnemyDesc.enemyStats.fMaxBP = 10.f;
        SardineDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
        SardineDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
        vector<_vec3> PatrolPos{};
        PatrolPos.push_back(XMVectorSet(491.f, 1.f, 339.f, 1.f));
        PatrolPos.push_back(XMVectorSet(487.f, 1.f, 353.f, 1.f));
        PatrolPos.push_back(XMVectorSet(492.f, 1.f, 366.f, 1.f));
        PatrolPos.push_back(XMVectorSet(509.f, 1.f, 371.f, 1.f));
        PatrolPos.push_back(XMVectorSet(523.f, 1.f, 363.f, 1.f));
        PatrolPos.push_back(XMVectorSet(511.f, 1.f, 341.f, 1.f));
        PatrolPos.push_back(XMVectorSet(501.f, 1.f, 336.f, 1.f));
        SardineDesc.EnemyDesc.PatrolPoints = PatrolPos;
        XMStoreFloat4x4(&SardineDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(501.f, 1.f, 336.f));
        XMStoreFloat4x4(&SardineDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(501.f, 1.f, 336.f));
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Enemy_Sardine", LEVEL_FIELD, strLayerTag, &SardineDesc)))
            return E_FAIL;
    }

    //중간 지점 원
    {
        CEnemy_Sardine::DESC SardineDesc{};
        SardineDesc.EnemyDesc.enemyStats.fMaxHP = 30.f;
        SardineDesc.EnemyDesc.enemyStats.fMaxBP = 10.f;
        SardineDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
        SardineDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
        vector<_vec3> PatrolPos{};
        PatrolPos.push_back(XMVectorSet(509.f, 1.f, 371.f, 1.f));
        PatrolPos.push_back(XMVectorSet(523.f, 1.f, 363.f, 1.f));
        PatrolPos.push_back(XMVectorSet(511.f, 1.f, 341.f, 1.f));
        PatrolPos.push_back(XMVectorSet(501.f, 1.f, 336.f, 1.f));
        PatrolPos.push_back(XMVectorSet(491.f, 1.f, 339.f, 1.f));
        PatrolPos.push_back(XMVectorSet(487.f, 1.f, 353.f, 1.f));
        PatrolPos.push_back(XMVectorSet(492.f, 1.f, 366.f, 1.f));
        SardineDesc.EnemyDesc.PatrolPoints = PatrolPos;
        XMStoreFloat4x4(&SardineDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(492.f, 1.f, 366.f));
        XMStoreFloat4x4(&SardineDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(492.f, 1.f, 366.f));
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Enemy_Sardine", LEVEL_FIELD, strLayerTag, &SardineDesc)))
            return E_FAIL;
    }

    //중간 지점 원
    {
        CEnemy_Sardine::DESC SardineDesc{};
        SardineDesc.EnemyDesc.enemyStats.fMaxHP = 30.f;
        SardineDesc.EnemyDesc.enemyStats.fMaxBP = 10.f;
        SardineDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
        SardineDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
        vector<_vec3> PatrolPos{};
        PatrolPos.push_back(XMVectorSet(511.f, 1.f, 341.f, 1.f));
        PatrolPos.push_back(XMVectorSet(501.f, 1.f, 336.f, 1.f));
        PatrolPos.push_back(XMVectorSet(491.f, 1.f, 339.f, 1.f));
        PatrolPos.push_back(XMVectorSet(487.f, 1.f, 353.f, 1.f));
        PatrolPos.push_back(XMVectorSet(492.f, 1.f, 366.f, 1.f));
        PatrolPos.push_back(XMVectorSet(509.f, 1.f, 371.f, 1.f));
        PatrolPos.push_back(XMVectorSet(523.f, 1.f, 363.f, 1.f));
        SardineDesc.EnemyDesc.PatrolPoints = PatrolPos;
        XMStoreFloat4x4(&SardineDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(523.f, 1.f, 363.f));
        XMStoreFloat4x4(&SardineDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(523.f, 1.f, 363.f));
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Enemy_Sardine", LEVEL_FIELD, strLayerTag, &SardineDesc)))
            return E_FAIL;
    }

    // 박스 앞
    {
        CEnemy_Bowman::DESC BowmanDesc{};
        BowmanDesc.EnemyDesc.enemyStats.fMaxHP = 100.f;
        BowmanDesc.EnemyDesc.enemyStats.fMaxBP = 30.f;
        BowmanDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
        BowmanDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;

        XMStoreFloat4x4(&BowmanDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(540.f, 1.f, 573.f));
        XMStoreFloat4x4(&BowmanDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(540.f, 1.f, 573.f));
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Enemy_Bowman", LEVEL_FIELD, strLayerTag, &BowmanDesc)))
            return E_FAIL;
    }

    //잉커톤 가는길
    //{
    //    CEnemy_Bowman::DESC BowmanDesc{};
    //    BowmanDesc.EnemyDesc.enemyStats.fMaxHP = 100.f;
    //    BowmanDesc.EnemyDesc.enemyStats.fMaxBP = 30.f;
    //    BowmanDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
    //    BowmanDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;

    //    XMStoreFloat4x4(&BowmanDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(560.f, 1.f, 552.f));
    //    XMStoreFloat4x4(&BowmanDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(560.f, 1.f, 552.f));
    //    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Enemy_Bowman", LEVEL_FIELD, strLayerTag, &BowmanDesc)))
    //        return E_FAIL;
    //}
    
    {
        CEnemy_Bowman::DESC BowmanDesc{};
        BowmanDesc.EnemyDesc.enemyStats.fMaxHP = 100.f;
        BowmanDesc.EnemyDesc.enemyStats.fMaxBP = 30.f;
        BowmanDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
        BowmanDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;

        XMStoreFloat4x4(&BowmanDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationRollPitchYaw(0.f, -75.f, 0.f) * XMMatrixTranslation(407.f, 1.f, 341.f));
        XMStoreFloat4x4(&BowmanDesc.EnemyDesc.OriginalPos, XMMatrixRotationRollPitchYaw(0.f, -75.f, 0.f) * XMMatrixTranslation(407.f, 1.f, 341.f));
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Enemy_Bowman", LEVEL_FIELD, strLayerTag, &BowmanDesc)))
            return E_FAIL;
    }

    //{
    //    CEnemy_Bowman::DESC BowmanDesc{};
    //    BowmanDesc.EnemyDesc.enemyStats.fMaxHP = 100.f;
    //    BowmanDesc.EnemyDesc.enemyStats.fMaxBP = 30.f;
    //    BowmanDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
    //    BowmanDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;

    //    XMStoreFloat4x4(&BowmanDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationRollPitchYaw(0.f, -31.f, 0.f) * XMMatrixTranslation(534.f, 1.f, 452.f));
    //    XMStoreFloat4x4(&BowmanDesc.EnemyDesc.OriginalPos, XMMatrixRotationRollPitchYaw(0.f, -31.f, 0.f)* XMMatrixTranslation(534.f, 1.f, 452.f));
    //    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Enemy_Bowman", LEVEL_FIELD, strLayerTag, &BowmanDesc)))
    //        return E_FAIL;
    //}

    //{
    //    CEnemy_Bowman::DESC BowmanDesc{};
    //    BowmanDesc.EnemyDesc.enemyStats.fMaxHP = 100.f;
    //    BowmanDesc.EnemyDesc.enemyStats.fMaxBP = 30.f;
    //    BowmanDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
    //    BowmanDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;

    //    XMStoreFloat4x4(&BowmanDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationRollPitchYaw(0.f, -132.72, 0.f) * XMMatrixTranslation(928.f, 1.f, 679.f));
    //    XMStoreFloat4x4(&BowmanDesc.EnemyDesc.OriginalPos, XMMatrixRotationRollPitchYaw(0.f, -132.72, 0.f) * XMMatrixTranslation(928.f, 1.f, 679.f));
    //    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Enemy_Bowman", LEVEL_FIELD, strLayerTag, &BowmanDesc)))
    //        return E_FAIL;
    //}

    //{
    //    CEnemy_Bowman::DESC BowmanDesc{};
    //    BowmanDesc.EnemyDesc.enemyStats.fMaxHP = 100.f;
    //    BowmanDesc.EnemyDesc.enemyStats.fMaxBP = 30.f;
    //    BowmanDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
    //    BowmanDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;

    //    XMStoreFloat4x4(&BowmanDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationRollPitchYaw(0.f, -63, 0.f) * XMMatrixTranslation(938.f, 1.f, 654.f));
    //    XMStoreFloat4x4(&BowmanDesc.EnemyDesc.OriginalPos, XMMatrixRotationRollPitchYaw(0.f, -63, 0.f) * XMMatrixTranslation(938.f, 1.f, 654.f));
    //    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Enemy_Bowman", LEVEL_FIELD, strLayerTag, &BowmanDesc)))
    //        return E_FAIL;
    //}

    {
        CEnemy_Splitter::DESC SpliterDesc{};
        SpliterDesc.EnemyDesc.enemyStats.fMaxHP = 500.f;
        SpliterDesc.EnemyDesc.enemyStats.fMaxBP = 200.f;
        SpliterDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
        SpliterDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;        
        vector<_vec3> PatrolPos{};
        SpliterDesc.EnemyDesc.PatrolPoints = PatrolPos;
        XMStoreFloat4x4(&SpliterDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(0.f, 0.f, 0.f));
        XMStoreFloat4x4(&SpliterDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(0.f, 0.f, 0.f));

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Enemy_Splitter", LEVEL_FIELD, strLayerTag, &SpliterDesc)))
            return E_FAIL;
    }

    {
        CBoss_Inkerton::Inkerton_DESC InkertonDesc{};
        InkertonDesc.bIsEncounter = true;
        InkertonDesc.EnemyDesc.enemyStats.fMaxHP = 1000.f;
        InkertonDesc.EnemyDesc.enemyStats.fMaxBP = 210.f;
        InkertonDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
        InkertonDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
        InkertonDesc.EnemyDesc.GameObjectDesc.strName = L"Inkerton";
        InkertonDesc.EnemyDesc.szName = L"홀란드의 오른집게 잉커톤";

        XMStoreFloat4x4(&InkertonDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationRollPitchYaw(0.f, -90.f, 0.f) * XMMatrixTranslation(806.12f, 0.1f, 777.36));
        XMStoreFloat4x4(&InkertonDesc.EnemyDesc.OriginalPos, XMMatrixRotationRollPitchYaw(0.f, -90.f, 0.f)* XMMatrixTranslation(806.12f, 0.1f, 777.36));

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Boss_Inkerton", LEVEL_FIELD, strLayerTag, &InkertonDesc)))
            return E_FAIL;
    }

    {
        CEnemy_BobbitWorm::DESC BobbitWormDesc{};
        BobbitWormDesc.EnemyDesc.enemyStats.fMaxHP = 30.f;
        BobbitWormDesc.EnemyDesc.enemyStats.fMaxBP = 30.f;
        BobbitWormDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
        BobbitWormDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
        
        XMStoreFloat4x4(&BobbitWormDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationRollPitchYaw(0.f, -90.f, 0.f) * XMMatrixTranslation(553.66f, 1.f, 537.77f));
        XMStoreFloat4x4(&BobbitWormDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(553.66f, 1.5f, 537.77f));

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Enemy_BobbitWorm", LEVEL_FIELD, strLayerTag, &BobbitWormDesc)))
            return E_FAIL;
    }    

    {
        CEnemy_BobbitWorm::DESC BobbitWormDesc{};
        BobbitWormDesc.EnemyDesc.enemyStats.fMaxHP = 30.f;
        BobbitWormDesc.EnemyDesc.enemyStats.fMaxBP = 30.f;
        BobbitWormDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
        BobbitWormDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
        
        XMStoreFloat4x4(&BobbitWormDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationRollPitchYaw(0.f, -90.f, 0.f) * XMMatrixTranslation(706.72f, 1.f, 722.65f));
        XMStoreFloat4x4(&BobbitWormDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(706.72f, 1.5f, 722.65f));

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Enemy_BobbitWorm", LEVEL_FIELD, strLayerTag, &BobbitWormDesc)))
            return E_FAIL;
    }
    
    {
        CEnemy_Sardine::DESC SardineDesc{};
        SardineDesc.EnemyDesc.enemyStats.fMaxHP = 30.f;
        SardineDesc.EnemyDesc.enemyStats.fMaxBP = 10.f;
        SardineDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
        SardineDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
        vector<_vec3> PatrolPos{};
        PatrolPos.push_back(XMVectorSet(570.f, 1.f, 587.f, 1.f));
        PatrolPos.push_back(XMVectorSet(611.f, 1.f, 653.f, 1.f));
        PatrolPos.push_back(XMVectorSet(578.f, 1.f, 636.f, 1.f));
        PatrolPos.push_back(XMVectorSet(561.f, 1.f, 614.f, 1.f));
        SardineDesc.EnemyDesc.PatrolPoints = PatrolPos;
        XMStoreFloat4x4(&SardineDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(561.f, 1.f, 614.f));
        XMStoreFloat4x4(&SardineDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(561.f, 1.f, 614.f));
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Enemy_Sardine", LEVEL_FIELD, strLayerTag, &SardineDesc)))
            return E_FAIL;
    }

    {
        CBoss_Pagurus::Pagurus_DESC PagurusDesc{};
        PagurusDesc.EnemyDesc.enemyStats.fMaxHP = 1000.f;
        PagurusDesc.EnemyDesc.enemyStats.fMaxBP = 210.f;
        PagurusDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
        PagurusDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
        PagurusDesc.EnemyDesc.GameObjectDesc.strName = L"Pagurus";
        PagurusDesc.EnemyDesc.szName = L"걸신들린 자 파구투스";

        vector<_vec3> PatrolPos{};
        PatrolPos.push_back(XMVectorSet(526, 1.f, 873, 1.f));
        PatrolPos.push_back(XMVectorSet(555, 1.f, 845, 1.f));
        PatrolPos.push_back(XMVectorSet(529, 1.f, 836, 1.f));
        PatrolPos.push_back(XMVectorSet(500, 1.f, 855, 1.f));

        PatrolPos.push_back(XMVectorSet(440, 1.f, 855, 1.f));
        PatrolPos.push_back(XMVectorSet(412, 1.f, 898, 1.f));
        PatrolPos.push_back(XMVectorSet(362, 1.f, 907, 1.f));
        PatrolPos.push_back(XMVectorSet(380, 1.f, 935, 1.f));

        PatrolPos.push_back(XMVectorSet(414, 1.f, 939, 1.f));
        PatrolPos.push_back(XMVectorSet(421, 1.f, 877, 1.f));
        PatrolPos.push_back(XMVectorSet(448, 1.f, 858, 1.f));
        PatrolPos.push_back(XMVectorSet(497, 1.f, 856, 1.f));
        PagurusDesc.EnemyDesc.PatrolPoints = PatrolPos;

        XMStoreFloat4x4(&PagurusDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixRotationRollPitchYaw(0.f, -90.f, 0.f) * XMMatrixTranslation(519.f, 1.f, 864.f));
        XMStoreFloat4x4(&PagurusDesc.EnemyDesc.OriginalPos, XMMatrixRotationRollPitchYaw(0.f, -90.f, 0.f) * XMMatrixTranslation(519.f, 1.f, 864.f));

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Boss_Pagurus", LEVEL_FIELD, strLayerTag, &PagurusDesc)))
            return E_FAIL;
    }

    //{
    //    CEnemy_Normie::DESC SardineDesc{};
    //    SardineDesc.EnemyDesc.enemyStats.fMaxHP = 30.f;
    //    SardineDesc.EnemyDesc.enemyStats.fMaxBP = 10.f;
    //    SardineDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
    //    SardineDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
    //    vector<_vec3> PatrolPos{};
    //    SardineDesc.EnemyDesc.PatrolPoints = PatrolPos;
    //    XMStoreFloat4x4(&SardineDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(197.f, 1.f, 338.f));
    //    XMStoreFloat4x4(&SardineDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(197.f, 1.f, 338.f));
    //    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Enemy_Normie", LEVEL_FIELD, strLayerTag, &SardineDesc)))
    //        return E_FAIL;
    //}


    {
        CEnemy_Gumpounder::DESC SardineDesc{};
        SardineDesc.EnemyDesc.enemyStats.fMaxHP = 500.f;
        SardineDesc.EnemyDesc.enemyStats.fMaxBP = 200.f;
        SardineDesc.EnemyDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
        SardineDesc.EnemyDesc.GameObjectDesc.TransformDesc.fRotationPerSec = 2.f;
        vector<_vec3> PatrolPos{};
        SardineDesc.EnemyDesc.PatrolPoints = PatrolPos;
        XMStoreFloat4x4(&SardineDesc.EnemyDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(197.f, 3.f, 328.f));
        XMStoreFloat4x4(&SardineDesc.EnemyDesc.OriginalPos, XMMatrixTranslation(197.f, 3.f, 328.f));
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Enemy_Gumpounder", LEVEL_FIELD, strLayerTag, &SardineDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_NPC(const _wstring& strLayerTag)
{
    {/*더미 NPC*/
        HRESULT hr{};
        DWORD dwByte{};
        HANDLE hFile = CreateFile(L"../Bin/DataFiles/NPC_Data/Field_Homeless.npc", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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

            CNPC_NonInteractive::DESC Desc = {};
            {
                Desc.GameObjectDesc.WorldMatrix = WorldMat;
                Desc.strModelTag = L"Prototype_Component_Model_Homeless";
                Desc.iIdleAnimNumber = 0;
            }
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_NonInteractive", LEVEL_VILLAGE, strLayerTag, &Desc)))
                return E_FAIL;
        }

        CloseHandle(hFile);
    }

    CNPC_BlackSmith::DESC BlackSmithDesc{};
    {
        BlackSmithDesc.szFileName = L"../Bin/DataFiles/NPC_Data/BlackSmith.npc";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_BlackSmith", LEVEL_FIELD, strLayerTag, &BlackSmithDesc)))
        return E_FAIL;

    CNPC_Nemma::DESC NemmaDesc{};
    {
        NemmaDesc.szFileName = L"../Bin/DataFiles/NPC_Data/Nemma.npc";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Nemma", LEVEL_FIELD, strLayerTag, &NemmaDesc)))
        return E_FAIL;

    CNPC_Prawnathan::DESC PrawnathanDesc{};
    {
        PrawnathanDesc.szFileName = L"../Bin/DataFiles/NPC_Data/Prawnathan.npc";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Prawnathan", LEVEL_FIELD, strLayerTag, &PrawnathanDesc)))
        return E_FAIL;

    // 스테이트가 -1이 아닐때만 생성
    CNPC_Chitan::DESC ChitanDesc{};
    {
        ChitanDesc.szFileName = L"../Bin/DataFiles/NPC_Data/Chitan1.npc";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Chitan", LEVEL_FIELD, strLayerTag, &ChitanDesc)))
        return E_FAIL;

    CNPC_Topoda::DESC TopodaDesc{};
    {
        TopodaDesc.szFileName = L"../Bin/DataFiles/NPC_Data/Topoda.npc";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Topoda", LEVEL_FIELD, strLayerTag, &TopodaDesc)))
        return E_FAIL;

    CNPC_ShellSplitter::DESC ShellSplitter{};
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_ShellSplitter", LEVEL_FIELD, strLayerTag, &ShellSplitter)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Manager(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_FIELD, L"Prototype_GameObject_Enemy_Manager", LEVEL_FIELD, strLayerTag, nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Effect(const _wstring& strLayerTag)
{
    CSpriteEffect::DESC SpriteDesc{};
    CMeshEffect::DESC   MeshDesc{};
    CMeshInstEffect::DESC   MeshInstDesc{};

    // 환경 이펙트
    HANDLE hFile = CreateFile(L"../Bin/DataFiles/Effects/Bubbles.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_BUBBLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 16, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/GodRay.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_GODRAY, LEVEL_STATIC, L"Prototype_Effect_GodRay", 16, &SpriteDesc)))
        return E_FAIL;

    // 자석 이펙트
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Magnet/Magnet_Spark.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_MAGNET_SPARK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 32, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Magnet/Magnet_MeshInst_Scrap.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshInstEffect(hFile, MeshInstDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_MAGNET_MESHINST_SCRAP, LEVEL_STATIC, L"Prototype_Effect_MeshInst_Once", 1, &MeshInstDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Magnet/Magnet_Tube.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_MAGNET_MESH_PULL, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 1, &MeshDesc)))
        return E_FAIL;

    // 증기 이펙트
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Steam/Steam_Ptcl_SteamBubbles.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_STEAM_BUBBLE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop_NonBlend", 32, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Steam/Steam_Ptcl_SteamSmoke.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_STEAM_SMOKE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop_NonBlend", 32, &SpriteDesc)))
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

    // 회피 반격 이펙트
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Revenge/Mesh_BackStepAttack.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_BACKSTEP_ATTACK, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 2, &MeshDesc)))
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

    // 디버프
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Player/Debuf_Fear_Teardrop.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_DEBUF_FEAR, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 2, &SpriteDesc)))
        return E_FAIL;

    // 몬스터 : 활게 이펙트
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Bowman_ArrowTrail.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENEMY_PTCL_ARROWTRAIL, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 16, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Bowman_ArrowBubble.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENEMY_PTCL_ARROWBUBBLE, LEVEL_STATIC, L"Prototype_Effect_Change_ExplToRiseW", 16, &SpriteDesc)))
        return E_FAIL;

    // 몬스터 : 처형자 이펙트
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Splitter/Splitter_ScaryFace.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::SPLITR_MESH_FEAR, LEVEL_STATIC, L"Prototype_Effect_SplitterFear", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Splitter/Splitter_ScaryScrew.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::SPLITR_MESH_FEAR_SCREW, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Splitter/Splitter_Grab_Trail.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::SPLITR_MESH_GRAB, LEVEL_STATIC, L"Prototype_Effect_SplitterSwipe", 4, &MeshDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Splitter/Splitter_Guillotine_Hit.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_HIT, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Splitter/Splitter_Guillotine_Hit_After.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::SPLITR_SPRT_HIT_AFTER, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Splitter/Splitter_Guillotine_Hit_End.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::SPLITR_SPRT_HIT_END, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 4, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Splitter/Splitter_Guillotine_HitOverhead.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_SLAMSHOCK, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Splitter/Splitter_Guillotine_HitOverhead_Sandburst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Splitter/Splitter_Execute_PlayerHit.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_EXECUTE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Splitter/Splitter_Execute_PlayerHit_After.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_EXECUTE_AFTER, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Splitter/Splitter_Claw_Bash.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_BASH, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Splitter/Splitter_Claw_Bash_After.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_BASH_AFTER, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Splitter/Splitter_Claw_Bash_Mesh.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_MeshEffect(hFile, MeshDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::SPLITR_MESH_BASH, LEVEL_STATIC, L"Prototype_Effect_Mesh_Once", 4, &MeshDesc)))
        return E_FAIL;

    // 보스 - 잉커톤 이펙트
    //--- 먹물
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

    // 보우맨 이펙트
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Field/Bowman/Ptcl_Bash.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::BOWMAN_PTCL_BASH, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Field/Bowman/Ptcl_Claw.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::BOWMAN_PTCL_CLAW, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 2, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Field/Bowman/Ptcl_Claw_SandBurst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::BOWMAN_PTCL_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 8, &SpriteDesc)))
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

    // PAGURUS_PTCL_SANDBURST
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Enemy/Pagurus/Pagurus_Ptcl_Sandburst.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::PAGURUS_PTCL_SANDBURST, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 64, &SpriteDesc)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Field::Ready_layer_PhysicsTest(const _wstring& strLayerTag)
{
    /* Dummy NonAnim */
    for (_uint i=0; i<10; ++i)
    {
        CDummy_NonAnim::DESC Desc {};
        Desc.eLevelID = LEVEL_STATIC;
        Desc.strModelTag = L"Prototype_Component_Model_Shell_MirrorBall";

        Desc.GameObjectDesc.WorldMatrix = _mat(XMMatrixTranslation(183.34f, 2.f * i + 2.f, 345.32f))._float4x4();

		if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Dummy_NonAnim", LEVEL_FIELD, strLayerTag, &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Field::Ready_Environment()
{
    /* Load all Data */
    //array<_wstring, THEME_END> ThemaFilePaths
    vector<_wstring> ThemaFilePaths
    {
        L"../Bin/DataFiles/Atmosphere/Field.atmosphere",                        /* FIELD */
        L"../Bin/DataFiles/Atmosphere/Field_Azit.atmosphere",                   /* AZIT */
        L"../Bin/DataFiles/Atmosphere/Field_Magnet_ColorTest2.atmosphere",                 /* MAGNET */
        L"../Bin/DataFiles/Atmosphere/Field_Pinball.atmosphere",                /* PINBALL */
        L"../Bin/DataFiles/Atmosphere/Field_CrabTomb.atmosphere",               /* CRABTOMB */
    };

    for (_uint i = 0; i < THEME_END; ++i)
    {
        m_Themes[i] = new THEME_DESC();
    }

    for (_uint i=0; i<THEME_END; ++i)
    { 
        const _wstring& strFilePath = ThemaFilePaths[i];

		HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (INVALID_HANDLE_VALUE == hFile)
            return E_FAIL;

		DWORD dwByte {};
		_bool bResult {};

		bResult = ReadFile(hFile, m_Themes[i], sizeof(THEME_DESC), &dwByte, nullptr);
		CloseHandle(hFile);

        if (false == bResult)
            return E_FAIL;
    }

    {   /*Point Light*/
        _wstring szFile = L"../Bin/DataFiles/Map_Light/Field.light";

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

    if (false == CTheme_Manager::Set_Atmospher_With_Pending_FromStatic())
        m_Themes[(_uint)THEME::FIELD]->Set_Atmosphere();


    return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Lights(const _wstring& strLayerTag)
{
    {
        CLight_Player::DESC Desc {};
        Desc.pPlayer = m_pGameInstance->Find_GameObject(LEVEL_FIELD, L"Layer_Player");
        Desc.GameObjectDesc.strName = L"lightPlayer";

		if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Light_Player", LEVEL_FIELD, strLayerTag, &Desc)))
			return E_FAIL;
    }

    return S_OK;
}

#ifdef _DEBUG
HRESULT CLevel_Field::Ready_Layer_Debug(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_ObjectInfo", LEVEL_FIELD, strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_Character", LEVEL_FIELD, strLayerTag)))
		return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_Level", LEVEL_FIELD, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_PostProcess", LEVEL_FIELD, strLayerTag)))
        return E_FAIL;



    //CTriggerObserverTest::DESC Desc {};
    //Desc.TriggerNames.push_back(L"Trigger_Test01");

    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_TriggerObserverTest", LEVEL_FIELD, strLayerTag, &Desc)))
    //    return E_FAIL;
    

    return S_OK;
}
HRESULT CLevel_Field::Ready_Layer_CameraTool(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer
    (
        LEVEL_STATIC,
        L"Prototype_GameObject_CameraTool",
        LEVEL_FIELD,
        strLayerTag
    )))
        return E_FAIL;

    return S_OK;
}
#endif

void CLevel_Field::SetUp_CollisionGroup()
{
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::MONSTER, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::MONSTER_HIT, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::NPC, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::ITEM, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::MAP_INTERACTION, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::GRAPPLENEEDLE, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::SPONGE, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::MAGNET, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::STEAM, true);
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
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::MONSTER, (_uint)COLLISION_GROUP::SPONGE, true);
}


void CLevel_Field::ReadFile_MeshEffect(HANDLE hFile, CMeshEffect::DESC& Desc)
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

void CLevel_Field::ReadFile_SpriteEffect(HANDLE hFile, CSpriteEffect::DESC& Desc)
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

void CLevel_Field::ReadFile_MeshInstEffect(HANDLE hFile, CMeshInstEffect::DESC& Desc)
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

CLevel_Field* CLevel_Field::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new CLevel_Field(pDevice, pContext);
}

void CLevel_Field::Free()
{
    super::Free();

    for (auto& pTheme : m_Themes)
    {
        Safe_Delete(pTheme);
    }
}
