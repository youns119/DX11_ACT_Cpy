#include "pch.h"
#include "Level_Club.h"

#include "GameInstance.h"

#include "Sky.h"
#include "Theme_Manager.h"

#include "Camera_Free.h"
#include "Player.h"
#include "NPC_Headers.h"
#include "CutScene_Manager.h"

#include "MapObject.h"
#include "Laser.h"

#include "MapTrigger.h"
#include "TriggerObserverTest.h"
#include "CallbackUpdate.h"
#include "Level_Loading.h"

#include "UI_Manager.h"

#include "NPC_Firth.h"

CLevel_Club::CLevel_Club(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

HRESULT CLevel_Club::Initialize()
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

void CLevel_Club::Update(_float fTimeDelta)
{
    _float fRadian = 15.f;
    for (auto& Light : m_ToolLightList)
    {
        LIGHT_DESC LightDesc = m_pGameInstance->Get_LightDesc(Light.strName);

        if (LightDesc.eType == LIGHT_TYPE::SPOT)
        {
            Light.fCurrentAngle += Light.fRotationSpeed * fTimeDelta;
            if (Light.fCurrentAngle >= XM_2PI)
                Light.fCurrentAngle -= XM_2PI;
            else if (Light.fCurrentAngle < 0.f)
                Light.fCurrentAngle += XM_2PI;

            Light.fVerticalAngle += Light.fRotationSpeed * fTimeDelta;
            if (Light.fVerticalAngle >= XM_2PI)
                Light.fVerticalAngle -= XM_2PI;
            else if (Light.fVerticalAngle < 0.f)
                Light.fVerticalAngle += XM_2PI;

            // 구형 좌표계로 위치 계산 (fTheta: 수평각도, fPhi: 수직각도)
            _float fTheta = Light.fCurrentAngle;  // Y축 기준 회전 각도
            _float fPhi = Light.fVerticalAngle;   // Z축 기준 위아래 각도

            //_float fCos = cosf(Light.fCurrentAngle);
            //_float fSin = sinf(Light.fCurrentAngle);

            Light.vPosition.x = fRadian * sinf(fPhi) * cosf(fTheta) + 11.f;  // X 위치
            Light.vPosition.y = fRadian * cosf(fPhi) + 10.f;                 // Y 위치
            Light.vPosition.z = fRadian * sinf(fPhi) * sinf(fTheta) + 12.f;  // Z 위치

            LightDesc.vPosition = Light.vPosition;

            _vector vCenter = XMVectorSet(11.f, 10.f, 12.f, 1.f); // 중심
            _vector vLight = XMLoadFloat4(&Light.vPosition);
            _vector vDir = XMVector3Normalize(vLight - vCenter); // 바깥 방향으로 쏘기
            XMStoreFloat4(&LightDesc.vDirection, vDir);

            m_pGameInstance->Set_LightDesc(Light.strName, LightDesc);
        }
    }
}

HRESULT CLevel_Club::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"해변 레벨 입니다.");
#endif
    return S_OK;
}

