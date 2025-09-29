#include "pch.h"
#include "MapLevel_Tool.h"

#include "GameInstance.h"

#include "Camera_Free.h"
#include "Sky.h"
#include "ShellHouse.h"
#include "Dialog.h"
#include "MapObject.h"
#include "Instancing_SeaWeeds.h"
#include "Instancing_KelpBig.h"
#include "Instancing_SeaGrass.h"
#include "Instancing_KelpGround.h"
#include "MapTool_NPC.h"
#include "MapTrigger.h"
#include "MapTool_Monster.h"
#include "MapTool_Item.h"

#include <filesystem>

//#include "Dummy_NonAnim.h"
//#include "Dummy_Anim.h"

CMapLevel_Tool::CMapLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CMapLevel_Tool::Initialize()
{
     if (FAILED(Ready_Lights()))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Debug(TEXT("Layer_Debug"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Environment(TEXT("Layer_Environment"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;


    return S_OK;
}

void CMapLevel_Tool::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(KEY::G))
        m_bDebugGrid = !m_bDebugGrid;

    if (m_bDebugGrid)
    {
        static _vec3 vOrigin = { -12.5f, 0.9f, 12.5f };

        if (m_pGameInstance->Key_Press(KEY::Z))
            vOrigin.y += 0.05f;
        if (m_pGameInstance->Key_Press(KEY::X))
            vOrigin.y -= 0.05f;
        if (m_pGameInstance->Key_Press(KEY::RIGHT))
            vOrigin.x -= 0.05f;
        if (m_pGameInstance->Key_Press(KEY::LEFT))
            vOrigin.x += 0.05f;
        if (m_pGameInstance->Key_Press(KEY::UP))
            vOrigin.z -= 0.05f;
        if (m_pGameInstance->Key_Press(KEY::DOWN))
            vOrigin.z += 0.05f;

        m_pGameInstance->Add_DebugRender_Grid(vOrigin, _vec3(1025.f, 0.f, 0.f), _vec3(0.f, 0.f, 1025.f), 25.f, 25.f, _vec4(0.f, 0.f, 0.f, 0.f));
        m_fAccTimeDelta += fTimeDelta * 10.f;
    }

    Update_ImGui(); 

    //m_fAngle += 2.f * fTimeDelta;
    //for (auto LightName : m_ToolLightList)
    //{
    //    LIGHT_DESC LightDesc = m_pGameInstance->Get_LightDesc(LightName);

    //    if (LightDesc.eType == LIGHT_TYPE::SPOT)
    //    {
    //        // 회전 반지름 (x, z, y축 각각 조정 가능)
    //        _float fRadiusX = 26.f;  // x축 회전 반지름
    //        _float fRadiusZ = 29.f;  // z축 회전 반지름
    //        _float fRadiusY = 10.f;   // y축 범위 (0에서 10까지)

    //        // 각 조명마다 회전 시작 각도 다르게 설정
    //        _float fAnglePerLight = XM_2PI / m_ToolLightList.size();
    //        _float fOffset = LightName[LightName.length() - 1] * fAnglePerLight; // LightName으로 인덱싱
    //        _float fCurrentAngle = m_fAngle + fOffset; // 각 조명별로 시작각 다르게

    //        _vector vCenter = XMVectorSet(0.f, 0.f, 0.f, 1.f);  // 바닥을 기준으로 회전
    //        //_vector vCenter = XMVectorSet(0.f, LightDesc.vPosition.y, 0.f, 1.f);

    //        // 회전 오프셋
    //        _vector vOffset = XMVectorSet(
    //            cosf(fCurrentAngle) * fRadiusX,    // x축 회전
    //            sinf(m_fAngle) * fRadiusY -15.f,           // y축 회전 (0에서 10까지 범위)
    //            sinf(fCurrentAngle) * fRadiusZ,    // z축 회전
    //            0.f
    //        );

    //        // 라이트 방향 조정 (아래로 조금 더 보이게)
    //        _float fDownwardBias = -0.3f; // 아래 방향 가중치

    //        // 새 위치와 방향
    //        _vector vPosition = vCenter + vOffset;
    //        _vector vDirection = vCenter - vPosition; // 중심을 바라보게
    //        vDirection = XMVectorSetY(vDirection, XMVectorGetY(vDirection) + fDownwardBias);
    //        vDirection = XMVector3Normalize(vDirection); // 단위 벡터로

    //        // 라이트 디스크립션 업데이트
    //        XMStoreFloat4(&LightDesc.vPosition, XMVectorSetW(vPosition, 1.f));
    //        XMStoreFloat4(&LightDesc.vDirection, XMVectorSetW(vDirection, 0.f));

    //        // 라이트 업데이트
    //        m_pGameInstance->Set_LightDesc(LightName, LightDesc);
    //    }
    //}

    if (m_bPointPicking)
        Picking_NavigationPoint();
}

HRESULT CMapLevel_Tool::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"게임 플레이 레벨이빈다.");
#endif

    return S_OK;
}

HRESULT CMapLevel_Tool::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    CSky::DESC SkyDesc{};
    SkyDesc.iTextureIndex = 3;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_Sky", LEVELID::LEVEL_TOOL, strLayerTag, &SkyDesc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
        return E_FAIL;

    CGameObject::DESC TerrainDesc = {};
    TerrainDesc.strName = L"Terrain";
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Terrain"), LEVELID::LEVEL_TOOL, strLayerTag, &TerrainDesc)))
        return E_FAIL;
    m_pTerrain = static_cast<CTerrain*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, strLayerTag, 1)); // Terrain

    /*if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Instancing_SeaWeeds"), LEVELID::LEVEL_TOOL, strLayerTag))) 
        return E_FAIL;
    m_pSeaWeeds = static_cast<CInstancing_SeaWeeds*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, strLayerTag, 2));

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Instancing_KelpBig"), LEVELID::LEVEL_TOOL, strLayerTag))) 
        return E_FAIL;
    m_pKelpBig = static_cast<CInstancing_KelpBig*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, strLayerTag, 3));

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Instancing_SeaGrass00"), LEVELID::LEVEL_TOOL, strLayerTag))) 
        return E_FAIL;
    m_pSeaGrass00 = static_cast<CInstancing_SeaGrass*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, strLayerTag, 4));
    
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Instancing_KelpGround"), LEVELID::LEVEL_TOOL, strLayerTag)))
        return E_FAIL;
    m_pKelpGround = static_cast<CInstancing_KelpGround*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, strLayerTag, 5));*/

    return S_OK;
}

HRESULT CMapLevel_Tool::Ready_Layer_Debug(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Debug_PostProcess"), LEVELID::LEVEL_TOOL, strLayerTag)))
        return E_FAIL;
    return S_OK;
}

HRESULT CMapLevel_Tool::Ready_Layer_Environment(const _wstring& strLayerTag)
{
    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_ShellHouse"), LEVELID::LEVEL_TOOL, strLayerTag)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CMapLevel_Tool::Ready_Layer_Interaction_Environment(const _wstring& strLayerTag)
{
    return S_OK;
}

HRESULT CMapLevel_Tool::Ready_Layer_Camera(const _wstring& strLayerTag)
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

        //CameraFreeDesc.fMouseSewnsitivity = 0.005f;
        CameraFreeDesc.fMouseSensitivity = 0.1f;
        //CameraFreeDesc.CameraDesc.strName = L"pick1";
    }

    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"), LEVEL_TOOL, strLayerTag, &CameraFreeDesc)))
        return E_FAIL;

    m_pCamera = static_cast<CCamera_Free*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Camera"), 0)); // Camera

    return S_OK;
}

HRESULT CMapLevel_Tool::Ready_Layer_Player(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Player"), LEVEL_TOOL, strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapLevel_Tool::Ready_Lights()
{
    m_pGameInstance->SetUp_ShadowLight(_vec3{ 0.5f, -2.f, 0.5f });

    LIGHT_DESC LightDesc{};
    ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));

    LightDesc.eType = LIGHT_TYPE::DIRECTIONAL;
    LightDesc.vDirection = _float4(0.f, -2.f, -1.f, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(L"Light_Global", LightDesc)))
        return E_FAIL;

    //{
    //    LIGHT_DESC Desc{};
    //    Desc.eType = LIGHT_TYPE::SPOT;
    //    Desc.fRange = 20.f;
    //    Desc.vAttenuation = _float3{ 1.f, 0.001f, 0.001f };
    //    Desc.vCosCutOff = _float2{ 7.f , 10.f };
    //    Desc.vDiffuse = _float4{ 0.f, 6.f, 2.f, 1.f };
    //    Desc.vDirection = _float4{ 0.f , 0.932f, 1.f, 0.f };
    //    Desc.vPosition = _float4{ 3.179f, 17.651f, 18.57f, 1.f };
    //    Desc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);


    //    m_pGameInstance->Add_Light(L"Spot0", Desc);
    //}
    //{   /*Point Light*/
    //    _wstring szFile = L"../Bin/DataFiles/Map_Light/ClubSpot.light";

    //    //로드하기
    //    HRESULT hr{};
    //    DWORD dwByte{};
    //    HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    //    _bool bRead = false;

    //    if (INVALID_HANDLE_VALUE == hFile)
    //    {
    //        MSG_BOX("Can Not Open File");
    //        return E_FAIL;
    //    }

    //    while (true)
    //    {
    //        _wstring strLightName;
    //        strLightName.resize(MAX_PATH);

    //        _tchar szLightName[MAX_PATH] = {};

    //        bRead = ReadFile(hFile, &szLightName, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);
    //        strLightName = szLightName;

    //        LIGHT_DESC LightDesc = {};

    //        bRead = ReadFile(hFile, &LightDesc, sizeof(LIGHT_DESC), &dwByte, nullptr);

    //        if (!bRead || dwByte == 0)
    //            break;

    //        m_vLightDir = LightDesc.vDiffuse;

    //        m_pGameInstance->Add_Light(strLightName.c_str(), LightDesc);
    //        m_ToolLightList.push_back(strLightName);

    //    }

    //    CloseHandle(hFile);
    //}

    //for (int i = 0; i < 10; ++i)
    //{
    //    LIGHT_DESC Desc{};
    //    Desc.eType = LIGHT_TYPE::SPOT;
    //    Desc.fRange = 20.f;
    //    Desc.vAttenuation = _float3{ 1.f, 0.001f, 0.001f };
    //    Desc.vCosCutOff = _float2{ 7.f , 10.f };
    //    Desc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);

    //    // 컬러 무지개 느낌으로 분산
    //    _float hue = i / 10.f; // 0 ~ 1
    //    _float r = sinf(hue * XM_2PI) * 0.5f + 0.5f;
    //    _float g = sinf(hue * XM_2PI + XM_2PI / 3) * 0.5f + 0.5f;
    //    _float b = sinf(hue * XM_2PI + XM_2PI * 2 / 3) * 0.5f + 0.5f;
    //    Desc.vDiffuse = _float4(r * 6.f, g * 6.f, b * 6.f, 1.f); // 밝기 *6 유지

    //    Desc.vDirection = _float4{ 0.f , -1.f, 0.f, 0.f };

    //    // 위치 원형 배치
    //    _float angle = XM_2PI * (_float(i) / 10.f);
    //    _float radius = 10.f;
    //    _float x = cosf(angle) * radius;
    //    _float z = sinf(angle) * radius;
    //    _float y = 17.651f;

    //    Desc.vPosition = _float4(x, y, z, 1.f);

    //    // 라이트 이름
    //    wstring wstrName = L"Spot" + to_wstring(i);
    //    m_pGameInstance->Add_Light(wstrName.c_str(), Desc);

    //    m_ToolLightList.push_back(wstrName);
    //}
    return S_OK;
}


