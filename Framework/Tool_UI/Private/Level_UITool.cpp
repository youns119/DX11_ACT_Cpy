#include "pch.h"
#include "Level_UITool.h"

#include "GameInstance.h"

#include "Camera_Free.h"
#include "UI_Dummy.h"

CLevel_UITool::CLevel_UITool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_UITool::Initialize()
{
    CUIObject::Resize(m_pContext);

    if (FAILED(Ready_Lights()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(Ready_Tags()))
        return E_FAIL;

    return S_OK;
}

void CLevel_UITool::Update(_float fTimeDelta)
{
    if (ImGui::Begin("UI Editor", NULL, ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::Button("Create"))
        {
            Create_UI();
        }

        ImGui::SameLine();

        if (ImGui::Button("Delete"))
        {
            if(m_iCount > 0)
                Delete_UI();
        }

        if (ImGui::Button("Save"))
        {
            Save_UI();
        }

        ImGui::SameLine();

        if (ImGui::Button("Load"))
        {
            Load_UI();
        }

        ImGui::Spacing();

        if (m_iCount > 0)
            UI_Menu();
    }
    ImGui::End();
}

HRESULT CLevel_UITool::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, L"UI TOOL");
#endif

    return S_OK;
}

HRESULT CLevel_UITool::Ready_Layer_Camera(const _wstring& strLayerTag)
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
    }

    if (FAILED(m_pGameInstance->
        Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"), LEVEL_UITOOL, strLayerTag, &CameraFreeDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_UITool::Ready_Tags()
{
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Background"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Loading"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Locked"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Mask"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Round"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Triangle"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Shell_Logo"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Square"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Clock"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_X"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Compass"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_CompassArrow"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_CompassArrow2"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Cursor"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Fear"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Dialogue_Box"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Kril_dialogue"));
    /*m_TextureTag.push_back(TEXT("Prototype_Component_Texture_NPC_AppleKing_dialogue"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_NPC_Chitan_dialogue"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_NPC_Konche_dialogue"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_NPC_Tortellini_dialogue"));*/

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Grapple"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Icon"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Logo"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_MenuDot"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_MenuGrass"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_MenuHeader"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_MenuStar"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Arrow"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Decoration"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_MoonsnailFilled"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_MoonsnailOutline"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_PrevBorder"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_PrevFilled"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_RectBorder"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Star"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Star_8Point"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_WindowBorder"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_WindowFilled"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Notice"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_ChatBubble"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_LocationIcon"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_StowawayStar"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Peril"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Crystal"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_CurrencyBorder"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Ghost"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_HeartKelpCount"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_HookCount"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_HurtBorder"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_IconBorder"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_IconBorder2"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Microplastic"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Ring"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_TechniqueBorder"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Umami"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_UmamiWhite"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_RemainLocationIcon"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Recycle"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Shell_Def"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Shell_Weight"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Shell"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Skill"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_SkillChoice"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_StatIcon"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_ExtraIcons"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Outline_Def"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Button_Arrow"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_BlurryBox"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_BlurryRect"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Button"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Rect"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_MenuButton"));

    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_BarBorder_Armor"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_BarBorder_Health"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_BarBoss"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_RampHp"));
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_RampShell"));
    
    m_TextureTag.push_back(TEXT("Prototype_Component_Texture_Technique"));

    /* ================================================================================== */

    m_SpriteTextureTag.push_back(TEXT("Prototype_Component_Texture_Keyboard"));
    m_SpriteTextureTag.push_back(TEXT("Prototype_Component_Texture_CuteKeyboard"));

    /* ================================================================================== */

    m_FontTag.push_back(TEXT("Font_Cafe_10"));
    m_FontTag.push_back(TEXT("Font_Cafe_15"));
    m_FontTag.push_back(TEXT("Font_Cafe_20"));

    m_FontTag.push_back(TEXT("Font_Main_40"));

    /* ================================================================================== */

    m_MaskTextureTag.push_back(TEXT("Prototype_Component_Texture_Dissolve"));
    m_MaskTextureTag.push_back(TEXT("Prototype_Component_Texture_DitherBlueNoise"));
    m_MaskTextureTag.push_back(TEXT("Prototype_Component_Texture_Noise"));
    m_MaskTextureTag.push_back(TEXT("Prototype_Component_Texture_SandNoise"));

    return S_OK;
}