HRESULT CLevel_Club::Ready_Layer_Environment(const _wstring& strLayerTag)
{
    CSky::DESC SkyDesc{};
    SkyDesc.iTextureIndex = 0;
    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_Sky", LEVELID::LEVEL_CLUB, strLayerTag, &SkyDesc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Club::Ready_Environment()
{

    {   /* 글로벌조명 + 쉐도우라이트 + 톤매핑 + 블룸 + 안개 */
        _wstring strFilePath = L"../Bin/DataFiles/Atmosphere/Pinball.atmosphere";

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

    {   /*Point Light*/
        _wstring szFile = L"../Bin/DataFiles/Map_Light/ClubPoint.light";

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

    {   /*Spot Light*/
        _wstring szFile = L"../Bin/DataFiles/Map_Light/ClubSpot.light";
        LIGHT_DESC LightDesc = {};

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


            bRead = ReadFile(hFile, &LightDesc, sizeof(LIGHT_DESC), &dwByte, nullptr);

            if (!bRead || dwByte == 0)
                break;
        }

        CloseHandle(hFile);
        for (int i = 0; i < 15; ++i)
        {
            //// 컬러 무지개 느낌으로 분산
            _float hue = i / 10.f; // 0 ~ 1
            _float r = sinf(hue * XM_2PI) * 0.7f + 0.3f;
            _float g = sinf(hue * XM_2PI + XM_2PI / 3) * 0.7f + 0.3f;
            _float b = sinf(hue * XM_2PI + XM_2PI * 2 / 3) * 0.7f + 0.3f;
            LightDesc.vDiffuse = _float4(r * (_float)(rand() % 10) * 2.f, 15.f, b * (_float)(rand() % 10) * 3.f, 1.f); // 밝기 *6 유지
            LightDesc.fRange = 50.f;
            //// 위치 원형 배치
            //_float angle = XM_2PI * (_float(i) / 10.f);
            //_float radius = 10.f;
            //_float x = 11.55f;
            //_float z = 5.f;
            //_float y = 12.51f;
            ////11.55f, 5.f, 12.51f, 1.f
            //_vector vPos = { x, y, z };
            //_vector vTarget = { 0.f, y - 15.f, 0.f }; // 중심보다 아래를 봄
            //_vector vDir = XMVectorSetW(XMVector3Normalize(vTarget - vPos), 0.f);

            //LightDesc.vPosition = _float4(x, y, z, 1.f);
            ////LightDesc.vDirection = _float4{ 0.f , -1.f, 0.f, 0.f };
            //XMStoreFloat4(&LightDesc.vDirection, vDir);

            SPOTLIGHT tSpot = {};
            // 라이트 이름
            _wstring strName = L"AddSpot" + to_wstring(i);
            tSpot.strName = strName;
            tSpot.vPosition = _float4((rand() % 20) - 10, (rand() % 20), (rand() % 20) - 10, 1.f);
            tSpot.vColor = _float4(1.0f, 1.0f, 1.0f, 1.0f);
            tSpot.fRotationSpeed = 1.0f + (i % 3) * 0.5f; // 속도 더 빠르게

            tSpot.fCurrentAngle = XM_2PI * (i / 10.0f); // 0~360도 분산
            tSpot.fVerticalAngle = XM_PI * ((i + 1) / 11.0f); // 위아래 분산 (0 ~ 180도 사이)

            m_pGameInstance->Add_Light(tSpot.strName.c_str(), LightDesc);
            m_ToolLightList.push_back(tSpot);

        }
    }
    return S_OK;
}

HRESULT CLevel_Club::Ready_Layer_MapObject(const _wstring& strLayerTag)
{
    //맵오브젝트5
    {
        _wstring szFile = L"../Bin/DataFiles/Map_Object/Club.environment";

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

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_MapObject", LEVELID::LEVEL_CLUB, strLayerTag, &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
                return E_FAIL;
        }

        CloseHandle(hFile);
    }


    return S_OK;
}

HRESULT CLevel_Club::Ready_Layer_InteractionObject(const _wstring& strLayerTag)
{
    {
        _wstring szFile = L"../Bin/DataFiles/Map_InteractionObject/Club.interaction"; //다이얼로그로 가져오는 파일패스 저장할 곳

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

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_CLUB, strName, LEVELID::LEVEL_CLUB, strLayerTag, &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
                return E_FAIL;
        }

        CloseHandle(hFile);
    }

    {
        _wstring szFile = L"../Bin/DataFiles/Map_InteractionObject/SheleportID_Club.interaction"; //다이얼로그로 가져오는 파일패스 저장할 곳
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

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_ShellHouse", LEVELID::LEVEL_CLUB, L"Layer_Insteraction_Environment", &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
                return E_FAIL;

        }

        CloseHandle(hFile);
    }

    //{
    //    CLaser::DESC Desc = {};
    //    XMStoreFloat4x4(&Desc.GameObjectDesc.TransformDesc.WorldMatrix, XMMatrixIdentity());
    //    Desc.GameObjectDesc.TransformDesc.WorldMatrix.m[3][0] = 10.f;
    //    Desc.GameObjectDesc.TransformDesc.WorldMatrix.m[3][1] = 5.f;
    //    Desc.GameObjectDesc.TransformDesc.WorldMatrix.m[3][2] = 11.f;
    //    Desc.GameObjectDesc.TransformDesc.WorldMatrix.m[3][3] = 1.f;

    //    Desc.strModelTag = L"Prototype_Component_Model_Laser";
    //    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_CLUB, L"Prototype_GameObject_Laser", LEVELID::LEVEL_CLUB, L"Layer_Insteraction_Environment", &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
    //        return E_FAIL;
    //}

   

    return S_OK;

}