HRESULT CMapLevel_Tool::Update_ImGui()
{
    if (m_pGameInstance->Key_Down(KEY::C))
    {
        ++m_iGizmoSRTType;
        if (m_iGizmoSRTType == 3)
            m_iGizmoSRTType = 0;
    }

    //=======================================Trigger=======================================//
#pragma region Trigger
    ImGui::Begin("Map_Trigger");
    //맵트리거 추가 + 저장 + 불러오기
    {
        ImGui::SeparatorText("Trigger");
        //m_szTriggerbuffer
        m_szTriggerbuffer[sizeof(m_szTriggerbuffer) - 1] = '\0';  // 널 종료 보장

        ImGui::SetNextItemWidth(300.0f);
        if (ImGui::InputText("##TriggerName", m_szTriggerbuffer, IM_ARRAYSIZE(m_szTriggerbuffer))) {}

        _int iSize = MultiByteToWideChar(CP_UTF8, 0, m_szTriggerbuffer, -1, NULL, 0);
        _wstring strString(iSize, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, m_szTriggerbuffer, -1, &strString[0], iSize);

        ImGui::Text("TrrigerName: %ws", strString.c_str());

        // 아래 추가 했을 때 인덱스 설정하려고 올렸어용. 


        if (ImGui::Button("Add_Trigger"))
        {
            CMapTrigger::DESC Desc{};
            Desc.GameObjectDesc.strName = strString.c_str();
            if (FAILED(m_pGameInstance->
                Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapTrigger", LEVEL_TOOL, L"Layer_MapTrigger", &Desc)))
                MSG_BOX("추가 안됨.");

            _uint iObjListSize = (_uint)(m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_MapTrigger")).size();
            CGameObject* pNewGameObject = m_pGameInstance->Find_GameObject(LEVEL_TOOL, L"Layer_MapTrigger", iObjListSize - 1);
            m_iPickedID = pNewGameObject->Get_ID(); // 꼭 바꿔주깅. 피킹 후에 바로 아이디 바꿔줘서 바로 상태 제어 할 수 있게끔요.
            m_iTriggerSelected = (_int)(m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_MapTrigger").size()) - 1;
        }

        list<CGameObject*> MapTriggerList = m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_MapTrigger");
        auto it = MapTriggerList.begin();
        advance(it, m_iTriggerSelected); // 선택된 인덱스로 이동

        // 콤보 박스 생성
        if (ImGui::BeginCombo("Trigger List", MapTriggerList.empty() ? "Empty Triggers" : WStringToString((*it)->Get_Name()).c_str()))
        {
            _int i = 0;
            for (auto it = MapTriggerList.begin(); it != MapTriggerList.end(); ++it, ++i)
            {
                _bool isSelected = (m_iTriggerSelected == i);
                if (ImGui::Selectable(WStringToString((*it)->Get_Name()).c_str(), isSelected))
                {
                    m_iTriggerSelected = i; // 선택된 인덱스 업데이트
                    m_iPickedID = (*it)->Get_ID();
                    ImGui::SetItemDefaultFocus();
                }
    
            }
            ImGui::EndCombo();
        }
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("Save_Trigger"))
        {
            Save_MapTrigger();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load_Trigger"))
        {
            Load_MapTrigger();
        }
    }
    ImGui::End();
#pragma endregion 
    //=======================================Trigger End=======================================//

    //=======================================NPC=======================================//
#pragma region NPC
    ImGui::Begin("NPC");
    //NPC 추가 + 저장 + 불러오긱
    {
        ImGui::SeparatorText("NPC");

        //strncpy(buffer, utf8Text.c_str(), sizeof(buffer));
        m_szbuffer[sizeof(m_szbuffer) - 1] = '\0';  // 널 종료 보장

        ImGui::SetNextItemWidth(300.0f);
        if (ImGui::InputText("##PrototypeName", m_szbuffer, IM_ARRAYSIZE(m_szbuffer))) {}

        _int iSize = MultiByteToWideChar(CP_UTF8, 0, m_szbuffer, -1, NULL, 0);
        _wstring strString(iSize, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, m_szbuffer, -1, &strString[0], iSize);

        ImGui::Text("Input: %ws", strString.c_str()); // 잘됐는지 확ㅇ닌마하는 용됴

        if (ImGui::Button("NPC_Clone"))
        {
            CGameObject::DESC Desc = {};
            Desc.strName = strString.c_str();

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapTool_NPC", LEVEL_TOOL, TEXT("Layer_NPC"), &Desc)))
                MSG_BOX("그런 프로토타입이 업다.");
            
            _uint iObjListSize = (_uint)(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_NPC")).size();
            CGameObject* pNewGameObject = m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_NPC", iObjListSize - 1);
            m_iPickedID = pNewGameObject->Get_ID(); // 꼭 바꿔주깅. 피킹 후에 바로 아이디 바꿔줘서 바로 상태 제어 할 수 있게끔요.
        }

        if (ImGui::Button("NPC_Save"))
            Save_NPC(strString.c_str());
        ImGui::SameLine();
        if (ImGui::Button("NPC_Load"))
            Load_NPC(strString.c_str());
    }
    ImGui::End();
#pragma endregion
    //=======================================NPC End=======================================//
    // 
    //=======================================Shell=======================================//
#pragma region SHELL
    ImGui::Begin("SHELL");
    //맵트리거 추가 + 저장 + 불러오기
    {
        ImGui::SeparatorText("SHELL");

        ImGui::RadioButton("House", &m_iShellType, (_int)CMapTool_Item::SHELL::SHELL_HOME);
        ImGui::RadioButton("Soda", &m_iShellType, (_int)CMapTool_Item::SHELL::SHELL_SODACAN);
        ImGui::RadioButton("MirrorBall", &m_iShellType, (_int)CMapTool_Item::SHELL::SHELL_MIRRORBALL);
        ImGui::RadioButton("Duck", &m_iShellType, (_int)CMapTool_Item::SHELL::SHELL_DUCK);
        ImGui::RadioButton("Tennis", &m_iShellType, (_int)CMapTool_Item::SHELL::SHELL_TENNISBALL);
        // 아래 추가 했을 때 인덱스 설정하려고 올렸어용. 

        if (ImGui::Button("Add_Shell"))
        {
            CMapTool_Item::DESC Desc = {};
            Desc.eType = (CMapTool_Item::SHELL)m_iShellType;
            if (m_iShellType == 0)
                Desc.GameObjectDesc.strName = L"House"+ to_wstring(m_iCountShell++);
            else if (m_iShellType == 1)
                Desc.GameObjectDesc.strName = L"Soda" + to_wstring(m_iCountShell++);
            else if (m_iShellType == 2)
                Desc.GameObjectDesc.strName = L"MirrorBall" + to_wstring(m_iCountShell++);
            else if (m_iShellType == 3)
                Desc.GameObjectDesc.strName = L"Duck" + to_wstring(m_iCountShell++);
            else if (m_iShellType == 4)
                Desc.GameObjectDesc.strName = L"Tennis" + to_wstring(m_iCountShell++);

            if (FAILED(m_pGameInstance->
                Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapTool_Item", LEVEL_TOOL, L"Layer_Shell", &Desc)))
                MSG_BOX("추가 안됨.");

            _uint iObjListSize = (_uint)(m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_Shell")).size();
            CGameObject* pNewGameObject = m_pGameInstance->Find_GameObject(LEVEL_TOOL, L"Layer_Shell", iObjListSize - 1);
            m_iPickedID = pNewGameObject->Get_ID(); // 꼭 바꿔주깅. 피킹 후에 바로 아이디 바꿔줘서 바로 상태 제어 할 수 있게끔요.
            m_iShellSelected = (_int)(m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_Shell").size()) - 1;
        }

            list<CGameObject*> MapItemList = m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_Shell");
            auto it = MapItemList.begin();
            if(it != MapItemList.end())
                advance(it, m_iShellSelected); // 선택된 인덱스로 이동

            // 콤보 박스 생성
            if (ImGui::BeginCombo("Shell List", MapItemList.empty() ? "Empty Shell" : WStringToString((*it)->Get_Name()).c_str()))
            {
                _int i = 0;
                for (auto it = MapItemList.begin(); it != MapItemList.end(); ++it, ++i)
                {
                    _bool isSelected = (m_iShellSelected == i);
                    if (ImGui::Selectable(WStringToString((*it)->Get_Name()).c_str(), isSelected))
                    {
                        m_iShellSelected = i; // 선택된 인덱스 업데이트
                        m_iPickedID = (*it)->Get_ID();
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            if (ImGui::Button("Save_Shell"))
            {
                Save_MapShell();
            }
            ImGui::SameLine();
            if (ImGui::Button("Load_Shell"))
            {
                Load_MapShell();
            }
        }

    ImGui::End();
    

#pragma endregion
    //=======================================Shell END=======================================//

     
    //=======================================Item=======================================//
#pragma region ITEM
    ImGui::Begin("ITEM");
    //맵트리거 추가 + 저장 + 불러오기
    {
        ImGui::SeparatorText("ITEM");
        //m_szItembuffer
        m_szItembuffer[sizeof(m_szItembuffer) - 1] = '\0';  // 널 종료 보장

        ImGui::SetNextItemWidth(300.0f);
        if (ImGui::InputText("##ItemName", m_szItembuffer, IM_ARRAYSIZE(m_szItembuffer))) {}

        _int iSize = MultiByteToWideChar(CP_UTF8, 0, m_szItembuffer, -1, NULL, 0);
        _wstring strString(iSize, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, m_szItembuffer, -1, &strString[0], iSize);

        ImGui::Text("ItemName: %ws", strString.c_str());

        // 아래 추가 했을 때 인덱스 설정하려고 올렸어용. 


        if (ImGui::Button("Add_Item"))
        {
            CGameObject::DESC Desc{};
            Desc.strName = strString.c_str();
            if (FAILED(m_pGameInstance->
                Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapTool_Item", LEVEL_TOOL, L"Layer_MapItem", &Desc)))
                MSG_BOX("추가 안됨.");

            _uint iObjListSize = (_uint)(m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_MapItem")).size();
            CGameObject* pNewGameObject = m_pGameInstance->Find_GameObject(LEVEL_TOOL, L"Layer_MapItem", iObjListSize - 1);
            m_iPickedID = pNewGameObject->Get_ID(); // 꼭 바꿔주깅. 피킹 후에 바로 아이디 바꿔줘서 바로 상태 제어 할 수 있게끔요.
            m_iItemSelected = (_int)(m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_MapItem").size()) - 1;
        }

        list<CGameObject*> MapItemList = m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_MapItem");
        auto it = MapItemList.begin();
        advance(it, m_iItemSelected); // 선택된 인덱스로 이동

        // 콤보 박스 생성
        if (ImGui::BeginCombo("Item List", MapItemList.empty() ? "Empty Items" : WStringToString((*it)->Get_Name()).c_str()))
        {
            _int i = 0;
            for (auto it = MapItemList.begin(); it != MapItemList.end(); ++it, ++i)
            {
                _bool isSelected = (m_iItemSelected == i);
                if (ImGui::Selectable(WStringToString((*it)->Get_Name()).c_str(), isSelected))
                {
                    m_iItemSelected = i; // 선택된 인덱스 업데이트
                    m_iPickedID = (*it)->Get_ID();
                    ImGui::SetItemDefaultFocus();
                }

            }
            ImGui::EndCombo();
        }
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("Save_Item"))
        {
            Save_MapItem();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load_Item"))
        {
            Load_MapItem();
        }
    }
    ImGui::End();


#pragma endregion
    //=======================================Item End=======================================//

    //=======================================Monster=======================================//
#pragma region MONSTER
    ImGui::Begin("Monster");
    //맵트리거 추가 + 저장 + 불러오기
    {
        ImGui::SeparatorText("MONSTER");
        //m_szMonsterbuffer
        m_szMonsterbuffer[sizeof(m_szMonsterbuffer) - 1] = '\0';  // 널 종료 보장

        ImGui::SetNextItemWidth(300.0f);
        if (ImGui::InputText("##MonsterName", m_szMonsterbuffer, IM_ARRAYSIZE(m_szMonsterbuffer))) {}

        _int iSize = MultiByteToWideChar(CP_UTF8, 0, m_szMonsterbuffer, -1, NULL, 0);
        _wstring strString(iSize, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, m_szMonsterbuffer, -1, &strString[0], iSize);

        ImGui::Text("TrrigerName: %ws", strString.c_str());

        // 아래 추가 했을 때 인덱스 설정하려고 올렸어용. 


        if (ImGui::Button("Add_Monster"))
        {
            CGameObject::DESC Desc{};
            Desc.strName = strString.c_str();
            if (FAILED(m_pGameInstance->
                Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapTool_Monster", LEVEL_TOOL, L"Layer_Monster", &Desc)))
                MSG_BOX("추가 안됨.");

            _uint iObjListSize = (_uint)(m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_Monster")).size();
            CGameObject* pNewGameObject = m_pGameInstance->Find_GameObject(LEVEL_TOOL, L"Layer_Monster", iObjListSize - 1);
            m_iPickedID = pNewGameObject->Get_ID(); // 꼭 바꿔주깅. 피킹 후에 바로 아이디 바꿔줘서 바로 상태 제어 할 수 있게끔요.
            m_iMonsterSelected = (_int)(m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_Monster").size()) - 1;
        }

        list<CGameObject*> MonsterList = m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_Monster");
        auto it = MonsterList.begin();
        advance(it, m_iMonsterSelected); // 선택된 인덱스로 이동

        // 콤보 박스 생성
        if (ImGui::BeginCombo("Monster List", MonsterList.empty() ? "Empty Monsters" : WStringToString((*it)->Get_Name()).c_str()))
        {
            _int i = 0;
            for (auto it = MonsterList.begin(); it != MonsterList.end(); ++it, ++i)
            {
                _bool isSelected = (m_iMonsterSelected == i);
                if (ImGui::Selectable(WStringToString((*it)->Get_Name()).c_str(), isSelected))
                {
                    m_iMonsterSelected = i; // 선택된 인덱스 업데이트
                    m_iPickedID = (*it)->Get_ID();
                    ImGui::SetItemDefaultFocus();
                }

            }
            ImGui::EndCombo();
        }
        if (!MonsterList.empty())
        {
            ImGui::Checkbox("Position_Add", &m_bMonsterAdd);
            auto iter = next(MonsterList.begin(), m_iMonsterSelected);
            if (m_bMonsterAdd && m_pGameInstance->Mouse_Down(DIM_RB))
            {
                _float3 vPickPos = {};
                m_pGameInstance->Compute_PixelPickPos(vPickPos);
                static_cast<CMapTool_Monster*>(*iter)->Add_Position(vPickPos);
            }
            if (m_pGameInstance->Key_Press(KEY::G))
            {
                _float3 vPickPos = {};
                m_pGameInstance->Compute_PixelPickPos(vPickPos);
                static_cast<CMapTool_Monster*>(*iter)->Set_WorldPosition(vPickPos);

            }
            static_cast<CMapTool_Monster*>(*iter)->DebugRender();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            if (ImGui::Button("Save_Monster"))
            {
                Save_Monster();
            }
            ImGui::SameLine();
        }
        if (ImGui::Button("Load_Monster"))
        {
            Load_Monster();
        }
    }
    ImGui::End();
#pragma endregion
    //=======================================Monster End=======================================//


#pragma region LIGHT
    ImGui::Begin("Tool");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::SeparatorText("LIGHT");
    // Direction Light
    {
        LIGHT_DESC LightDesc = m_pGameInstance->Get_LightDesc(L"Light_Global");

        ImGui::DragFloat4("Dir", (_float*)(&LightDesc.vDirection), 0.01f, 0.f, 1.f, "%.2f");
        ImGui::DragFloat4("Dif", (_float*)(&LightDesc.vDiffuse), 0.01f, 0.f, 1.f, "%.2f");
        ImGui::DragFloat4("Amb", (_float*)(&LightDesc.vAmbient), 0.01f, 0.f, 1.f, "%.2f");

        m_pGameInstance->Set_LightDesc(L"Light_Global", LightDesc);

    }


    // Point Light
    {
        ImGui::SeparatorText("POINT_LIGHT");
        if (ImGui::Checkbox("PointLight", &m_bPointLightPicking))
        {
            if (m_bPointLightPicking)
            {
                m_bInstancing_Mesh = false;
                m_bSetMapObject = false;
            }
        }
        if (ImGui::RadioButton("Point", &m_iLightType, 1)) {}
        ImGui::SameLine();
        if (ImGui::RadioButton("Spot", &m_iLightType, 2)) {}

        if (m_iLightType == 2)
        {
            ImGui::DragFloat4("Direction", (_float*)&m_vDirection, 0.005f, -10.f, 10.f);
            ImGui::DragFloat2("CosCutOff", (_float*)&m_vCosCutOff, 0.001f, 0.f, 90.f);
        }

        ImGui::DragFloat("Range", &m_fRange, 0.05f, 0.01f, 100.f);
        ImGui::DragFloat4("Position", (_float*)&m_vPosition, 0.01f, -50.f, 1024.f);
        ImGui::ColorEdit4("Diffuse", (_float*)&m_vDiffuse);
        ImGui::ColorEdit4("Ambient", (_float*)&m_vAmbient);
        ImGui::ColorEdit4("Specular", (_float*)&m_vSpecular);
        ImGui::DragFloat3("Attenuation", (_float*)&m_vAttenuation, 0.0001f, 0.f, 1.f, "%.4f");
        

        if (!m_ToolLightList.empty() && m_bPointLightPicking)
        {
            /* Guizmo */
            _matrix ResultMatrix = {};
            _matrix GuizmoMatrix = {};

            _vector vLook = XMVector3Normalize(XMLoadFloat4(&m_vDirection));
            // 1. Look이 Y축에 거의 수직일 때는 Up을 Z축으로 바꿔줌
            _vector vWorldUp;
            if (fabsf(XMVectorGetY(vLook)) > 0.99f)  // 위나 아래를 거의 정면으로 보는 경우
                vWorldUp = XMVectorSet(0.f, 0.f, 1.f, 0.f); // Z축 Up 사용
            else
                vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f); // 일반적으로 Y축 Up

            // 2. 오른쪽 벡터: Up Look 외적
            _vector vRight = XMVector3Normalize(XMVector3Cross(vWorldUp, vLook));

            // 3. 위쪽 벡터: Look Right 외적
            _vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

            _matrix RotMatrix = XMMatrixIdentity();
            RotMatrix.r[0] = vRight;
            RotMatrix.r[1] = vUp;
            RotMatrix.r[2] = vLook;

            GuizmoMatrix = XMMatrixScaling(m_fRange, m_fRange, m_fRange)
                * RotMatrix
                * XMMatrixTranslation(m_vPosition.x, m_vPosition.y, m_vPosition.z);

            _float4x4 PointLightMatrix = {};
            XMStoreFloat4x4(&PointLightMatrix, GuizmoMatrix);
            Handle_Manipulate(PointLightMatrix); // 기즈모 코드ㅊ

            ResultMatrix = XMLoadFloat4x4(&PointLightMatrix);

            _vector vScale, vRotQuat, vTrans;
            XMMatrixDecompose(&vScale, &vRotQuat, &vTrans, ResultMatrix);


            m_fRange = XMVectorGetX(vScale);

            _vector vDefaultLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
            _vector vNewLook = XMVector3Normalize(XMVector3Rotate(vDefaultLook, vRotQuat));
            XMStoreFloat4(&m_vDirection, vNewLook);

            XMStoreFloat4(&m_vPosition, vTrans);
            /* Guizmo End*/

            _float3 vPos = {};
            vPos.x = m_vPosition.x;
            vPos.y = m_vPosition.y;
            vPos.z = m_vPosition.z;
            m_pGameInstance->Add_DebugRender_Sphere(vPos, m_fRange, _vec4(1.f, 0.5f, 1.f, 1.f));

            LIGHT_DESC LightDesc = {};
            LightDesc.eType = (LIGHT_TYPE)m_iLightType;
            LightDesc.fRange = m_fRange;
            LightDesc.vPosition = m_vPosition;
            LightDesc.vDiffuse = m_vDiffuse;
            LightDesc.vAmbient = m_vAmbient;
            //LightDesc.vSpecular = m_vSpecular;
            LightDesc.vAttenuation = m_vAttenuation;

            if (m_iLightType == 2)
            {
                LightDesc.vCosCutOff = {};
                LightDesc.vCosCutOff.x = cosf(XMConvertToRadians(m_vCosCutOff.x));
                LightDesc.vCosCutOff.y = cosf(XMConvertToRadians(m_vCosCutOff.y));
                //LightDesc.vCosCutOff = m_vCosCutOff;

                LightDesc.vDirection = m_vDirection;
            }
            m_pGameInstance->Set_LightDesc(m_ToolLightList[m_iLightSelected].c_str(), LightDesc);
        }

        //문자열 입력 함수ㅠ
        m_szLightbuffer[sizeof(m_szLightbuffer) - 1] = '\0';  // 널 종료 보장

        ImGui::SetNextItemWidth(300.0f);
        if (ImGui::InputText("LightTag", m_szLightbuffer, IM_ARRAYSIZE(m_szLightbuffer))) {}

        _int iSize = MultiByteToWideChar(CP_UTF8, 0, m_szLightbuffer, -1, NULL, 0);
        _wstring strString(iSize, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, m_szLightbuffer, -1, &strString[0], iSize);


        /* 콤보바긋 */
        auto it = m_ToolLightList.begin();
        if(it != m_ToolLightList.end())
            advance(it, m_iLightSelected); // 선택된 인덱스로 이동

        // 콤보 박스 생성
        if (ImGui::BeginCombo("Light List", m_ToolLightList.empty() ? "Empty Light" : WStringToString(*it).c_str()))
        {
            _int i = 0;
            for (auto it = m_ToolLightList.begin(); it != m_ToolLightList.end(); ++it, ++i)
            {
                _bool isSelected = (m_iLightSelected == i);
                if (ImGui::Selectable(WStringToString(*it).c_str(), isSelected))
                {
                    m_iLightSelected = i; // 선택된 인덱스 업데이트
                    ImGui::SetItemDefaultFocus();

                    LIGHT_DESC Desc = m_pGameInstance->Get_LightDesc((*it).c_str());
                    m_iLightType = (_int)Desc.eType;
                    m_fRange = Desc.fRange;
                    m_vPosition = Desc.vPosition;
                    m_vDiffuse = Desc.vDiffuse;
                    m_vAmbient = Desc.vAmbient;
                    m_vAttenuation = Desc.vAttenuation;
                    if ((_int)Desc.eType == 2)
                    {
                        m_vCosCutOff.x = XMConvertToDegrees(acosf(Desc.vCosCutOff.x));
                        m_vCosCutOff.y = XMConvertToDegrees(acosf(Desc.vCosCutOff.y));

                        m_vDirection = Desc.vDirection;
                    }
                }

            }
            ImGui::EndCombo();
        }
        if (ImGui::Button("Delete_Light"))
        {
            for (auto it = m_ToolLightList.begin(); it != m_ToolLightList.end(); )
            {
                if (*it == m_ToolLightList[m_iLightSelected])
                {
                    m_pGameInstance->Remove_Light(it->c_str());
                    it = m_ToolLightList.erase(it);
                    if(m_iLightSelected != 0)
                        m_iLightSelected--;
                    if (m_ToolLightList.empty())
                        break;

                    LIGHT_DESC Desc = m_pGameInstance->Get_LightDesc(m_ToolLightList[m_iLightSelected].c_str());
                    m_iLightType = (_int)Desc.eType;
                    m_fRange = Desc.fRange;
                    m_vPosition = Desc.vPosition;
                    m_vDiffuse = Desc.vDiffuse;
                    m_vAmbient = Desc.vAmbient;
                    m_vAttenuation = Desc.vAttenuation;

                    if ((_int)Desc.eType == 2)
                    {
                        m_vCosCutOff.x = XMConvertToDegrees(acosf(Desc.vCosCutOff.x));
                        m_vCosCutOff.y = XMConvertToDegrees(acosf(Desc.vCosCutOff.y));

                        m_vDirection = Desc.vDirection;
                    }
                    break;
                }
                else
                    ++it;
            }

        }
        ImGui::SameLine();
        if (ImGui::Button("Add_Light") || m_pGameInstance->Mouse_Down(DIM_RB))
        {
            if (m_bPointLightPicking)
            {
                _bool bAdd = true;
                _float3 vPickPos = {};

                m_pGameInstance->Compute_PixelPickPos(vPickPos);
                m_vPosition.x = vPickPos.x;
                m_vPosition.y = vPickPos.y;
                m_vPosition.z = vPickPos.z;

                LIGHT_DESC LightDesc = {};
                LightDesc.eType = (LIGHT_TYPE)m_iLightType;
                LightDesc.fRange = m_fRange;
                LightDesc.vPosition = m_vPosition;
                LightDesc.vDiffuse = m_vDiffuse;
                LightDesc.vAmbient = m_vAmbient;
                //LightDesc.vSpecular = m_vSpecular;
                LightDesc.vAttenuation = m_vAttenuation;

                if (m_iLightType == 2)
                {
                    LightDesc.vCosCutOff = {};
                    LightDesc.vCosCutOff.x = cosf(XMConvertToRadians(m_vCosCutOff.x));
                    LightDesc.vCosCutOff.y = cosf(XMConvertToRadians(m_vCosCutOff.y));

                    LightDesc.vDirection = m_vDirection;
                }

                if (FAILED(m_pGameInstance->Add_Light(strString.c_str(), LightDesc)))
                {
                    bAdd = false;
                }

                if (bAdd)
                {
                    m_ToolLightList.push_back(strString);
                    m_iLightSelected = (_int)m_ToolLightList.size() -1;
                }
            }
        }


        if (ImGui::Button("Save_Light"))
        {
            _wstring szFile = {};
            /*다이얼로그 띄우기*/
            CDialog* pDialog = CDialog::Create(CDialog::TYPE::SAVE);
            pDialog->Set_FileType(CDialog::FILE_TYPE::LIGHT);
            pDialog->Set_DefaultExt(L"light");

            if (SUCCEEDED(pDialog->Show_Dialog()))
            {
                szFile = (pDialog->Get_FilePath());
            }
            Safe_Release(pDialog);
            /*다이얼로그 띄우기(끝)*/

            //로드하기
            HRESULT hr{};
            DWORD dwByte{};
            HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

            if (INVALID_HANDLE_VALUE == hFile)
            {
                MSG_BOX("Can Not Open File");
            }
            m_pGameInstance->Save_Light(hFile, (LIGHT_TYPE)m_iLightType);

            CloseHandle(hFile);
        }


        ImGui::SameLine();

        if (ImGui::Button("Load_Light"))
        {
            _wstring szFile = {}; //다이얼로그로 가져오는 파일패스 저장할 곳
            /*다이얼로그 띄우기*/
            CDialog* pDialog = CDialog::Create(CDialog::TYPE::LOAD);
            pDialog->Set_FileType(CDialog::FILE_TYPE::LIGHT);
            pDialog->Set_DefaultExt(L"light");

            if (SUCCEEDED(pDialog->Show_Dialog()))
            {
                szFile = (pDialog->Get_FilePath());
            }
            Safe_Release(pDialog);
            /*다이얼로그 띄우기(끝)*/

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
                m_ToolLightList.push_back(strLightName.c_str());
            }
            m_iLightSelected = (_int)m_ToolLightList.size() - 1;
            CloseHandle(hFile);
        }
    }
#pragma endregion

    _float4x4 ViewMat = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW);
    _float4x4 ProjMat = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ);
    //ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());// 이건 임구 창 안에만 그려지는 코드

    // 카메라 : 스피드 조절
    {
        ImGui::SeparatorText("Camera");
        static _float fInputValue = 0.f;
        ImGui::Text("CameraSpeed");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100.f);
        ImGui::InputFloat("##HiddenLabel", &fInputValue);
        ImGui::SameLine();
        if (ImGui::Button("Apply"))
        {
            m_fSpeed = fInputValue;  // 버튼을 누르면 myValue에 적용
            m_pCamera->Set_Speed(m_fSpeed);
        }
        ImGui::Spacing();
    }
    //메쉬 인스턴싱
    {
        ImGui::SeparatorText("Instancing_Mesh");

        if(ImGui::Checkbox("Instancing_Mesh", &m_bInstancing_Mesh))
        {
            if (m_bInstancing_Mesh)
            {
                m_bPointLightPicking = false;
                m_bSetMapObject = false;
            }
        }
        ImGui::Spacing();
        ImGui::Separator();

        if (m_bInstancing_Mesh == true)
        {
            m_pGameInstance->Compute_PixelPickPos(m_vCenter);

            ImGui::InputInt("NumInstancing", &m_iNumInstancing);
            ImGui::InputFloat3("Center", (_float*)&m_vCenter, "%.2f");
            ImGui::InputFloat3("Range", (_float*)&m_vRange, "%.2f");
            ImGui::InputFloat2("Size", (_float*)&m_vSize);
            
            _mat WorldMat;
            WorldMat.right *= (m_vRange.x * 0.5f);
            WorldMat.up *= (m_vRange.y * 0.5f);
            WorldMat.look *= (m_vRange.z * 0.5f);
            WorldMat.position = _vec4(m_vCenter, 1.f);

            m_pGameInstance->Add_DebugRender_Box(WorldMat, _vec4(1.f, 0.f, 1.f, 1.f));

            if (ImGui::RadioButton("SeaWeeds", &m_iCurInstancingObject, 0)) {}
            ImGui::SameLine();
            if (ImGui::RadioButton("BigKelp", &m_iCurInstancingObject, 1)) {}
            ImGui::SameLine();
            if (ImGui::RadioButton("SeaGrass00", &m_iCurInstancingObject, 2)) {}

            if (ImGui::RadioButton("KelpGround", &m_iCurInstancingObject, 3)) {}
            ImGui::SameLine();

            if (m_bPickInstancing)
            {
                _float4x4 WorldMat = {};
                switch (m_iCurInstancingObject)
                {
                case 0:
                    if (true == m_pSeaWeeds->Get_PickInstanceMesh_WorldMatrix(WorldMat))
                    {
                        Handle_Manipulate(WorldMat);
                        m_pSeaWeeds->Set_PickInstanceMesh_WorldMatrix(WorldMat);
                    }
                    break;
                case 1:
                    if (true == m_pKelpBig->Get_PickInstanceMesh_WorldMatrix(WorldMat))
                    {
                        Handle_Manipulate(WorldMat);
                        m_pKelpBig->Set_PickInstanceMesh_WorldMatrix(WorldMat);
                    }
                    break;
                case 2:
                    if (true == m_pSeaGrass00->Get_PickInstanceMesh_WorldMatrix(WorldMat))
                    {
                        Handle_Manipulate(WorldMat);
                        m_pSeaGrass00->Set_PickInstanceMesh_WorldMatrix(WorldMat);
                    }
                    break;
                case 3:
                    if (true == m_pKelpGround->Get_PickInstanceMesh_WorldMatrix(WorldMat))
                    {
                        Handle_Manipulate(WorldMat);
                        m_pKelpGround->Set_PickInstanceMesh_WorldMatrix(WorldMat);
                    }
                    break;
                }
            }
            if (m_pGameInstance->Mouse_Down(DIM_RB))
            {
                if (m_iNumInstancing >= 0 && m_vRange.x >= 0.f && m_vSize.x >= 0.f)
                {
                    switch (m_iCurInstancingObject)
                    {
                    case 0:
                        m_pSeaWeeds->Input_InstancingInfo(m_iNumInstancing, m_vCenter, m_vRange, m_vSize);
                        break;
                    case 1:
                        m_pKelpBig->Input_InstancingInfo(m_iNumInstancing, m_vCenter, m_vRange, m_vSize);
                        break;
                    case 2:
                        m_pSeaGrass00->Input_InstancingInfo(m_iNumInstancing, m_vCenter, m_vRange, m_vSize);
                        break;
                    case 3:
                        m_pKelpGround->Input_InstancingInfo(m_iNumInstancing, m_vCenter, m_vRange, m_vSize);
                        break;
                    }
                    
                }
            }
            if (m_pGameInstance->Mouse_Down(DIM_MB))
            {
                m_bInstancing_Mesh = false;
            }

            if (m_pGameInstance->Mouse_Down(DIM_LB))
            {
                if (!ImGuizmo::IsUsing())
                {
                    _float4x4 PickingInstanceMatrix = {};

                    _float3 vPickedPos = {};
                    m_pGameInstance->Compute_PixelPickPos(vPickedPos); // 피킹된 위치 가져오기 

                    switch (m_iCurInstancingObject)
                    {
                    case 0:
                        if (true == m_pSeaWeeds->Find_PickedInstance_WorldMatrix(vPickedPos, PickingInstanceMatrix))
                        {
                            m_bPickInstancing = true;
                            break;
                        }
                        m_bPickInstancing = false;
                        break;
                    case 1:
                        if (true == m_pKelpBig->Find_PickedInstance_WorldMatrix(vPickedPos, PickingInstanceMatrix))
                        {
                            m_bPickInstancing = true;
                            break;
                        }
                        m_bPickInstancing = false;
                        break;
                    case 2:
                        if (true == m_pSeaGrass00->Find_PickedInstance_WorldMatrix(vPickedPos, PickingInstanceMatrix))
                        {
                            m_bPickInstancing = true;
                            break;
                        }
                        m_bPickInstancing = false;
                        break;
                    case 3:
                        if (true == m_pKelpGround->Find_PickedInstance_WorldMatrix(vPickedPos, PickingInstanceMatrix))
                        {
                            m_bPickInstancing = true;
                            break;
                        }
                        m_bPickInstancing = false;
                        break;
                    }
                }
            }

            if (ImGui::Button("Instancing Save"))
            {
                _wstring szFile = {}; //다이얼로그로 가져오는 파일패스 저장할 곳
                /*다이얼로그 띄우기*/
                CDialog* pDialog = CDialog::Create(CDialog::TYPE::SAVE);
                pDialog->Set_FileType(CDialog::FILE_TYPE::INSTANCING);
                pDialog->Set_DefaultExt(L"instancing");

                if (SUCCEEDED(pDialog->Show_Dialog()))
                {
                    szFile = (pDialog->Get_FilePath());
                }
                Safe_Release(pDialog);
                /*다이얼로그 띄우기(끝)*/

                HRESULT hr{};
                DWORD dwByte{};
                HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

                if (INVALID_HANDLE_VALUE == hFile)
                {
                    MSG_BOX("Can Not Open File");
                    CloseHandle(hFile);
                }

                switch (m_iCurInstancingObject)
                {
                case 0:
                    m_pSeaWeeds->Save_InstancingFile(hFile);
                    break;
                case 1:
                    m_pKelpBig->Save_InstancingFile(hFile);
                    break;
                case 2:
                    m_pSeaGrass00->Save_InstancingFile(hFile);
                    break;
                case 3:
                    m_pKelpGround->Save_InstancingFile(hFile);
                    break;
                }

                CloseHandle(hFile);
            }

            if (ImGui::Button("Instancing_PrevDelete"))
            {
                switch (m_iCurInstancingObject)
                {
                case 0:
                    m_pSeaWeeds->Delete_PrevInstancing(m_iNumInstancing);
                    break;
                case 1:
                    m_pKelpBig->Delete_PrevInstancing(m_iNumInstancing);
                    break;
                case 2:
                    m_pSeaGrass00->Delete_PrevInstancing(m_iNumInstancing);
                    break;
                case 3:
                    m_pKelpGround->Delete_PrevInstancing(m_iNumInstancing);
                    break;
                }
            }
        }
    }
#pragma region ObjectTool
    // 현재 ObjectID 출력 + 오브젝트 피킹 상태 제어
    {
        ImGui::Spacing();
        ImGui::SeparatorText("OBJECT");
        ImGui::Spacing();
        _bool isImguiHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered();

        ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Picking ID: %d", m_iPickedID);
        if (ImGui::Checkbox("MapObjectSetting", &m_bSetMapObject))
        {
            if (m_bSetMapObject)
            {
                m_bPointLightPicking = false;
                m_bInstancing_Mesh = false;
            }
        }
        if (m_bSetMapObject && m_iPickedID != 0 && m_iPickedID != 2/* 0은 sky, 2는Terrain임*/)
        {
            ImGui::SeparatorText("Object_Interaction");
            CGameObject* pPickedGameObject = m_pGameInstance->Find_GameObject_With_ID(m_pGameInstance->Get_CurrentLevelIndex(), m_iPickedID);
            CTransform* pTransform = static_cast<CTransform*>(pPickedGameObject->Find_Component(g_strTransformTag));
            //피킹한 오브젝트의 Transform을 가져옴
            CModel* pModel = static_cast<CModel*>(pPickedGameObject->Find_Component(L"Com_Model"));
            //ID로 얻은 모델이에요
            if (pModel != nullptr)
            {

                ImGui::Separator();
                if (m_iCurMeshIdx == -1) //else에서 안누르면은 -1로 바꿔주고 있어여 
                {
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
                            || strName == "WormCoral_LOD0" || strName == "TubeCoral_01_LOD0_A.002" || strName == "TubeCoral_01_LOD0_A.001"
                            )
                        {
                            m_iCurMeshIdx = i; // 동일하면 인덱스 넣어줘여
                            m_iCurMaterialIdx = pModel->Get_MaterialIndex_In_Mesh(m_iCurMeshIdx); //메쉬안에 메테리얼 인덱스찾어
                            m_vBaseColor = pModel->Get_BaseColor(m_iCurMaterialIdx); // 베이스컬러 가져와여
                            break;
                        }

                    }

                }
                else
                {
                    ImGui::ColorEdit4("##BaseColor", (_float*)&m_vBaseColor, ImGuiColorEditFlags_Float); // 다른데 클릭하지 아느면은 계속 베이스컬러 세팅해줘여 
                    pModel->Set_BaseColor(m_iCurMaterialIdx, m_vBaseColor);
                    static_cast<CMapObject*>(pPickedGameObject)->Set_BaseColor(m_vBaseColor);
                }

                m_fFrustumRange = static_cast<CMapObject*>(pPickedGameObject)->Get_FrustumRange();
                m_iPhysicsState = static_cast<CMapObject*>(pPickedGameObject)->Get_PhysicState();

                ImGui::DragFloat("FrustumRange", (_float*)&m_fFrustumRange, 0.001f, 0.001f, 2000.f, " % .2f");
                if (ImGui::RadioButton("NONE", &m_iPhysicsState, 0)) {}
                ImGui::SameLine();
                if (ImGui::RadioButton("CONVEX", &m_iPhysicsState, 1)) {}
                ImGui::SameLine();
                if (ImGui::RadioButton("TRIANGLE", &m_iPhysicsState, 2)) {}

 
                static_cast<CMapObject*>(pPickedGameObject)->Set_FrustumRange(m_fFrustumRange);
                static_cast<CMapObject*>(pPickedGameObject)->Set_PhysicState(m_iPhysicsState);
            }
                _vector vObjPos = pTransform->Get_State(CTransform::STATE::POSITION);
                _float3 vObjScale = pTransform->Get_Scale();

                //ImGuizmo
                _float4x4   fWorldMat = {};
                XMStoreFloat4x4(&fWorldMat, pTransform->Get_WorldMatrix());

                ImGui::SeparatorText("Picking");

               
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();


                if (ImGui::RadioButton("Scale", &m_iGizmoSRTType, 0)) {}
                ImGui::SameLine();
                if (ImGui::RadioButton("Rotation", &m_iGizmoSRTType, 1)) {}
                ImGui::SameLine();
                if (ImGui::RadioButton("Translation", &m_iGizmoSRTType, 2)) {}


                Handle_Manipulate(fWorldMat);
                //ImGuizmo 끝

                m_bUseImGuizmo = ImGuizmo::IsUsing();
                _matrix vWorldMat = XMLoadFloat4x4(&fWorldMat);


                // Scale Position을 가지고 온다.

                _float3 vPos = {};
                XMStoreFloat3(&vPos, vWorldMat.r[3]);

                ImGui::DragFloat3("##Scale", (float*)&vObjScale, 0.001f, 0.001f, 100.f, "%.2f");
                ImGui::SameLine();
                ImGui::Text("Scale");
                pTransform->Set_Scale(vObjScale.x, vObjScale.y, vObjScale.z); //스케일은 바로 적용 해줘야 회전할때 문제 안색ㅇ겨요
                //로테이션 코드 여깄어요 네 

                if (ImGuizmo::IsUsing())
                {
                    pTransform->Set_State(CTransform::STATE::RIGHT, vWorldMat.r[0]);
                    pTransform->Set_State(CTransform::STATE::UP, vWorldMat.r[1]);
                    pTransform->Set_State(CTransform::STATE::LOOK, vWorldMat.r[2]);
                    pTransform->Set_State(CTransform::STATE::POSITION, vWorldMat.r[3]);
                }

#pragma region Rataion 코드가 존나 길어요 
                const _float4x4& WorldMatrix = pTransform->Get_WorldMatrix_Ref();
                // 스케일? 
                float pitch, yaw, roll;

                _float fZ_Up = WorldMatrix.m[2][1];

                // Compute pitch (rotation around X-axis)
                pitch = std::asin(-WorldMatrix.m[2][1]);

                // Check for gimbal lock
                if (std::abs(WorldMatrix.m[2][1]) < 0.9999f)
                {
                    // Compute yaw (rotation around Y-axis) and roll (rotation around Z-axis)
                    yaw = std::atan2(WorldMatrix.m[2][0], WorldMatrix.m[2][2]);
                    roll = std::atan2(WorldMatrix.m[0][1], WorldMatrix.m[1][1]);
                }
                else
                {
                    // Gimbal lock case
                    yaw = 0.0f; // Can set to 0 or some default value
                    roll = std::atan2(-WorldMatrix.m[1][0], WorldMatrix.m[0][0]);
                }

                // Return the Euler angles (pitch, yaw, roll)
                _float3 vRotation = _float3(pitch, yaw, roll);
                _float3 vRotationDegree = vRotation;
                vRotationDegree.x = XMConvertToDegrees(vRotation.x);
                vRotationDegree.y = XMConvertToDegrees(vRotation.y);
                vRotationDegree.z = XMConvertToDegrees(vRotation.z);

                _float3 vPreRotation = vRotationDegree;
                ImGui::DragFloat3("##Rotation", (float*)&vRotationDegree, 0.1f);

                if (abs(vPreRotation.x - vRotationDegree.x) > 0.005f ||
                    abs(vPreRotation.y - vRotationDegree.y) > 0.005f ||
                    abs(vPreRotation.z - vRotationDegree.z) > 0.005f)
                {
                    // 값에 유효한 변화가 있을 경우만 저장하기 
                    pTransform->Rotate(vRotationDegree.x, vRotationDegree.y, vRotationDegree.z);
                }
                ImGui::SameLine();
                ImGui::Text("Rotation");
#pragma endregion        
                ImGui::DragFloat3("##Position", (float*)&vPos, 0.01f, -100.f, 100.f, "%.2f");
                ImGui::SameLine();
                ImGui::Text("Position");

                // 가져온 값으로 조절을 하니간은 기존 값이 임구이에서 계속 유지됨 
                if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::DIM_RB) && !m_bPointPicking)
                {
                    m_pGameInstance->Compute_PixelPickPos(vPos); // 오른쪽 마우스를 누르면 바로 이동 시키깅 ㅋㅋ
                    pTransform->Set_State(CTransform::STATE::POSITION, XMLoadFloat3(&vPos));
                }
                else 
                    pTransform->Set_State(CTransform::STATE::POSITION, XMLoadFloat3(&vPos));

                if (ImGui::Button("PickingOBJ_Delete") || m_pGameInstance->Key_Down(KEY::DELETEKEY))
                {
                    m_pGameInstance->Delete_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Environment"), pPickedGameObject);
                    m_pGameInstance->Delete_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_MapTrigger"), pPickedGameObject);
                    m_pGameInstance->Delete_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_MapItem"), pPickedGameObject);
                    m_pGameInstance->Delete_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Insteraction_Environment"), pPickedGameObject);
                    m_pGameInstance->Delete_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_NPC"), pPickedGameObject);
                    m_pGameInstance->Delete_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Shell"), pPickedGameObject);
                    m_iPickedID = 0;
                    m_iTriggerSelected = 0;
                }
            
                if (m_pGameInstance->Key_Press(KEY::LCONTROL))
                {
                    if (m_pGameInstance->Key_Down(KEY::C))
                    {
                        XMStoreFloat4x4(&m_CopyWorldMat, pTransform->Get_WorldMatrix());
                        m_strName = pPickedGameObject->Get_Name();
                    }
                    if (m_pGameInstance->Key_Down(KEY::V))
                    {
                        CGameObject::DESC Desc = {};
                        Desc.WorldMatrix = m_CopyWorldMat;
                        Desc.strName = m_strName;
                        if (SUCCEEDED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapObject", m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Environment"), &Desc)))
                        {
                            _uint iObjListSize = (_uint)(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Environment")).size();
                            CGameObject* pNewGameObject = m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Environment", iObjListSize - 1);
                            m_iPickedID = pNewGameObject->Get_ID(); // 꼭 바꿔주깅. 피킹 후에 바로 아이디 바꿔줘서 바로 상태 제어 할 수 있게끔요.
                        }
                    }
                }
        }
        else
        {
            m_iCurMeshIdx = -1;
        }

        if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::DIM_LB) && !isImguiHovered && m_bSetMapObject)
        {

            if (m_bUseImGuizmo == false)
            {
                _uint iOut = 0;
                if (true == m_pGameInstance->Compute_ObjectID(iOut))
                    m_iPickedID = iOut;
            }
        }
        ImGui::Spacing();
    }

    //인터렉션 오브젝트 생성 !생성 시에 무조건 피킹ID 생성한 새끼로 넣어야겠는디?
    {

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::SeparatorText("INTERACTION OBJECT");
        ImGui::SetNextItemWidth(110.f);
        const char* items[] = { "ShellHouse", "GrappleNeedle", "HeartKelp", "Urchin", "RotatePipe_T",
            "RotatePipe_TFloor","StreamVent","Sponge","FakeMoon","Laser","Spong","NonMeshCollider"}; // 드롭다운 항목
        if (ImGui::Combo("##ObjectName", &m_iCurObjectIdx, items, IM_ARRAYSIZE(items))) {}

        //m_szInteractionbuffer[sizeof(m_szInteractionbuffer) - 1] = '\0';  // 널 종료 보장

        //ImGui::SetNextItemWidth(300.0f);
        //if (ImGui::InputText("##InteractionName", m_szInteractionbuffer, IM_ARRAYSIZE(m_szInteractionbuffer))) {} 

        //_int iSize = MultiByteToWideChar(CP_UTF8, 0, m_szInteractionbuffer, -1, NULL, 0);
        //_wstring strString(iSize, L'\0');
        //MultiByteToWideChar(CP_UTF8, 0, m_szInteractionbuffer, -1, &strString[0], iSize);

        //ImGui::Text("Itr_Name: %ws", strString.c_str());

        // 아래 추가 했을 때 인덱스 설정하려고 올렸어용. 

        ImGui::InputInt("SheleportID :", &m_iShell_ID, 1);
        if (ImGui::Button("ItrObj_Add"))
        {
            CShellHouse::DESC Desc = {};
            //Desc.strName = strString.c_str();


            _wstring strPrototypeTag = L"";
            switch (m_iCurObjectIdx)
            {
            case 0:
                Desc.iSheleportID = m_iShell_ID;
                Desc.GameObjectDesc.strName = L"Prototype_GameObject_ShellHouse";
                strPrototypeTag = L"Prototype_GameObject_ShellHouse";
                break;
            case 1:
                Desc.GameObjectDesc.strName = L"Prototype_GameObject_GrappleNeedle";
                strPrototypeTag = L"Prototype_GameObject_GrappleNeedle";
                break;
            case 2:
                Desc.GameObjectDesc.strName = L"Prototype_GameObject_HeartKelp";
                strPrototypeTag = L"Prototype_GameObject_HeartKelp";
                break;
            case 3:
                Desc.GameObjectDesc.strName = L"Prototype_GameObject_Urchin";
                strPrototypeTag = L"Prototype_GameObject_Urchin";
                break;
            case 4 :
                Desc.GameObjectDesc.strName = L"Prototype_GameObject_RotatePipe";
                strPrototypeTag = L"Prototype_GameObject_RotatePipe";
                break;
            case 5:
                Desc.GameObjectDesc.strName = L"Prototype_GameObject_RotatePipeFloor";
                strPrototypeTag = L"Prototype_GameObject_RotatePipeFloor";
                break;
            case 6:
                Desc.GameObjectDesc.strName = L"Prototype_GameObject_StreamVent";
                strPrototypeTag = L"Prototype_GameObject_StreamVent";
                break;
            case 7:
                Desc.GameObjectDesc.strName = L"Prototype_GameObject_Sponge";
                strPrototypeTag = L"Prototype_GameObject_Sponge";
                break;
            case 8:
                Desc.GameObjectDesc.strName = L"Prototype_GameObject_FakeMoonBrick";
                strPrototypeTag = L"Prototype_GameObject_FakeMoonBrick";
                break;
            case 9:
                Desc.GameObjectDesc.strName = L"Prototype_GameObject_Laser";
                strPrototypeTag = L"Prototype_GameObject_Laser";
                break;
            case 10:
                Desc.GameObjectDesc.strName = L"Prototype_GameObject_Spong";
                strPrototypeTag = L"Prototype_GameObject_Spong";
                break;
            case 11:
                Desc.GameObjectDesc.strName = L"Prototype_GameObject_NonMeshCollider";
                strPrototypeTag = L"Prototype_GameObject_NonMeshCollider";
                break;
            }
            if (SUCCEEDED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, strPrototypeTag, m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Insteraction_Environment"), &Desc)))
            {
                _uint iObjListSize = (_uint)(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Environment")).size();
                CGameObject* pNewGameObject = m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Environment", iObjListSize - 1);
                m_iPickedID = pNewGameObject->Get_ID(); // 꼭 바꿔주깅. 피킹 후에 바로 아이디 바꿔줘서 바로 상태 제어 할 수 있게끔요.
            }
        }
    }


    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::SeparatorText("NONE INTERACTION OBJECT");
    ImGui::SetNextItemWidth(110.f);
    const char* items[] = { "WRITE_NAME", "PLAYER", "ALCHOHOLBUILDING", "LARGE_PLATEAU", "MOONSHELL"
                          , "CYLINDER00", "CYLINDER01", "CYLINDER02","CYLINDER03", "NCCITY_SHELFCORAL"
                          , "CORALTREE_00", "CORALTREE_01", "CORALTREE_02", "CORALTREE_03", "CORALTREE_04", "CORALTREE_05"
                          , "CORALBRANCH", "TUBECORAL_00",  "TUBECORAL_01"
                          , "SHELFCORAL_01", "SHELFCORAL_02", "SHELFCORAL_03", "SHELFCORAL_04", "SHELFCORAL_05"
                          , "BRIDGE", "GIN", "WHISKEY", "RECEIPTROAD"
                          , "TOPSIDE00", "TOPSIDE01", "TOPSIDE02","TOPSIDE03","TOPSIDE04","PLANE004","PLANE005","PLANE006"

                          , "WOODPLATE","METALCHAIR", "BUTTERTUBTOP", "BUTTERTUB", "TRASHCANTOP", "TRASHCAN"
                          , "RUSTSEACHAIN", "SEACHAIN", "DUMPSTER", "ANCHOR", "SPIRE", "LARGEKELPTREE"
                          , "CLUSTER_LARGE00", "CLUSTER_LARGE01", "CLUSTER_MEDIUM", "CLUSTER_SMALL", "PLATFORMYROCK00", "PLATFORMYROCK01"
                          , "LARGE_SHARP_CLIFF", "DUNEFENCE00", "LIFERING", "TANK", "HOLETANK", "TIRE", "TRASHPILE00", "TRASHPILE01", "TRASHPILE02", "COOLER", "COOLERTOP"
    }; // 드롭다운 항목
    //strncpy(buffer, utf8Text.c_str(), sizeof(buffer));
    m_szObjectbuffer[sizeof(m_szObjectbuffer) - 1] = '\0';  // 널 종료 보장

    ImGui::SetNextItemWidth(300.0f);
    if (ImGui::InputText("##PrototypeName", m_szObjectbuffer, IM_ARRAYSIZE(m_szObjectbuffer))) {}

    _int iSize = MultiByteToWideChar(CP_UTF8, 0, m_szObjectbuffer, -1, NULL, 0);
    _wstring strString(iSize, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, m_szObjectbuffer, -1, &strString[0], iSize);

    ImGui::Text("Input: %ws", strString.c_str()); // 잘됐는지 확ㅇ닌마하는 용됴

    if (ImGui::Combo("##NoneObjectName", &m_iCurMapObjectIdx, items, IM_ARRAYSIZE(items))) {}

    if (ImGui::Button("Object_Delete"))
    {
        CGameObject* pDeleteObject = nullptr;
        pDeleteObject = m_pGameInstance->Find_GameObject_With_Name(LEVEL_TOOL, L"Layer_Environment", strString.c_str());
        m_pGameInstance->Delete_GameObject(LEVEL_TOOL, L"Layer_Environment", pDeleteObject);

    }
    if (ImGui::Button("Object_Add"))
    {
        CMapObject::DESC Desc = {};
        ImGui::SameLine();
        switch (m_iCurMapObjectIdx)
        {
        case WRITE_NAME :
            Desc.GameObjectDesc.fSpeedPerSec = 30.f;
            Desc.GameObjectDesc.strName = strString.c_str();
            break;
        case PLAYER:
            Desc.GameObjectDesc.fSpeedPerSec = 90.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Player";

            break;
        case ALCHOHOLBUILDING:// 왕돌
            Desc.GameObjectDesc.fSpeedPerSec = 90.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_AlchoholBuilding";
            break;
        case LARGE_PLATEAU:// 중간돌
            Desc.GameObjectDesc.fSpeedPerSec = 50.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Large_Plateau";
            Desc.ePhysicsType = CMapObject::PHYSICS_STATE::TRIANGLE;
            break;
        //case MOONSHELL:
        //    Desc.GameObjectDesc.fSpeedPerSec = 50.f;
        //    Desc.GameObjectDesc.strName = L"Prototype_Component_Model_MoonShell";
        //    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapObject", m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Environment"), &Desc)))
        //        MSG_BOX("생성실패 >_<ㅋ");
        //    break;
        case CYLINDER00:
            Desc.GameObjectDesc.fSpeedPerSec = 20.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Cylinder00";
            break;
        case CYLINDER01:
            Desc.GameObjectDesc.fSpeedPerSec = 20.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Cylinder01";
            break;
        case CYLINDER02:
            Desc.GameObjectDesc.fSpeedPerSec = 20.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Cylinder02";
            break;
        case CYLINDER03:
            Desc.GameObjectDesc.fSpeedPerSec = 20.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Cylinder03";
            break;
        case NCCITY_SHELFCORAL:
            Desc.GameObjectDesc.fSpeedPerSec = 150.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_NCCITY_SHELFCORAL";
            break;
        case CORALTREE00:
            Desc.GameObjectDesc.fSpeedPerSec = 10.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_CoralTree_00";
            break;
        case CORALTREE01:
            Desc.GameObjectDesc.fSpeedPerSec = 10.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_CoralTree_01";
             break;
        case CORALTREE02:
            Desc.GameObjectDesc.fSpeedPerSec = 10.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_CoralTree_02";
            break;
        case CORALTREE03:
            Desc.GameObjectDesc.fSpeedPerSec = 10.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_CoralTree_03";          
            break;
        case CORALTREE04:
            Desc.GameObjectDesc.fSpeedPerSec = 10.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_CoralTree_04";
            break;
        case CORALTREE05:
            Desc.GameObjectDesc.fSpeedPerSec = 10.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_CoralTree_05";
            break;
        case CORAL_BRANCH:
            Desc.GameObjectDesc.fSpeedPerSec = 5.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_CoralBranch";
            break;
        case TUBECORAL00:
            Desc.GameObjectDesc.fSpeedPerSec = 5.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_TubeCoral00";
            break;
        case TUBECORAL01:
            Desc.GameObjectDesc.fSpeedPerSec = 5.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_TubeCoral01";
            break;
        case SHELFCORAL01:
            Desc.GameObjectDesc.fSpeedPerSec = 100.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_ShelfCoral_01";
            break;
        case SHELFCORAL02:
            Desc.GameObjectDesc.fSpeedPerSec = 100.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_ShelfCoral_02";
            break;
        case SHELFCORAL03:
            Desc.GameObjectDesc.fSpeedPerSec = 100.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_ShelfCoral_03";
            break;
        case SHELFCORAL04:
            Desc.GameObjectDesc.fSpeedPerSec = 100.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_ShelfCoral_04";
            break;
        case SHELFCORAL05:
            Desc.GameObjectDesc.fSpeedPerSec = 100.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_ShelfCoral_05";
            break;
        case BRIDGE:
            Desc.GameObjectDesc.fSpeedPerSec = 500.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Bridge";
            break;
        case GIN:
            Desc.GameObjectDesc.fSpeedPerSec = 100.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_NC_Building_Gin_01";
            break;
        case WHISKEY:
            Desc.GameObjectDesc.fSpeedPerSec = 100.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_NC_Building_Whiskey_01";
            break;
        case RECEIPTROAD:
            Desc.GameObjectDesc.fSpeedPerSec = 1000.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_ReceiptRoad";
            break;
        case TOPSIDE00:
            Desc.GameObjectDesc.fSpeedPerSec = 500.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_TopSide00";
            break;
        case TOPSIDE01:
            Desc.GameObjectDesc.fSpeedPerSec = 500.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_TopSide01";
            break;
        case TOPSIDE02:
            Desc.GameObjectDesc.fSpeedPerSec = 500.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_TopSide02";
            break;
        case TOPSIDE03:
            Desc.GameObjectDesc.fSpeedPerSec = 500.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_TopSide03";
            break;
        case TOPSIDE04:
            Desc.GameObjectDesc.fSpeedPerSec = 500.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_TopSide04";
            break;
        case PLANE004:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Plane004";
            break;
        case PLANE005:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Plane005";
            break;
        case PLANE006:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Plane006";
            break;
        case WOODPLATE:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_WoodPlate";
            break;
        case METALCHAIR:
            Desc.GameObjectDesc.fSpeedPerSec = 500.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_MetalChair";
            break;
        case BUTTERTUBTOP:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_ButterTubTop";
            break;
        case BUTTERTUB:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_ButterTub";
            break;
        case TRASHCANTOP:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_TrashCanTop";
            break;
        case TRASHCAN:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_TrashCan";
            break;
        case RUSTSEACHAIN:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_RustSeaChain";
            break;
        case SEACHAIN:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_SeaChain";
            break;
        case DUMPSTER:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Dumpster";
            break;
        case ANCHOR:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Anchor";
            break;
        case SPIRE:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Spire";
            break;
        case LARGEKELPTREE:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_LargeKelpTree";
            break;
        case CLUSTER_LARGE00:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Cluster_Large00";
            break;
        case CLUSTER_LARGE01:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Cluster_Large01";
            break;
            //CLUSTER_MEDIUM, CLUSTER_SMALL,
        case CLUSTER_MEDIUM:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_ClusterMedium";
            break;
        case CLUSTER_SMALL:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_ClusterSmall";
            break;
        case PLATFORMYROCK00:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_PlatformyRock00";
            break;
        case PLATFORMYROCK01:
            Desc.GameObjectDesc.fSpeedPerSec = 300.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_PlatformyRock01";
            break;
        case LARGE_SHARP_CLIFF:
            Desc.GameObjectDesc.fSpeedPerSec = 1000.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Large_Sharp_Cliff";
            break;
        case DUNEFENCE00:
            Desc.GameObjectDesc.fSpeedPerSec = 100.f;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_DuneFence00";
            break;
        case LIFERING:
            Desc.GameObjectDesc.fSpeedPerSec = 600;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_LifeRing";
            break;
        case TANK:
            Desc.GameObjectDesc.fSpeedPerSec = 200;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Tank";
            break;
        case HOLETANK:
            Desc.GameObjectDesc.fSpeedPerSec = 200;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_HoleTank";
            break;
        case TIRE:
            Desc.GameObjectDesc.fSpeedPerSec = 400;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Tire";
            break;
        case TRASHPILE00:
            Desc.GameObjectDesc.fSpeedPerSec = 500;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_TrashPile00";
            break;
        case TRASHPILE01:
            Desc.GameObjectDesc.fSpeedPerSec = 500;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_TrashPile01";
            break;
        case TRASHPILE02:
            Desc.GameObjectDesc.fSpeedPerSec = 500;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_TrashPile02";
            break;
        case COOLER:
            Desc.GameObjectDesc.fSpeedPerSec = 500;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_Cooler";
            break;
        case COOLERTOP:
            Desc.GameObjectDesc.fSpeedPerSec = 500;
            Desc.GameObjectDesc.strName = L"Prototype_Component_Model_CoolerTop";
            break;
        }
        if (SUCCEEDED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapObject", m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Environment"), &Desc)))
        {
            _uint iObjListSize = (_uint)(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Environment")).size();
            CGameObject* pNewGameObject = m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Environment", iObjListSize - 1);
            m_iPickedID = pNewGameObject->Get_ID(); // 꼭 바꿔주깅. 피킹 후에 바로 아이디 바꿔줘서 바로 상태 제어 할 수 있게끔요.
        }
    }


    //오브젝트 삭제
    {
        ImGui::SameLine();
        //if (ImGui::Button("Interaction_Del") || m_pGameInstance->Key_Down(KEY::BACKSPACE))
        //{
        //    _uint iListSize = (_uint)(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Insteraction_Environment")).size();
        //    if (iListSize != 0)
        //    {
        //        m_pGameInstance->Delete_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Insteraction_Environment", iListSize - 1);
        //        m_iPickedID = 0; // 지우고 꼭 초기화 해주어야 해여
        //    }

        //}
        //ImGui::SameLine();
        //if (ImGui::Button("MapObject_Del"))
        //{
        //    _uint iListSize = (_uint)(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Environment")).size();
        //    if (iListSize != 0)
        //    {
        //        m_pGameInstance->Delete_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Environment", iListSize - 1);
        //        m_iPickedID = 0; // 지우고 꼭 초기화 해주어야 해여
        //    }
        //}
        ImGui::Spacing();
        ImGui::Separator();
    }
    //오브젝트 저장 & 불러오기
    {
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("Interaction_Save"))
        {
            Save_Interaction_Object();
        }
        ImGui::SameLine();
        if (ImGui::Button("Interaction_Load"))
        {
            Load_Interaction_Object();
        }
        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Spacing();
        if (ImGui::Button("Environment_Save"))
        {
            Save_MapObject();
        }
        ImGui::SameLine();
        if (ImGui::Button("Environment_Load"))
        {
            Load_MapObject();
        }

        if (ImGui::Button("Sheleport_Save"))
        {
            Sheleport_Save();
        }
        ImGui::SameLine();
        if (ImGui::Button("Sheleport_Load"))
        {
            Sheleport_Load();
        }
        ImGui::Spacing();
    }
#pragma endregion

#pragma region Navigation
    //포인트 피킹 체크박스
    {
        ImGui::Spacing();
        ImGui::SeparatorText("NAVIGATION");
        ImGui::Spacing();
        ImGui::Checkbox("Check_PointPicking", &m_bPointPicking);
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Pick_Cnt: %d", m_iPickingCnt);
        ImGui::Spacing();
    }
    // 네비게이션 저장 & 불러오기
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("Navigation_Save"))
        {
            if (FAILED(m_pTerrain->Save_Navigation(_T("../Bin/DataFiles/Navigation_LevelStartMap.dat"))))
                MSG_BOX("Navigation_Save_Failed!!!!!!!");
            else
                MSG_BOX("Navigation_Save_Success!");
        }
        ImGui::SameLine();
        if (ImGui::Button("Navigation_Load"))
        {
            if (FAILED(m_pTerrain->Load_Navigation(_T("../Bin/DataFiles/Navigation_LevelStartMap.dat"))))
                MSG_BOX("Navigation_Load_Failed!!!!!!!");
            else
                MSG_BOX("Navigation_Load_Success!");
        }
        ImGui::Spacing();
    }
    // 이전 포인트, 셀 삭제
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("PrevPoint_Delete"))
        {
            if (m_iPickingCnt != 0)
            {
                _int iIndex = m_PointIndices[m_iPickingCnt - 1];
                HRESULT hr = m_pTerrain->Delete_Point(iIndex);
                if (hr == E_FAIL)
                    MSG_BOX("삭제 실패");
            }
            else if (m_iPickingCnt == 0)
                MSG_BOX("삭제할 포인트가 없어여. 셀 삭제 하세여!");
            else
            {
                m_PointIndices[m_iPickingCnt - 1] = -1;
                m_iPickingCnt--;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("PrevCell_Delete"))
        {
            m_pTerrain->Delete_PrevCell();
        }
        ImGui::Spacing();
    }
#pragma endregion

    //하이트맵 생성
    {
        ImGui::SeparatorText("HEIGHTMAP");

        if (ImGui::RadioButton("None", &m_iSelected, 0)) {}
        ImGui::SameLine();
        if (ImGui::RadioButton("Up", &m_iSelected, 1)) {}
        ImGui::SameLine();
        if (ImGui::RadioButton("Down", &m_iSelected, 2)) {}
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Radius");
        ImGui::SameLine();
        ImGui::DragFloat("##Radius", &m_fRadius, 0.1f, 0.1f, 100.f, "%.1f");
        ImGui::Text("Height");
        ImGui::SameLine();
        ImGui::DragFloat("##HeightSize", &m_fHeightSize, 0.1f, 0.1f, 100.f, "%.1f");
        ImGui::Text("Sigma");
        ImGui::SameLine();
        ImGui::DragFloat("##Sigma", &m_fSigma, 0.1f, 0.1f, 100.f, "%.1f");
        if (m_pGameInstance->Mouse_Down(DIM_RB) && m_iSelected != 0)
        {
            CGameObject* pPickedGameObject = m_pGameInstance->Find_GameObject_With_ID(m_pGameInstance->Get_CurrentLevelIndex(), m_iPickedID);
            if (pPickedGameObject != nullptr && _wstring(L"Terrain") == pPickedGameObject->Get_Name())
            {
                _float4 vPickWorldPos = {};
                _float3		vMousePos = {};
                if (true == m_pGameInstance->Compute_PixelPickPos(vMousePos))
                {
                    _vector vWorldMousePos = {};
                    vWorldMousePos = XMLoadFloat3(&vMousePos);
                    XMVectorSetW(vWorldMousePos, 1.f);
                    XMStoreFloat4(&vPickWorldPos, vWorldMousePos);

                    if (m_iSelected == 1)
                        m_pTerrain->Generate_HeightMap(vPickWorldPos, m_fRadius, m_fHeightSize, m_fSigma, true);

                    if (m_iSelected == 2)
                        m_pTerrain->Generate_HeightMap(vPickWorldPos, m_fRadius, m_fHeightSize, m_fSigma, false);
                }
            }
        }
    }
    //하이트맵 저장 & 불러오기
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("HeightMap_Load"))
        {
            _wstring strFile = {}; //다이얼로그로 가져오는 파일패스 저장할 곳
            //_wstring strBasePath = L"../Bin/Resources/Textures/Terrain/";

            ///*다이얼로그 띄우기*/
            //CDialog* pDialog = CDialog::Create(CDialog::TYPE::LOAD);
            //pDialog->Set_FileType(CDialog::FILE_TYPE::BMP);
            //pDialog->Set_DefaultExt(L"bmp");

            //if (SUCCEEDED(pDialog->Show_Dialog()))
            //{
            //    strFile = (pDialog->Get_FilePath());
            //}
            //Safe_Release(pDialog);
            ///*다이얼로그 띄우기(끝)*/

            //로드하기
            //HRESULT hr{};
            //DWORD dwByte{};
            //HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

            //if (INVALID_HANDLE_VALUE == hFile)
            //{
            //    MSG_BOX("Can Not Open File");
            //    return E_FAIL;
            //}

            //size_t lastSlash = strFile.find_last_of(L"\\/");  // 윈도우 경로 구분자 처리
            //strFile = strFile.substr(lastSlash + 1);

            //strFile = strBasePath + strFile;
            //const _tchar* szFilePath = strFile.c_str();

            if (FAILED(m_pTerrain->Load_HeightMap(L"../Bin/Resources/Textures/Terrain/MakeHeightMap.bmp"))) // Field
            //if (FAILED(m_pTerrain->Load_HeightMap(L"../Bin/Resources/Textures/Terrain/MakeInkertonMap.bmp"))) // CrabTomb
            //if (FAILED(m_pTerrain->Load_HeightMap(L"../Bin/Resources/Textures/Terrain/MakeTutorialMap.bmp"))) // Beach
                MSG_BOX("Height_Load_Failed!!!!!!!");
            else
                MSG_BOX("Height_Load_Success!");

            //CloseHandle(hFile);
        }
        ImGui::SameLine();
        //if (ImGui::Button("HeightMap_Save"))
        //{
        //    if (FAILED(m_pTerrain->Save_HeightMap(_T("../Bin/Resources/Textures/Terrain/MakeHeightMap.bmp"))))
        //    //if (FAILED(m_pTerrain->Save_HeightMap(_T("../Bin/Resources/Textures/Terrain/MakeInkertonMap.bmp"))))
        //    //if (FAILED(m_pTerrain->Save_HeightMap(_T("../Bin/Resources/Textures/Terrain/MakeTutorialMap.bmp"))))
        //        MSG_BOX("Height_Save_Failed!!!!!!!");
        //    else
        //        MSG_BOX("Height_Save_Success!");
        //}
        ImGui::Spacing();
    }
    ImGui::End();

    return S_OK;
}

void CMapLevel_Tool::Handle_Manipulate(_float4x4& WorldMat)
{
    _float4x4 ViewMat = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW);
    _float4x4 ProjMat = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ);

    switch (m_iGizmoSRTType)
    {
    case 0: // Scale
        ImGuizmo::Manipulate(*ViewMat.m, *ProjMat.m, ImGuizmo::OPERATION::SCALE, ImGuizmo::LOCAL, *WorldMat.m);
        break;
    case 1: // Rotate
        ImGuizmo::Manipulate(*ViewMat.m, *ProjMat.m, ImGuizmo::OPERATION::ROTATE, ImGuizmo::WORLD, *WorldMat.m);
        break;
    case 2: // Translate
        ImGuizmo::Manipulate(*ViewMat.m, *ProjMat.m, ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::WORLD, *WorldMat.m);
        break;
    }
}