void CLevel_UITool::UI_Menu()
{
    _bool bGuiHover = { false };

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered())
        bGuiHover = true;

    Initialize_UI();

    vector<string> labels;

    _char label[32];

    for (_int i = 0; i < m_iCount; i++)
    {
        snprintf(label, sizeof(label), "No.%02d", i);
        labels.push_back(string(label));
    }

    _int iSelect = {};

    if (ImGui::BeginCombo("UI Choice", labels[m_iCurIndex].c_str()))
    {
        for (int i = 0; i < labels.size(); i++)
        {
            _bool isSelected = (m_iCurIndex == i);

            if (ImGui::Selectable(labels[i].c_str(), isSelected))
            {
                m_iCurIndex = i; // 선택된 항목 업데이트

                Initialize_UI();
            }

            if (isSelected)
                ImGui::SetItemDefaultFocus(); // 기본 포커스 설정
        }
        ImGui::EndCombo();
    }


    ImGui::Text("Resolution X : %d, Y : %d", (_int)m_vResolution.x, (_int)m_vResolution.y);
    
    ImGui::SameLine();

    if (ImGui::Button("Origin"))
    {
        m_pUI->Set_Scale(m_vResolution);
    }
    
    ImGui::Spacing();

    for (_int i = 0; i < m_TextureTag.size(); i++)
    {
        CTexture* pTexture = static_cast<CTexture*>(m_pGameInstance->Clone_Prototype(LEVEL_UITOOL, m_TextureTag[i], nullptr));

        _char szName[100];
        sprintf_s(szName, sizeof(szName), "Texture%d", i);
        if (true == ImGui::ImageButton(szName, (ImTextureID)pTexture->Get_SRV(0), ImVec2(64, 64)))
        {
            m_vResolution = pTexture->Get_Resolution();
            m_pUI->Clone_Texture(LEVEL_UITOOL, m_TextureTag[i]);
        }

        if (i % 4 < 3)
            ImGui::SameLine();

        Safe_Release(pTexture);
    }

    ImGui::Spacing();

    for (_int i = 0; i < m_SpriteTextureTag.size(); i++)
    {
        CTexture* pTexture = static_cast<CTexture*>(m_pGameInstance->Clone_Prototype(LEVEL_UITOOL, m_SpriteTextureTag[i], nullptr));

        _char szName[100];
        sprintf_s(szName, sizeof(szName), "Sprite Texture%d", i);
        if (true == ImGui::ImageButton(szName, (ImTextureID)pTexture->Get_SRV(0), ImVec2(64, 64)))
        {
            m_vResolution = pTexture->Get_Resolution();
            m_pUI->Clone_Texture(LEVEL_UITOOL, m_SpriteTextureTag[i]);
        }

        if (i % 4 < 3)
            ImGui::SameLine();

        Safe_Release(pTexture);
    }

    ImGui::Spacing();

    if (!bGuiHover && m_pGameInstance->Mouse_Press(MOUSEKEYSTATE::DIM_LB))
    {
        _float2 fMousePos = m_pGameInstance->Calulate_ScreenMousePos();

        m_vLocate.x = fMousePos.x;
        m_vLocate.y = fMousePos.y;

        m_pUI->Set_Locate(m_vLocate);
    }

    ImGui::DragFloat("ScaleX", &m_vScale.x, 0.1f, 0.0f, g_iWindowSizeX);
    ImGui::DragFloat("ScaleY", &m_vScale.y, 0.1f, 0.0f, g_iWindowSizeY);

    m_pUI->Set_Scale(m_vScale);

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    ImGui::DragFloat("LocateX", &m_vLocate.x, 0.1f, 0.0f, g_iWindowSizeX);
    ImGui::DragFloat("LocateY", &m_vLocate.y, 0.1f, 0.0f, g_iWindowSizeY);
    ImGui::DragFloat("LocateZ", &m_vLocate.z, 0.1f, 0.0f, 1.0f);

    m_pUI->Set_Locate(m_vLocate);

    ImGui::DragFloat("AxisX", &m_vAxis.x, 0.1f, 0.0f, 1.0f);
    ImGui::DragFloat("AxisY", &m_vAxis.y, 0.1f, 0.0f, 1.0f);
    ImGui::DragFloat("AxisZ", &m_vAxis.z, 0.1f, 0.0f, 1.0f);

    m_pUI->Set_Axis(m_vAxis);

    ImGui::DragFloat("Degrees", &m_fDegrees, 0.1f, 0.0f, 360.f);

    m_pUI->Set_Degrees(m_fDegrees);

    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    ImGui::DragFloat("SpriteSizeX", &m_vSpriteSize.x, 1.f, 0.0f, 20.f);
    ImGui::DragFloat("SpriteSizeY", &m_vSpriteSize.y, 1.f, 0.0f, 20.f);

    m_pUI->Set_SpriteSize(m_vSpriteSize);

    ImGui::DragFloat("SpriteIndexX", &m_vSpriteIndex.x, 1.f, 0.0f, 20.f);
    ImGui::DragFloat("SpriteIndexY", &m_vSpriteIndex.y, 1.f, 0.0f, 20.f);

    m_pUI->Set_SpriteIndex(m_vSpriteIndex);

    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    ImGui::DragFloat("Threshold", &m_fThreshold, 0.1f, 0.0f, 1.0f);
    m_pUI->Set_Threshold(m_fThreshold);

    ImGui::DragFloat("Interval", &m_fInterval, 0.1f, 0.0f, 5.f);
    m_pUI->Set_Interval(m_fInterval);

    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    ImGui::DragFloat("StretchRangeX", &m_vStretchRange.x, 0.1f, 0.0f, 1.f);
    ImGui::DragFloat("StretchRangeY", &m_vStretchRange.y, 0.1f, 0.0f, 1.f);
    m_pUI->Set_StretchRange(m_vStretchRange);

    ImGui::DragFloat("StretchFactor", &m_fStretchFactor, 1.0f, 0.0f, 100.f);
    m_pUI->Set_StretchFactor(m_fStretchFactor);

    string strFont = CUtils::ConvertToMultibyteChar(m_FontTag[m_iFontSelect]);

    // Combo를 생성하고, 현재 선택된 항목을 표시
    if (ImGui::BeginCombo("Font Choice", strFont.c_str()))
    {
        for (int i = 0; i < m_FontTag.size(); i++)
        {
            _bool isSelected = (m_iFontSelect == i);

            string strSelectFont = CUtils::ConvertToMultibyteChar(m_FontTag[i]);

            if (ImGui::Selectable(strSelectFont.c_str(), isSelected))
            {
                m_iFontSelect = i;
                m_pUI->Set_FontTag(m_FontTag[i]);
            }

            if (isSelected)
                ImGui::SetItemDefaultFocus(); // 기본 포커스 설정
        }
        ImGui::EndCombo();
    }

    ImGui::Text("SORTING");

    if (ImGui::RadioButton("NONE", m_iSortingNum == 0))
    {
        m_iSortingNum = 0;
        m_pUI->Set_FontSortType(SORTING::NONE);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("LEFT", m_iSortingNum == 1))
    {
        m_iSortingNum = 1;
        m_pUI->Set_FontSortType(SORTING::LEFT);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("MIDDLE", m_iSortingNum == 2))
    {
        m_iSortingNum = 2;
        m_pUI->Set_FontSortType(SORTING::MIDDLE);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("RIGHT", m_iSortingNum == 3))
    {
        m_iSortingNum = 3; 
        m_pUI->Set_FontSortType(SORTING::RIGHT);
    }

    ImGui::Text(u8"Font Text 입력");
    wstring strSaveName{};

    if (ImGui::InputText(u8"텍스트 내용 작성", m_szInputText, IM_ARRAYSIZE(m_szInputText)))
    {
        // 1. 변환할 유니코드 문자열의 크기 계산
        _int iResize = MultiByteToWideChar(CP_UTF8, 0, m_szInputText, -1, NULL, 0);

        // 2. wstring의 크기를 조정하여 메모리 확보
        strSaveName.resize(iResize - 1);

        // 3. 실제 변환 수행
        MultiByteToWideChar(CP_UTF8, 0, m_szInputText, -1, &strSaveName[0], iResize);

        // 4. 사용 예: UI 텍스트 설정
        m_pUI->Set_Text(strSaveName.c_str()); // wstring을 전달
    }

    ImGui::DragFloat("Font OffsetX", &m_vTextOffset.x, 10.f, 0.0f, 100.f);
    ImGui::DragFloat("Font OffsetY", &m_vTextOffset.y, 10.f, 0.0f, 100.f);
    m_pUI->Set_TextOffset(m_vTextOffset);

    ImGui::DragFloat("Font Scale", &m_fTextScale, 10.f, 0.0f, 100.f);
    m_pUI->Set_TextScale(m_fTextScale);

    ImGui::ColorEdit4("TextColor", reinterpret_cast<_float*>(&m_vTextColor));
    m_pUI->Set_TextColor(m_vTextColor);

    ImGui::ColorEdit4("HoverTextColor", reinterpret_cast<_float*>(&m_vHoverTextColor));
    m_pUI->Set_HoverTextColor(m_vHoverTextColor);

    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    for (_int i = 0; i < m_MaskTextureTag.size(); i++)
    {
        CTexture* pTexture = static_cast<CTexture*>(m_pGameInstance->Clone_Prototype(LEVEL_UITOOL, m_MaskTextureTag[i], nullptr));

        _char szName[100];
        sprintf_s(szName, sizeof(szName), "Mask Texture%d", i);
        if (true == ImGui::ImageButton(szName, (ImTextureID)pTexture->Get_SRV(0), ImVec2(64, 64)))
        {
            m_pUI->Clone_MaskTexture(LEVEL_UITOOL, m_MaskTextureTag[i]);
        }

        if (i % 4 < 3)
            ImGui::SameLine();

        Safe_Release(pTexture);
    }


    // Combo를 생성하고, 현재 선택된 항목을 표시
    if (ImGui::BeginCombo("Shader Choice", m_Shaderlabels[m_iShaderPass].c_str()))
    {
        for (int i = 0; i < m_Shaderlabels.size(); i++)
        {
            _bool isSelected = (m_iShaderPass == i);

            if (ImGui::Selectable(m_Shaderlabels[i].c_str(), isSelected))
            {
                m_iShaderPass = i;
                m_pUI->Set_ShaderPass(i);
            }

            if (isSelected)
                ImGui::SetItemDefaultFocus(); // 기본 포커스 설정
        }
        ImGui::EndCombo();
    }

    ImGui::ColorEdit4("SrcColor", reinterpret_cast<_float*>(&m_vSrcColor));
    m_pUI->Set_SrcColor(m_vSrcColor);

    ImGui::ColorEdit4("DstColor", reinterpret_cast<_float*>(&m_vDstColor));
    m_pUI->Set_DstColor(m_vDstColor);

    ImGui::Checkbox("Stretch Y", &m_bStretchY);
    m_pUI->Set_StretchY(m_bStretchY);

    ImGui::Checkbox("Right Angle", &m_bRightAngle);
    m_pUI->Set_RightAngle(m_bRightAngle);
}