HRESULT CLevel_Club::Ready_Layer_MapTrigger(const _wstring& strLayerTag)
{
    {//여기서 트리거 파일 불러오깅 ㅠ
        _wstring szFile = L"../Bin/DataFiles/MapTrigger/ClubTrigger.mapTrigger";
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
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapTrigger", LEVEL_CLUB, L"Layer_MapTrigger", &Desc)))
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
        pTrigger = static_cast<CMapTrigger*>(m_pGameInstance->Find_GameObject_With_Name(LEVEL_CLUB, L"Layer_MapTrigger", L"Trigger_FadeIn"));

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

                            CCutScene_Manager* pCutSceneManager = static_cast<CCutScene_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CutSceneManager", 0));
                            pCutSceneManager->Begin_CutScene(L"Enter_Club");

                            THEME_DESC* pThemeDesc { nullptr };
							vector<_wstring> FilePaths { L"../Bin/DataFiles/Atmosphere/Club.atmosphere" };
                            THEME_DESC::Load_Themes(&pThemeDesc, FilePaths, 1);
                            pThemeDesc->Set_Atmosphere();
                            Safe_Delete(pThemeDesc);

                            pUIManager->FadeIn();
                        }
                        return bFadeEnd;
                    });
            });
    }
    return S_OK;
}

HRESULT CLevel_Club::Ready_Layer_Projectile(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Projectile_Manager", LEVEL_CLUB, strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Club::Ready_Layer_Camera(const _wstring& strLayerTag)
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
        Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"), LEVEL_CLUB, strLayerTag, &CameraFreeDesc)))
        return E_FAIL;

    return S_OK;

}