string CMapLevel_Tool::WStringToString(const _wstring& szName)
{
    if (szName.empty()) return "";

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, szName.c_str(), -1, NULL, 0, NULL, NULL);
    _string strName(size_needed - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, szName.c_str(), -1, &strName[0], size_needed, NULL, NULL);
    return strName;
}

HRESULT CMapLevel_Tool::Picking_NavigationPoint()
{
    if (m_pGameInstance->Mouse_Down(DIM_RB))
    {
        _float3 vMousePos = {};
        _int iResult = m_pTerrain->Add_Point(vMousePos);

        if (iResult == -1) // 범위 밖
            return S_OK;
        else
        {
            if (m_PointIndices.size() < 3) // 3개 미만일 때만 실행
            {
                _bool bAlreadyExists = false;

                // 기존 값이 있?
                for (_uint i = 0; i < m_PointIndices.size(); i++)
                {
                    if (m_PointIndices[i] == iResult) // 이미 존재하는 값이면
                    {
                        bAlreadyExists = true;
                        break;
                    }
                }
                // 중복이 없으면 푸시
                if (!bAlreadyExists)
                {
                    m_PointIndices.push_back(iResult);
                    m_iPickingCnt++;
                }
            }
            else
            {
                _bool bAlreadyExists = false;

                for (_uint i = 0; i < 3; i++)
                {
                    if (m_PointIndices[i] == iResult) // 기존 새끼랑 인덱스가 똑같음
                    {
                        bAlreadyExists = true;
                        break;
                    }
                }
                if (!bAlreadyExists)
                {
                    //m_PointIndices.push_back(iResult);
                    m_PointIndices[m_iPickingCnt] = iResult;
                    m_iPickingCnt++;
                }
            }
        }

        if (m_iPickingCnt == 3)
        {
            for (size_t i = 0; i < 3; i++)
            {
                if (m_PointIndices[i] == -1)
                {
                    m_iPickingCnt = 0;
                    return S_OK;

                }
            }
            m_pTerrain->Add_Cell(m_PointIndices);
            lstrcpy(m_szToolText, TEXT("Add Cell Success!!"));

            m_iPickingCnt = 0;
            for (size_t i = 0; i < 3; i++)
            {
                m_PointIndices[i] = -1; // 초기화
            }
        }
    }
    return S_OK;
}