void CLevel_UITool::Initialize_UI()
{
    m_pUI = static_cast<CUI_Dummy*>(m_pGameInstance->Find_GameObject(LEVEL_UITOOL, L"Layer_UI", m_iCurIndex));

    m_vScale = m_pUI->Get_Scale();
    m_vLocate = m_pUI->Get_Locate();
    m_vAxis = m_pUI->Get_Axis();
    m_fDegrees = m_pUI->Get_Degrees();

    m_vSpriteSize = m_pUI->Get_SpriteSize();
    m_vSpriteIndex = m_pUI->Get_SpriteIndex();

    m_fThreshold = m_pUI->Get_Threshold();
    m_fInterval = m_pUI->Get_Interval();
    
    m_vStretchRange = m_pUI->Get_StretchRange();
    m_fStretchFactor = m_pUI->Get_StretchFactor();

    m_vTextOffset = m_pUI->Get_TextOffset();
    m_fTextScale = m_pUI->Get_TextScale();
    m_vTextColor = m_pUI->Get_TextColor();
    m_vHoverTextColor = m_pUI->Get_HoverTextColor();

    m_iShaderPass = m_pUI->Get_ShaderPass();
    m_vSrcColor = m_pUI->Get_SrcColor();
    m_vDstColor = m_pUI->Get_DstColor();
}