HRESULT CLevel_Club::Ready_Layer_CinematicCamera(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer
        (
            LEVEL_STATIC,
            TEXT("Prototype_GameObject_CinematicCamera"),
            LEVEL_CLUB,
            strLayerTag
        )))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Club::Ready_Layer_Player(const _wstring& strLayerTag)
{
    CPlayer::DESC PlayerDesc{};
    XMStoreFloat4x4(&PlayerDesc.GameObjectDesc.WorldMatrix, XMMatrixTranslation(11.26f, 156.73f, 7.48f));
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Player"), LEVEL_CLUB, strLayerTag, &PlayerDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Club::Ready_Layer_Monster(const _wstring& strLayerTag)
{
    return S_OK;
}

HRESULT CLevel_Club::Ready_Layer_NPC(const _wstring& strLayerTag)
{
    {/*댄스 게*/
        HRESULT hr{};
        DWORD dwByte{};
        HANDLE hFile = CreateFile(L"../Bin/DataFiles/NPC_Data/DanceCrab.npc", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        _bool bRead = false;

        if (INVALID_HANDLE_VALUE == hFile)
        {
            MSG_BOX("Can Not Open File");
        }

        while (true)
        {
            _float4x4 WorldMat = {};;
            
            bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);
            CGameObject::DESC Desc = {};
            Desc.strName = L"Prototype_GameObject_NPC_DanceCrab";
            Desc.TransformDesc.WorldMatrix = WorldMat;
            if (!bRead || dwByte == 0)
                break;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_DanceCrab", LEVEL_CLUB, strLayerTag, &Desc)))
                return E_FAIL;
        }

        CloseHandle(hFile);
    }

    CNPC_GuardCrab::DESC Crab{};
    {
        Crab.szFileName = L"../Bin/DataFiles/NPC_Data/GuardCrab.npc";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_GuardCrab", LEVEL_CLUB, strLayerTag, &Crab)))
        return E_FAIL;

    CNPC_Player::DESC Player{};
    {
        Player.szFileName = L"../Bin/DataFiles/NPC_Data/ClubPlayer.npc";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Player", LEVEL_CLUB, strLayerTag, &Player)))
        return E_FAIL;

    CNPC_Firth::DESC Firth{};
    {
        Firth.szFileName = L"../Bin/DataFiles/NPC_Data/ClubFirth.npc";
    }
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_NPC_Firth", LEVEL_CLUB, strLayerTag, &Firth)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Club::Ready_Layer_Manager(const _wstring& strLayerTag)
{


    return S_OK;
}

HRESULT CLevel_Club::Ready_Layer_Effect(const _wstring& strLayerTag)
{
    CSpriteEffect::DESC SpriteDesc{};
    CMeshEffect::DESC   MeshDesc{};


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

    // 음표 이펙트
    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Music/MusicOn.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_MUSIC_ON, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 16, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Music/MusicQuater.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_MUSIC_QUATER, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 16, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Music/MusicLine.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_MUSIC_LINE, LEVEL_STATIC, L"Prototype_Effect_Sprite_Once", 16, &SpriteDesc)))
        return E_FAIL;

    hFile = CreateFile(L"../Bin/DataFiles/Effects/Map/Music/FireCracker.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile_SpriteEffect(hFile, SpriteDesc);
    if (FAILED(m_pGameInstance->Pool_Effect((_uint)EFFECT_TYPE::ENV_FIRECRACKER, LEVEL_STATIC, L"Prototype_Effect_Sprite_Loop", 16, &SpriteDesc)))
        return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG
HRESULT CLevel_Club::Ready_Layer_Debug(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_ObjectInfo", LEVEL_CLUB, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_Character", LEVEL_CLUB, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_Level", LEVEL_CLUB, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Debug_PostProcess", LEVEL_CLUB, strLayerTag)))
        return E_FAIL;

    return S_OK;
}
HRESULT CLevel_Club::Ready_Layer_CameraTool(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer
    (
        LEVEL_STATIC,
        L"Prototype_GameObject_CameraTool",
        LEVEL_CLUB,
        strLayerTag
    )))
        return E_FAIL;

    return S_OK;
}
#endif

void CLevel_Club::SetUp_CollisionGroup()
{
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::MONSTER, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::MONSTER_HIT, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::NPC, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::ITEM, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::MAP_INTERACTION, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::GRAPPLENEEDLE, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::SPONGE, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::MAGNET, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::HEARTKELP, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::SHELL, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::PORTAL, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::TRIGGER, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER, (_uint)COLLISION_GROUP::URCHIN, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER_LOCKON, (_uint)COLLISION_GROUP::MONSTER, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER_WEAPON, (_uint)COLLISION_GROUP::MONSTER, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER_PROJECTILE, (_uint)COLLISION_GROUP::MONSTER, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER_PROJECTILE, (_uint)COLLISION_GROUP::GRAPPLENEEDLE, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER_GRAPPLE, (_uint)COLLISION_GROUP::GRAPPLENEEDLE, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER_GRAPPLE, (_uint)COLLISION_GROUP::MONSTER, true);
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER_GRAPPLE, (_uint)COLLISION_GROUP::SHELL, true);
    //m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::PLAYER_PROJECTILE, (_uint)COLLISION_GROUP::SHELL, true);

}

void CLevel_Club::ReadFile_MeshEffect(HANDLE hFile, CMeshEffect::DESC& Desc)
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

void CLevel_Club::ReadFile_SpriteEffect(HANDLE hFile, CSpriteEffect::DESC& Desc)
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

CLevel_Club* CLevel_Club::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new CLevel_Club(pDevice, pContext);
}

void CLevel_Club::Free()
{
    super::Free();
}