HRESULT CMapLevel_Tool::Save_Interaction_Object()
{
    _wstring szFile = {}; //다이얼로그로 가져오는 파일패스 저장할 곳
    /*다이얼로그 띄우기*/
    CDialog* pDialog = CDialog::Create(CDialog::TYPE::SAVE);
    pDialog->Set_FileType(CDialog::FILE_TYPE::INTERACTION_OBJ);
    pDialog->Set_DefaultExt(L"interaction");

    if (SUCCEEDED(pDialog->Show_Dialog())) 
    {
        szFile = (pDialog->Get_FilePath());
    }
    Safe_Release(pDialog);
    /*다이얼로그 띄우기(끝)*/

    //로드하기
    HRESULT hr{};
    DWORD dwByte{};
    HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File");
    }

    // 레이어가 가지고 있는 오브젝트 리스트가져와서 사이즈 알아낸 다음에ㅋㅋㅋㅋDesc(상태제어값) 안에 내용물 desc.이름 저장 
    list<CGameObject*> Envi_ObjList = m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Insteraction_Environment");
    //_uint iListSize = Envi_ObjList.size();
    //WriteFile(hFile, &iListSize, sizeof(_uint), &dwByte, nullptr);

    for (auto& pSaveObject : Envi_ObjList)
    {
        //_uint strlength = (_uint)pSaveObject->Get_Name().size();// 이름 길이 
        //WriteFile(hFile, &strlength, sizeof(_uint), &dwByte, nullptr); 

        //_wstring strName = pSaveObject->Get_Name();// 이름 문자열
        _tchar szName[MAX_PATH]{};
        lstrcpy(szName, pSaveObject->Get_Name().c_str());

        WriteFile(hFile, szName, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);

        _float4x4 WorldMat = {};// 월드 행렬
        XMStoreFloat4x4(&WorldMat, static_cast<CTransform*>(pSaveObject->Find_Component(g_strTransformTag))->Get_WorldMatrix());
        WriteFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);
    }

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CMapLevel_Tool::Load_Interaction_Object()
{
    _wstring szFile = {}; //다이얼로그로 가져오는 파일패스 저장할 곳
    /*다이얼로그 띄우기*/
    CDialog* pDialog = CDialog::Create(CDialog::TYPE::LOAD);
    pDialog->Set_FileType(CDialog::FILE_TYPE::INTERACTION_OBJ);
    pDialog->Set_DefaultExt(L"interaction");

    if (SUCCEEDED(pDialog->Show_Dialog()))
    {
        szFile = (pDialog->Get_FilePath());
    }
    Safe_Release(pDialog);
    /*다이얼로그 띄우기(끝)*/

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
        _bool bRead = false;

        bRead = ReadFile(hFile, &szName, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);
        strName = szName;
        bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);

        if (!bRead || 0 == dwByte)
            break;

        CGameObject::DESC Desc = {};
        Desc.strName = strName;
        Desc.TransformDesc.WorldMatrix = WorldMat;

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, strName, LEVELID::LEVEL_TOOL, L"Layer_Insteraction_Environment", &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
            return E_FAIL;
    }

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CMapLevel_Tool::Save_MapObject()
{
    _wstring szFile = {}; //다이얼로그로 가져오는 파일패스 저장할 곳
    /*다이얼로그 띄우기*/
    CDialog* pDialog = CDialog::Create(CDialog::TYPE::SAVE);
    pDialog->Set_FileType(CDialog::FILE_TYPE::NON_INTERACTION_OBJ);
    pDialog->Set_DefaultExt(L"environment");

    if (SUCCEEDED(pDialog->Show_Dialog()))
    {
        szFile = (pDialog->Get_FilePath());
    }
    Safe_Release(pDialog);
    /*다이얼로그 띄우기(끝)*/

    //로드하기
    HRESULT hr{};
    DWORD dwByte{};
    HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File");
        return E_FAIL;
    }

    // 레이어가 가지고 있는 오브젝트 리스트가져와서 사이즈 알아낸 다음에ㅋㅋㅋㅋDesc(상태제어값) 안에 내용물 desc.이름 저장 
    list<CGameObject*> Envi_ObjList = m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Environment");
    //_uint iListSize = Envi_ObjList.size();
    //WriteFile(hFile, &iListSize, sizeof(_uint), &dwByte, nullptr);

    for (auto& pSaveObject : Envi_ObjList)
    {
        //_uint strlength = (_uint)pSaveObject->Get_Name().size();// 이름 길이 
        //WriteFile(hFile, &strlength, sizeof(_uint), &dwByte, nullptr); 

        //_wstring strName = pSaveObject->Get_Name();// 이름 문자열
        _tchar szName[MAX_PATH]{};
        lstrcpy(szName, pSaveObject->Get_Name().c_str());

        WriteFile(hFile, szName, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);

        _float4x4 WorldMat = {};// 월드 행렬
        XMStoreFloat4x4(&WorldMat, static_cast<CTransform*>(pSaveObject->Find_Component(g_strTransformTag))->Get_WorldMatrix());
        WriteFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);

        _float fFrustumRange = static_cast<CMapObject*>(pSaveObject)->Get_FrustumRange();
        WriteFile(hFile, &fFrustumRange, sizeof(_float), &dwByte, nullptr);

        _float4 vBaseColor = static_cast<CMapObject*>(pSaveObject)->Get_BaseColor();
        WriteFile(hFile, &vBaseColor, sizeof(_float4), &dwByte, nullptr);

        _uint iPhysicState = static_cast<CMapObject*>(pSaveObject)->Get_PhysicState();
        WriteFile(hFile, &iPhysicState, sizeof(_uint), &dwByte, nullptr);


    }

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CMapLevel_Tool::Load_MapObject()
{
    _wstring szFile = {}; //다이얼로그로 가져오는 파일패스 저장할 곳
    /*다이얼로그 띄우기*/
    CDialog* pDialog = CDialog::Create(CDialog::TYPE::LOAD);
    pDialog->Set_FileType(CDialog::FILE_TYPE::NON_INTERACTION_OBJ);
    pDialog->Set_DefaultExt(L"environment");

    if (SUCCEEDED(pDialog->Show_Dialog()))
    {
        szFile = (pDialog->Get_FilePath());
    }
    Safe_Release(pDialog);
    /*다이얼로그 띄우기(끝)*/

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

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_MapObject", LEVELID::LEVEL_TOOL, L"Layer_Environment", &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
            return E_FAIL;

        _uint iObjListSize = (_uint)(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Environment")).size();
        CGameObject* pNewGameObject = m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Environment", iObjListSize - 1);
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
                    || strName == "WormCoral_LOD0" || strName == "TubeCoral_01_LOD0_A.002" || strName == "TubeCoral_01_LOD0_A.001"
                    )
                {
                    m_iCurMeshIdx = i; // 동일하면 인덱스 넣어줘여
                    m_iCurMaterialIdx = pModel->Get_MaterialIndex_In_Mesh(m_iCurMeshIdx); //메쉬안에 메테리얼 인덱스찾어
                    pModel->Set_BaseColor(m_iCurMaterialIdx, vBaseColor);
                    break;
                }
            }
        }
    }

    CloseHandle(hFile);

    return S_OK;
}