void CLevel_UITool::Create_UI()
{
    CUIObject::UIOBJECT_DESC    Desc{};
    m_pGameInstance->Add_GameObject_To_Layer(LEVEL_UITOOL, L"Prototype_GameObject_UI", LEVEL_UITOOL, L"Layer_UI", &Desc);
        
    m_iCount++;

    m_iCurIndex = m_iCount - 1;
}

void CLevel_UITool::Delete_UI()
{
    m_pGameInstance->Delete_GameObject(LEVEL_UITOOL, L"Layer_UI", m_iCurIndex);

    m_iCount--;

    if (m_iCurIndex >= m_iCount)
        m_iCurIndex--;
}

void CLevel_UITool::Save_UI()
{
    TCHAR szCurrentDir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, szCurrentDir);

    OPENFILENAME FileName;
    _tchar szFile[MAX_PATH] = L"untitled.dat";  // 기본 파일 이름 설정

    ZeroMemory(&FileName, sizeof FileName);
    FileName.lStructSize = sizeof FileName;
    FileName.hwndOwner = g_hWnd;
    FileName.lpstrFilter = L"Text Files (*.dat)\0*.dat\0All Files (*.*)\0*.*\0";
    FileName.lpstrFile = szFile;
    FileName.nMaxFile = MAX_PATH;
    FileName.Flags = OFN_OVERWRITEPROMPT;
    FileName.lpstrInitialDir = szCurrentDir;
    FileName.lpstrDefExt = L"dat";  // 기본 확장자 설정

    // 파일 탐색기 열기
    if (GetSaveFileName(&FileName))
    {
        HANDLE hFile = CreateFile(FileName.lpstrFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (INVALID_HANDLE_VALUE == hFile)
        {
            MSG_BOX("Failed to Create File");
            return;
        }

        DWORD dwByte = 0;
        
        Initialize_UI();

        size_t length = {}; // 문자열 길이 저장 (널 문자는 포함하지 않음)
        _wstring strTextureTag = m_pUI->Get_TextureTag();
        _wstring strFontTag = m_pUI->Get_FontTag();
        _wstring strMaskTextureTag = m_pUI->Get_MaskTextureTag();

        WriteFile(hFile, &m_vScale, sizeof(_float2), &dwByte, nullptr);
        WriteFile(hFile, &m_vLocate, sizeof(_float3), &dwByte, nullptr);

        WriteFile(hFile, &m_vAxis, sizeof(_float3), &dwByte, nullptr);
        WriteFile(hFile, &m_fDegrees, sizeof(_float), &dwByte, nullptr);

        WriteFile(hFile, &m_vSpriteSize, sizeof(_float2), &dwByte, nullptr);
        WriteFile(hFile, &m_vSpriteIndex, sizeof(_float2), &dwByte, nullptr);

        WriteFile(hFile, &m_fThreshold, sizeof(_float), &dwByte, nullptr);
        WriteFile(hFile, &m_fInterval, sizeof(_float), &dwByte, nullptr);

        WriteFile(hFile, &m_vStretchRange, sizeof(_float2), &dwByte, nullptr);

        WriteFile(hFile, &m_vTextOffset, sizeof(_float2), &dwByte, nullptr);
        WriteFile(hFile, &m_fTextScale, sizeof(_float), &dwByte, nullptr);
        WriteFile(hFile, &m_vTextColor, sizeof(_float4), &dwByte, nullptr);
        WriteFile(hFile, &m_vHoverTextColor, sizeof(_float4), &dwByte, nullptr);

        length = strTextureTag.length();
        WriteFile(hFile, &length, sizeof(size_t), &dwByte, nullptr);
        WriteFile(hFile, strTextureTag.c_str(), sizeof(_tchar) * (length + 1), &dwByte, nullptr);

        length = strFontTag.length();
        WriteFile(hFile, &length, sizeof(size_t), &dwByte, nullptr);
        WriteFile(hFile, strFontTag.c_str(), sizeof(_tchar) * (length + 1), &dwByte, nullptr);

        length = strMaskTextureTag.length();
        WriteFile(hFile, &length, sizeof(size_t), &dwByte, nullptr);
        WriteFile(hFile, strMaskTextureTag.c_str(), sizeof(_tchar) * (length + 1), &dwByte, nullptr);

        WriteFile(hFile, &m_iShaderPass, sizeof(_uint), &dwByte, nullptr);
        WriteFile(hFile, &m_vSrcColor, sizeof(_float4), &dwByte, nullptr);
        WriteFile(hFile, &m_vDstColor, sizeof(_float4), &dwByte, nullptr);


        CloseHandle(hFile);
        MSG_BOX("Save Complete!");
    }
    SetCurrentDirectory(szCurrentDir);
}