void CMapLevel_Tool::Sheleport_Save()
{
    _wstring szFile = {}; //다이얼로그로 가져오는 파일패스 저장할 곳
    /*다이얼로그 띄우기*/
    CDialog* pDialog = CDialog::Create(CDialog::TYPE::SAVE);
    pDialog->Set_FileType(CDialog::FILE_TYPE::INTERACTION_OBJ);
    pDialog->Set_DefaultExt(L"interaction");

    if (SUCCEEDED(pDialog->Show_Dialog()))
    {
        szFile = (pDialog->Get_FilePath());
    }
    Safe_Release(pDialog);
    /*다이얼로그 띄우기(끝)*/

    //로드하기
    HRESULT hr{};
    DWORD dwByte{};
    HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File");
    }

    list<CGameObject*> Envi_ObjList = m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Insteraction_Environment");


    for (auto& pSaveObject : Envi_ObjList)
    {
        _int iSheleportID = static_cast<CShellHouse*>(pSaveObject)->Get_SheleportID();

        WriteFile(hFile, &iSheleportID, sizeof(_int), &dwByte, nullptr);

        _float4x4 WorldMat = {};// 월드 행렬
        XMStoreFloat4x4(&WorldMat, static_cast<CTransform*>(pSaveObject->Find_Component(g_strTransformTag))->Get_WorldMatrix());
        WriteFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);
    }

    CloseHandle(hFile);
}

void CMapLevel_Tool::Sheleport_Load()
{
    _wstring szFile = {}; //다이얼로그로 가져오는 파일패스 저장할 곳
    /*다이얼로그 띄우기*/
    CDialog* pDialog = CDialog::Create(CDialog::TYPE::LOAD);
    pDialog->Set_FileType(CDialog::FILE_TYPE::INTERACTION_OBJ);
    pDialog->Set_DefaultExt(L"interaction");

    if (SUCCEEDED(pDialog->Show_Dialog()))
    {
        szFile = (pDialog->Get_FilePath());
    }
    Safe_Release(pDialog);
    /*다이얼로그 띄우기(끝)*/

    //로드하기
    HRESULT hr{};
    DWORD dwByte{};
    HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File");
        return;
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

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_ShellHouse", LEVELID::LEVEL_TOOL, L"Layer_Insteraction_Environment", &Desc))) // 얘도 사실 Static 에 있어도 되는거 아니야?
            return;
    }

    CloseHandle(hFile);

}

void CMapLevel_Tool::Save_MapItem()
{
    _wstring szFile = {};
    CDialog* pDialog = CDialog::Create(CDialog::TYPE::SAVE);

    pDialog->Set_FileType(CDialog::FILE_TYPE::ITEM);
    pDialog->Set_DefaultExt(L"item");

    if (SUCCEEDED(pDialog->Show_Dialog()))
    {
        szFile = (pDialog->Get_FilePath());
    }
    Safe_Release(pDialog);

    HRESULT hr{};
    DWORD dwByte{};
    HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File");
        CloseHandle(hFile);
        return;
    }

    list<CGameObject*> MapTriggerList = m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_MapItem");

    for (auto pSaveObject : MapTriggerList)
    {
        //_wstring strName = pSaveObject->Get_Name();
        //_tchar szName[MAX_PATH] = {};
        //lstrcpy(szName, strName.c_str());
        //WriteFile(hFile, szName, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);

        _float4x4 WorldMat = {};
        XMStoreFloat4x4(&WorldMat, static_cast<CTransform*>(pSaveObject->Find_Component(g_strTransformTag))->Get_WorldMatrix());
        WriteFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);
    }

    CloseHandle(hFile);
}