void CLevel_UITool::Load_UI()
{
    TCHAR szCurrentDir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, szCurrentDir);

    OPENFILENAME FileName;
    _tchar szFile[MAX_PATH] = {};

    ZeroMemory(&FileName, sizeof FileName);
    FileName.lStructSize = sizeof FileName;
    FileName.hwndOwner = g_hWnd;
    FileName.lpstrFilter = L"Text Files (*.dat)\0*.dat\0All Files (*.*)\0*.*\0";
    FileName.lpstrFile = szFile;
    FileName.nMaxFile = MAX_PATH;
    FileName.lpstrTitle = L"Load";
    FileName.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    FileName.lpstrInitialDir = szCurrentDir;

    if (GetOpenFileName(&FileName))
    {
        HANDLE hFile = CreateFile(FileName.lpstrFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (INVALID_HANDLE_VALUE == hFile)
        {
            MSG_BOX("Failed to open file.");
            return;
        }

        _bool bResult = {};

        DWORD dwByte = 0;

        size_t length = {}; // 문자열 길이 저장 (널 문자는 포함하지 않음)
        _wstring strTextureTag = L"";
        _wstring strFontTag = L"";
        _wstring strMaskTextureTag = L"";

        bResult = ReadFile(hFile, &m_vScale, sizeof(_float2), &dwByte, nullptr);
        bResult = ReadFile(hFile, &m_vLocate, sizeof(_float3), &dwByte, nullptr);

        bResult = ReadFile(hFile, &m_vAxis, sizeof(_float3), &dwByte, nullptr);
        bResult = ReadFile(hFile, &m_fDegrees, sizeof(_float), &dwByte, nullptr);

        bResult = ReadFile(hFile, &m_vSpriteSize, sizeof(_float2), &dwByte, nullptr);
        bResult = ReadFile(hFile, &m_vSpriteIndex, sizeof(_float2), &dwByte, nullptr);

        bResult = ReadFile(hFile, &m_fThreshold, sizeof(_float), &dwByte, nullptr);
        bResult = ReadFile(hFile, &m_fInterval, sizeof(_float), &dwByte, nullptr);

        bResult = ReadFile(hFile, &m_vStretchRange, sizeof(_float2), &dwByte, nullptr);

        bResult = ReadFile(hFile, &m_vTextOffset, sizeof(_float2), &dwByte, nullptr);
        bResult = ReadFile(hFile, &m_fTextScale, sizeof(_float), &dwByte, nullptr);
        bResult = ReadFile(hFile, &m_vTextColor, sizeof(_float4), &dwByte, nullptr);
        bResult = ReadFile(hFile, &m_vHoverTextColor, sizeof(_float4), &dwByte, nullptr);

        length = strTextureTag.length();
        bResult = ReadFile(hFile, &length, sizeof(size_t), &dwByte, nullptr);
        strTextureTag.resize(length);
        bResult = ReadFile(hFile, &strTextureTag[0], sizeof(_tchar) * (length + 1), &dwByte, nullptr);

        length = strFontTag.length();
        bResult = ReadFile(hFile, &length, sizeof(size_t), &dwByte, nullptr);
        strFontTag.resize(length);
        bResult = ReadFile(hFile, &strFontTag[0], sizeof(_tchar) * (length + 1), &dwByte, nullptr);

        length = strMaskTextureTag.length();
        bResult = ReadFile(hFile, &length, sizeof(size_t), &dwByte, nullptr);
        strMaskTextureTag.resize(length);
        bResult = ReadFile(hFile, &strMaskTextureTag[0], sizeof(_tchar) * (length + 1), &dwByte, nullptr);

        bResult = ReadFile(hFile, &m_iShaderPass, sizeof(_uint), &dwByte, nullptr);
        bResult = ReadFile(hFile, &m_vSrcColor, sizeof(_float4), &dwByte, nullptr);
        bResult = ReadFile(hFile, &m_vDstColor, sizeof(_float4), &dwByte, nullptr);

        Create_UI();

        m_pUI = static_cast<CUI_Dummy*>(m_pGameInstance->Find_GameObject(LEVEL_UITOOL, L"Layer_UI", m_iCurIndex));
        m_pUI->Clone_Texture(LEVELID::LEVEL_UITOOL, strTextureTag);

        m_pUI->Set_Scale(m_vScale);
        m_pUI->Set_Locate(m_vLocate);
        m_pUI->Set_Axis(m_vAxis);
        m_pUI->Set_Degrees(m_fDegrees);

        m_pUI->Set_SpriteSize(m_vSpriteSize);
        m_pUI->Set_SpriteIndex(m_vSpriteIndex);

        m_pUI->Set_Threshold(m_fThreshold);
        m_pUI->Set_Interval(m_fInterval);

        m_pUI->Set_StretchRange(m_vStretchRange);
        m_pUI->Set_StretchFactor(m_fStretchFactor);

        m_pUI->Set_TextOffset(m_vTextOffset);
        m_pUI->Set_TextScale(m_fTextScale);
        m_pUI->Set_TextColor(m_vTextColor);
        m_pUI->Set_HoverTextColor(m_vHoverTextColor);

        m_pUI->Set_ShaderPass(m_iShaderPass);
        m_pUI->Set_SrcColor(m_vSrcColor);
        m_pUI->Set_DstColor(m_vDstColor);

        CloseHandle(hFile);

        MSG_BOX("Load Complete!");
    }
    SetCurrentDirectory(szCurrentDir);
}

HRESULT CLevel_UITool::Ready_Lights()
{
    if (FAILED(m_pGameInstance->SetUp_ShadowLight(XMVectorSet(-10.f, 10.f, -10.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
        XMConvertToRadians(60.f), g_iWindowSizeX / static_cast<_float>(g_iWindowSizeY), 0.1f, 1000.f)))
        return E_FAIL;

    LIGHT_DESC LightDesc{};
    ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));

    LightDesc.eType = LIGHT_TYPE::DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(1.0f, 1.0f, 1.0f, 1.f);
    LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(L"Light_Global", LightDesc)))
        return E_FAIL;

    return S_OK;
}

CLevel_UITool* CLevel_UITool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_UITool* pInstance = new CLevel_UITool{ pDevice, pContext };

    return pInstance;
}

void CLevel_UITool::Free()
{
    CLevel::Free();
}