void CMapLevel_Tool::Load_MapItem()
{
    _bool bRead = false;
    _wstring szFile = {};
    CDialog* pDialog = CDialog::Create(CDialog::TYPE::LOAD);
    pDialog->Set_FileType(CDialog::FILE_TYPE::ITEM);
    pDialog->Set_DefaultExt(L"item");

    if (SUCCEEDED(pDialog->Show_Dialog()))
    {
        szFile = (pDialog->Get_FilePath());
    }
    Safe_Release(pDialog);

    HRESULT hr{};
    DWORD dwByte{};
    HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File");
        CloseHandle(hFile);
        return;
    }

    while (true)
    {
        //_wstring strName = {};
        //_tchar szName[MAX_PATH] = {};
        //bRead = ReadFile(hFile, szName, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);
        //strName = szName;

        _float4x4 WorldMat = {};
        bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);

        if (!bRead || dwByte == 0)
            break;
        CMapTrigger::DESC Desc = {};
        //Desc.GameObjectDesc.strName = strName.c_str();
        Desc.GameObjectDesc.WorldMatrix = WorldMat;
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapTool_Item", LEVEL_TOOL, L"Layer_MapItem", &Desc)))
        {
            MSG_BOX("트리거 추가 안댐.");
            break;
        }
    }

    CloseHandle(hFile);
}

void CMapLevel_Tool::Save_MapShell()
{
    _wstring szFile = {};
    CDialog* pDialog = CDialog::Create(CDialog::TYPE::SAVE);

    pDialog->Set_FileType(CDialog::FILE_TYPE::ITEM);
    pDialog->Set_DefaultExt(L"item");

    if (SUCCEEDED(pDialog->Show_Dialog()))
    {
        szFile = (pDialog->Get_FilePath());
    }
    Safe_Release(pDialog);

    HRESULT hr{};
    DWORD dwByte{};
    HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File");
        CloseHandle(hFile);
        return;
    }

    list<CGameObject*> MapShellList = m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_Shell");

    for (auto pSaveObject : MapShellList)
    {   
        _int iType = static_cast<CMapTool_Item*>(pSaveObject)->Get_Type();
        WriteFile(hFile, &iType, sizeof(_int), &dwByte, nullptr);

        _float4x4 WorldMat = {};
        XMStoreFloat4x4(&WorldMat, static_cast<CTransform*>(pSaveObject->Find_Component(g_strTransformTag))->Get_WorldMatrix());
        WriteFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);
    }

    CloseHandle(hFile);
}

void CMapLevel_Tool::Load_MapShell()
{
    _bool bRead = false;
    _wstring szFile = {};
    CDialog* pDialog = CDialog::Create(CDialog::TYPE::LOAD);
    pDialog->Set_FileType(CDialog::FILE_TYPE::ITEM);
    pDialog->Set_DefaultExt(L"item");

    if (SUCCEEDED(pDialog->Show_Dialog()))
    {
        szFile = (pDialog->Get_FilePath());
    }
    Safe_Release(pDialog);

    HRESULT hr{};
    DWORD dwByte{};
    HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File");
        CloseHandle(hFile);
        return;
    }

    while (true)
    {
        _int   iType = {};
        bRead = ReadFile(hFile, &iType, sizeof(_int), &dwByte, nullptr);

        _float4x4 WorldMat = {};
        bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);

        if (!bRead || dwByte == 0)
            break;
        CMapTool_Item::DESC Desc = {};
        Desc.eType = (CMapTool_Item::SHELL)iType;
        if (iType == 0)
            Desc.GameObjectDesc.strName = L"House" + to_wstring(m_iCountShell++);
        else if (iType == 1)
            Desc.GameObjectDesc.strName = L"Soda" + to_wstring(m_iCountShell++);
        else if (iType == 2)
            Desc.GameObjectDesc.strName = L"MirrorBall" + to_wstring(m_iCountShell++);
        else if (iType == 3)
            Desc.GameObjectDesc.strName = L"Duck" + to_wstring(m_iCountShell++);
        else if (iType == 4)
            Desc.GameObjectDesc.strName = L"Tennis" + to_wstring(m_iCountShell++);

        Desc.GameObjectDesc.WorldMatrix = WorldMat;
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapTool_Item", LEVEL_TOOL, L"Layer_Shell", &Desc)))
        {
            MSG_BOX("쉘 추가 안댐.");
            break;
        }
    }

    CloseHandle(hFile);
}

HRESULT CMapLevel_Tool::Save_NPC(const _wstring& strPrototypeTag)
{
    _wstring szFile = {};
    CDialog* pDialog = CDialog::Create(CDialog::TYPE::SAVE);
    pDialog->Set_FileType(CDialog::FILE_TYPE::NPC);
    pDialog->Set_DefaultExt(L"npc");

    if (SUCCEEDED(pDialog->Show_Dialog()))
    {
        szFile = (pDialog->Get_FilePath());
    }
    Safe_Release(pDialog);

    HRESULT hr{};
    DWORD dwByte{};
    HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File");
        CloseHandle(hFile);
        return E_FAIL;
    }

    //CGameObject* pSaveObject = nullptr;
    //pSaveObject = m_pGameInstance->Find_GameObject_With_Name(LEVEL_TOOL, L"Layer_NPC", strPrototypeTag);

    list<CGameObject*> NPCList = m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_NPC");

    for (auto& NPC : NPCList)
    {
        _float4x4 WorldMat = {};// 월드 행렬
        XMStoreFloat4x4(&WorldMat, static_cast<CTransform*>(NPC->Find_Component(g_strTransformTag))->Get_WorldMatrix());
        WriteFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);
    }
    CloseHandle(hFile);

    return S_OK;
}

HRESULT CMapLevel_Tool::Load_NPC(const _wstring& strPrototypeTag)
{
    _bool bRead = false;
    _wstring szFile = {};
    CDialog* pDialog = CDialog::Create(CDialog::TYPE::LOAD);
    pDialog->Set_FileType(CDialog::FILE_TYPE::NPC);
    pDialog->Set_DefaultExt(L"npc");

    if (SUCCEEDED(pDialog->Show_Dialog()))
    {
        szFile = (pDialog->Get_FilePath());    
    }
    Safe_Release(pDialog);

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
        _float4x4 WorldMat = {};// 월드 행렬
        bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);
    
        if (!bRead || 0 == dwByte)
            break;

        CGameObject::DESC Desc = {};
        Desc.strName = strPrototypeTag.c_str();
        Desc.WorldMatrix = WorldMat; 

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapTool_NPC", LEVEL_TOOL, TEXT("Layer_NPC"), &Desc)))
            MSG_BOX("NPC로드 실패!");
    }

    CloseHandle(hFile);

    return S_OK;
}

void CMapLevel_Tool::Save_MapTrigger()
{
    _wstring szFile = {};
    CDialog* pDialog = CDialog::Create(CDialog::TYPE::SAVE);
    pDialog->Set_FileType(CDialog::FILE_TYPE::MAP_TRIGGER);
    pDialog->Set_DefaultExt(L"mapTrigger");

    if (SUCCEEDED(pDialog->Show_Dialog()))
    {
        szFile = (pDialog->Get_FilePath());
    }
    Safe_Release(pDialog);

    HRESULT hr{};
    DWORD dwByte{};
    HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File");
        CloseHandle(hFile);
        return;
    }

    list<CGameObject*> MapTriggerList = m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_MapTrigger");

    for (auto pSaveObject : MapTriggerList)
    {
        _wstring strName = pSaveObject->Get_Name();
        _tchar szName[MAX_PATH] = {};
        lstrcpy(szName, strName.c_str());
        WriteFile(hFile, szName, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);

        _float4x4 WorldMat = {};
        XMStoreFloat4x4(&WorldMat, static_cast<CTransform*>(pSaveObject->Find_Component(g_strTransformTag))->Get_WorldMatrix());
        WriteFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);
    }

    CloseHandle(hFile);
}

void CMapLevel_Tool::Load_MapTrigger()
{
    _bool bRead = false;
    _wstring szFile = {};
    CDialog* pDialog = CDialog::Create(CDialog::TYPE::LOAD);
    pDialog->Set_FileType(CDialog::FILE_TYPE::MAP_TRIGGER);
    pDialog->Set_DefaultExt(L"mapTrigger");

    if (SUCCEEDED(pDialog->Show_Dialog()))
    {
        szFile = (pDialog->Get_FilePath());
    }
    Safe_Release(pDialog);

    HRESULT hr{};
    DWORD dwByte{};
    HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File");
        CloseHandle(hFile);
        return;
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
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapTrigger", LEVEL_TOOL, L"Layer_MapTrigger", &Desc)))
        {
            MSG_BOX("트리거 추가 안댐.");
            break;
        }
    }

    CloseHandle(hFile);

}

void CMapLevel_Tool::Save_Monster()
{
    _wstring szFile = {};
    CDialog* pDialog = CDialog::Create(CDialog::TYPE::SAVE);
    pDialog->Set_FileType(CDialog::FILE_TYPE::MONSTER);
    pDialog->Set_DefaultExt(L"monster");

    if (SUCCEEDED(pDialog->Show_Dialog()))
    {
        szFile = (pDialog->Get_FilePath());
    }
    Safe_Release(pDialog);

    HRESULT hr{};
    DWORD dwByte{};
    HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File");
        CloseHandle(hFile);
        return;
    }

    list<CGameObject*> MapTriggerList = m_pGameInstance->Get_GameObjects_In_Layer(LEVEL_TOOL, L"Layer_Monster");

    for (auto pSaveObject : MapTriggerList)
    {
        _wstring strName = pSaveObject->Get_Name();
        _tchar szName[MAX_PATH] = {};
        lstrcpy(szName, strName.c_str());
        WriteFile(hFile, szName, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);

        vector<_float4x4> vPositions = static_cast<CMapTool_Monster*>(pSaveObject)->Get_Postions();

        _uint iVectorSize = (_uint)vPositions.size();
        WriteFile(hFile,  &iVectorSize, sizeof(_uint), &dwByte, nullptr);

        for (auto& vPos : vPositions)
        {
            WriteFile(hFile, &vPos, sizeof(_float4x4), &dwByte, nullptr);
        }
    }

    CloseHandle(hFile);
}

void CMapLevel_Tool::Load_Monster()
{
    _bool bRead = false;
    _wstring szFile = {};
    CDialog* pDialog = CDialog::Create(CDialog::TYPE::LOAD);
    pDialog->Set_FileType(CDialog::FILE_TYPE::MONSTER);
    pDialog->Set_DefaultExt(L"monster");

    if (SUCCEEDED(pDialog->Show_Dialog()))
    {
        szFile = (pDialog->Get_FilePath());
    }
    Safe_Release(pDialog);

    HRESULT hr{};
    DWORD dwByte{};
    HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File");
        CloseHandle(hFile);
        return;
    }
    _uint iIndex = {};
    while (true)
    {
        _wstring strName = {};
        _tchar szName[MAX_PATH] = {};
        bRead = ReadFile(hFile, szName, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);
        strName = szName;

        if (!bRead || dwByte == 0)
            break;

        CGameObject::DESC Desc{};
        Desc.strName = strName.c_str();
        if (FAILED(m_pGameInstance->
            Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapTool_Monster", LEVEL_TOOL, L"Layer_Monster", &Desc)))
            MSG_BOX("추가 안됨.");

        CMapTool_Monster* pMonster = static_cast<CMapTool_Monster*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, L"Layer_Monster", iIndex));

        _uint iSize = {};
        bRead = ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

        for (_uint i = 0; i < iSize; i++)
        {
            _float4x4 WorldMat = {};
            bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);
            pMonster->Add_Matrix(WorldMat);
        }
    }

    CloseHandle(hFile);
}

CMapLevel_Tool* CMapLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMapLevel_Tool* pInstance = new CMapLevel_Tool{ pDevice, pContext };

    //if (FAILED(pInstance->Initialize()))
    //{
    //    MSG_BOX("Failed to Create : CMapLevel_Tool");
    //    Safe_Release(pInstance);
    //}

    return pInstance;
}

void CMapLevel_Tool::Free()
{
    CLevel::Free();
}
