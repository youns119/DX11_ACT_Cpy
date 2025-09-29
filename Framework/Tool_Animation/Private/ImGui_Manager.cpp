#include "pch.h"
#include "Imgui_Manager.h"

#include "GameInstance.h"
#include "Animation.h"
#include "Bone.h"

#include "AnimObject.h"
#include "ToolPartObject.h"
#include "ToolCamera.h"
#include "CineCamera_Tool.h"
#include "CineActor_Tool.h"
#include "CameraDebug_Tool.h"

IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
{
}

HRESULT CImgui_Manager::Init(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.5f); // 반투명 배경 (알파 값 0.5)

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        //style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        style.Colors[ImGuiCol_WindowBg].w = 0.6f;
    }

    if (!ImGui_ImplWin32_Init(g_hWnd) ||
        !ImGui_ImplDX11_Init(_pDevice, _pContext))
        return E_FAIL;

    return S_OK;
}

void CImgui_Manager::Pre_Update(_float _fTimeDelta)
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void CImgui_Manager::Update(_float _fTimeDelta)
{
    if (ImGui::Begin("Tool"))
    {
        if (ImGui::BeginTabBar("AnimationTool"))
        {
            if (ImGui::BeginTabItem("Animation"))
            {
                m_eToolType = EToolType::EToolType_Animation;

                if (m_pAnimObject != nullptr)
                {
                    ImGui_AnimationTool();

                    // Animation Playing
                    {
                        if (m_pCurrAnimation->Get_Blendable())
                        {
                            if (m_iCurrAnimIndex == m_iPrevAnimIndex)
                            {
                                m_pAnimObject->SetUp_Animation(m_iPostAnimIndex, false);
                                m_pCurrAnimation = m_pAnimObject->Get_CurrAnimation();
                                m_iCurrAnimIndex = m_iPostAnimIndex;
                            }
                            else if (m_iCurrAnimIndex == m_iPostAnimIndex)
                            {
                                m_pAnimObject->SetUp_Animation(m_iPrevAnimIndex, false);
                                m_pCurrAnimation = m_pAnimObject->Get_CurrAnimation();
                                m_iCurrAnimIndex = m_iPrevAnimIndex;
                            }
                        }
                    }
                }

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Camera"))
            {
                m_eToolType = EToolType::EToolType_Camera;

                if (m_pAnimObject != nullptr)
                    ImGui_AnimationTool();

                if (!m_pCurrAnimation->Get_Playing())
                {
                    m_pAnimObject->SetUp_Animation(m_iPrevAnimIndex, false);
                    m_pCurrAnimation->Set_Active(false);
                    m_pCurrAnimation = m_pAnimObject->Get_CurrAnimation();
                    m_iCurrAnimIndex = m_iPrevAnimIndex;
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }

    ImGui::End();
}

void CImgui_Manager::Post_Update(_float _fTimeDelta)
{
}

HRESULT CImgui_Manager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    return S_OK;
}

void CImgui_Manager::Set_AnimObject(CAnimObject* _pAnimObject)
{
    m_pAnimObject = _pAnimObject;
    m_pAnimObject->SetUp_Animation(0, false);

    m_strAnimObject = L"Model_Player";

    m_pPrevAnimation = m_pAnimObject->Get_CurrAnimation();
    m_pCurrAnimation = m_pAnimObject->Get_CurrAnimation();
    m_pPostAnimation = m_pAnimObject->Get_CurrAnimation();

    m_pCineCamera = m_pAnimObject->Get_CineCamera();
}

void CImgui_Manager::Set_ToolCamera(CToolCamera* _pToolCamera)
{
    m_pToolCamera = _pToolCamera;
}

void CImgui_Manager::Set_PartObject(CToolPartObject* _pPartObject)
{
    m_pPartObject = _pPartObject;

    m_strPartObject = L"Model_Fork";
}

void CImgui_Manager::ImGui_AnimationTool()
{
    ImGui_AnimObject();
    ImGui_AnimProperty();
    ImGui_AnimationPlaying();
    ImGui_AnimNotify();

    ImGui_PartObject();
}

void CImgui_Manager::ImGui_AnimObject()
{
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate); // FPS

    // AnimObject 선택 Combo
    {
        vector<string> vecAnimObjectTag;
        vector<const _char*> vecAnimObject;
        static _int iPostAnimObject{};

        for (auto& AnimObjectTag : m_vecAnimObject)
        {
            string strAnimObject = CUtils::ConvertToMultibyteChar(AnimObjectTag);
            vecAnimObjectTag.push_back(strAnimObject);
        }

        for (auto& AnimObjectString : vecAnimObjectTag)
            vecAnimObject.push_back(AnimObjectString.c_str());

        ImGui::Text("AnimObject");
        if (ImGui::Combo("##AnimObject", &iPostAnimObject, vecAnimObject.data(), static_cast<_int>(vecAnimObject.size())))
        {
            m_pAnimObject->Change_Model(m_vecAnimObject[iPostAnimObject], LEVEL_TOOL);
            Set_AnimObject(m_pAnimObject);

            m_strAnimObject = m_vecAnimObject[iPostAnimObject];

            m_iPrevAnimIndex = 0;
            m_iCurrAnimIndex = 0;
            m_iPostAnimIndex = 0;

            m_bPrevAnimPlaying = true;
            m_bPostAnimPlaying = false;

            m_bPrevTyping = false;
            m_bPrevEditTyping = false;
            m_bPostTyping = false;
            m_bPostEditTyping = false;
        }
    }
}

void CImgui_Manager::ImGui_AnimProperty()
{
    ImGui::NewLine();

    // AnimationTool
    ImGui_AnimProperty_Prev();

    if (m_eToolType == EToolType::EToolType_Animation)
        ImGui_AnimProperty_Post();

    ImGui::NewLine();
    ImGui_AnimSave();

    // CameraTool
    if (m_eToolType == EToolType::EToolType_Camera)
        ImGui_CameraTool();

    ImGui::NewLine();
    if (m_eToolType == EToolType::EToolType_Camera)
        ImGuiDialogue_Camera();
}

void CImgui_Manager::ImGui_AnimProperty_Prev()
{
    vector<CAnimation*> vecAnimation = *m_pAnimObject->Get_Animations();
    vector<const _char*> vecPrevAnimationTag;
    vector<const _char*> vecPostAnimationTag;

    for (auto& Animation : vecAnimation)
    {
        vecPrevAnimationTag.push_back(Animation->Get_Name());
        vecPostAnimationTag.push_back(Animation->Get_Name());
    }

    // PrevAnimation
    {
        // PrevAnimation 선택 Combo
        ImGui::Text("PrevAnimation");
        if (ImGui::Combo("##PrevAnimation", &m_iPrevAnimIndex, vecPrevAnimationTag.data(), static_cast<_int>(vecPrevAnimationTag.size())))
        {
            m_pAnimObject->SetUp_Animation(m_iPrevAnimIndex, false);
            m_pAnimObject->Set_Animation_TrackPosition(0.f);

            m_pPrevAnimation = vecAnimation[m_iPrevAnimIndex];
            m_pCurrAnimation = m_pAnimObject->Get_CurrAnimation();
            m_pPrevAnimation->Set_Active(true);
            m_pPostAnimation->Set_Active(true);

            m_iCurrAnimIndex = m_iPrevAnimIndex;

            m_bPrevAnimPlaying = true;
            m_bPostAnimPlaying = false;
        }

        // PrevAnimation AnimProperty
        {
            _float fDuration = static_cast<_float>(m_pPrevAnimation->Get_Duration());
            _float fTrackPosition = static_cast<_float>(m_pPrevAnimation->Get_TrackPosition());

            // TrackPosition : Duration
            ImGui::Text("%d : %d", static_cast<_uint>(fTrackPosition), static_cast<_uint>(fDuration));

            // Animation BaseSpeed Drag
            ImGui::NewLine();
            _float fSpeed = m_pPrevAnimation->Get_BaseAnimSpeed();
            ImGui::SetNextItemWidth(120);
            if (ImGui::DragFloat("Prev Speed", &fSpeed, 0.01f, 0.f))
                m_pPrevAnimation->Set_BaseAnimSpeed(fSpeed);

            // Animation BaseLerpTime Drag
            _float fLerpTime = m_pPrevAnimation->Get_BaseLerpTime();
            ImGui::SetNextItemWidth(120);
            if (ImGui::DragFloat("Prev LerpTime", &fLerpTime, 0.01f, 0.f, 1.f))
                m_pPrevAnimation->Set_BaseLerpTime(fLerpTime);

            // Animation BlendInTime Drag
            _float fBlendInTime = m_pPrevAnimation->Get_BlendInTime();
            ImGui::SetNextItemWidth(120);
            if (ImGui::DragFloat("Prev BlendInTime", &fBlendInTime, 0.01f, 0.f, fDuration))
                m_pPrevAnimation->Set_BlendInTime(fBlendInTime);

            // Animation BlendOutTime Drag
            _float fBlendOutTime = m_pPrevAnimation->Get_BlendOutTime();
            ImGui::SetNextItemWidth(120);
            if (ImGui::DragFloat("Prev BlendOutTime", &fBlendOutTime, 0.01f, 0.f, fDuration))
                m_pPrevAnimation->Set_BlendOutTime(fBlendOutTime);

            // 루트 모션 설정 RadioButton
            _int iRootMotionSelected = static_cast<_int>(m_pPrevAnimation->Get_RootAnimation());
            CAnimation::ROOTMOTION eRootMotion = m_pPrevAnimation->Get_RootAnimation();
            // 루트 모션 None
            if (ImGui::RadioButton("Prev RootMotion_None", &iRootMotionSelected, 0))
            {
                eRootMotion = static_cast<CAnimation::ROOTMOTION>(iRootMotionSelected);
                m_pPrevAnimation->Set_RootAnimation(eRootMotion);
                m_pAnimObject->Reset_RootBone(m_iPrevAnimIndex);
            }
            ImGui::SameLine();
            // 루트 모션 Delete
            if (ImGui::RadioButton("Prev RootMotion_Delete", &iRootMotionSelected, 1))
            {
                eRootMotion = static_cast<CAnimation::ROOTMOTION>(iRootMotionSelected);
                m_pPrevAnimation->Set_RootAnimation(eRootMotion);
                m_pAnimObject->Reset_RootBone(m_iPrevAnimIndex);
            }
            //ImGui::SameLine();
            //// 루트 모션 Transform        
            //if (ImGui::RadioButton("Prev RootMotion_Transform", &iRootMotionSelected, 2))
            //{
            //    eRootMotion = static_cast<CAnimation::ROOTMOTION>(iRootMotionSelected);
            //    m_pPrevAnimation->Set_RootAnimation(eRootMotion);
            //    m_pAnimObject->Reset_RootBone(m_iPrevAnimIndex);
            //}
        }
    }
}

void CImgui_Manager::ImGui_AnimProperty_Post()
{
    vector<CAnimation*> vecAnimation = *m_pAnimObject->Get_Animations();
    vector<const _char*> vecPrevAnimationTag;
    vector<const _char*> vecPostAnimationTag;

    for (auto& Animation : vecAnimation)
    {
        vecPrevAnimationTag.push_back(Animation->Get_Name());
        vecPostAnimationTag.push_back(Animation->Get_Name());
    }

    // PostAnimation
    {
        // PostAnimation 선택 Combo
        ImGui::NewLine();
        ImGui::Text("PostAnimation");
        if (ImGui::Combo("##PostAnimation", &m_iPostAnimIndex, vecPostAnimationTag.data(), static_cast<_int>(vecPostAnimationTag.size())))
        {
            m_pAnimObject->SetUp_Animation(m_iPrevAnimIndex, false);
            m_pAnimObject->Set_Animation_TrackPosition(0.f);

            m_pPostAnimation = vecAnimation[m_iPostAnimIndex];
            m_pCurrAnimation = m_pAnimObject->Get_CurrAnimation();
            m_pPrevAnimation->Set_Active(true);
            m_pPostAnimation->Set_Active(true);

            m_iCurrAnimIndex = m_iPrevAnimIndex;

            m_bPrevAnimPlaying = true;
            m_bPostAnimPlaying = false;
        }

        // PostAnimation AnimProperty
        {
            _float fDuration = static_cast<_float>(m_pPostAnimation->Get_Duration());
            _float fTrackPosition = static_cast<_float>(m_pPostAnimation->Get_TrackPosition());

            // TrackPosition : Duration
            ImGui::Text("%d : %d", static_cast<_uint>(fTrackPosition), static_cast<_uint>(fDuration));

            // Animation BaseSpeed Drag
            ImGui::NewLine();
            _float fSpeed = m_pPostAnimation->Get_BaseAnimSpeed();
            ImGui::SetNextItemWidth(120);
            if (ImGui::DragFloat("Post Speed", &fSpeed, 0.01f, 0.f))
                m_pPostAnimation->Set_BaseAnimSpeed(fSpeed);

            // Animation BaseLerpTime Drag
            _float fLerpTime = m_pPostAnimation->Get_BaseLerpTime();
            ImGui::SetNextItemWidth(120);
            if (ImGui::DragFloat("Post LerpTime", &fLerpTime, 0.01f, 0.f, 1.f))
                m_pPostAnimation->Set_BaseLerpTime(fLerpTime);

            // Animation BlendInTime Drag
            _float fBlendInTime = m_pPostAnimation->Get_BlendInTime();
            ImGui::SetNextItemWidth(120);
            if (ImGui::DragFloat("Post BlendInTime", &fBlendInTime, 0.01f, 0.f, fDuration))
                m_pPostAnimation->Set_BlendInTime(fBlendInTime);

            // Animation BlendOutTime Drag
            _float fBlendOutTime = m_pPostAnimation->Get_BlendOutTime();
            ImGui::SetNextItemWidth(120);
            if (ImGui::DragFloat("Post BlendOutTime", &fBlendOutTime, 0.01f, 0.f, fDuration))
                m_pPostAnimation->Set_BlendOutTime(fBlendOutTime);

            // 루트 모션 설정 RadioButton
            _int iRootMotionSelected = static_cast<_int>(m_pPostAnimation->Get_RootAnimation());
            CAnimation::ROOTMOTION eRootMotion = m_pPostAnimation->Get_RootAnimation();
            // 루트 모션 None
            if (ImGui::RadioButton("Post RootMotion_None", &iRootMotionSelected, 0))
            {
                eRootMotion = static_cast<CAnimation::ROOTMOTION>(iRootMotionSelected);
                m_pPostAnimation->Set_RootAnimation(eRootMotion);
                m_pAnimObject->Reset_RootBone(m_iPostAnimIndex);
            }
            ImGui::SameLine();
            // 루트 모션 Delete
            if (ImGui::RadioButton("Post RootMotion_Delete", &iRootMotionSelected, 1))
            {
                eRootMotion = static_cast<CAnimation::ROOTMOTION>(iRootMotionSelected);
                m_pPostAnimation->Set_RootAnimation(eRootMotion);
                m_pAnimObject->Reset_RootBone(m_iPostAnimIndex);
            }
            //ImGui::SameLine();
            //// 루트 모션 Transform        
            //if (ImGui::RadioButton("Post RootMotion_Transform", &iRootMotionSelected, 2))
            //{
            //    eRootMotion = static_cast<CAnimation::ROOTMOTION>(iRootMotionSelected);
            //    m_pPostAnimation->Set_RootAnimation(eRootMotion);
            //    m_pAnimObject->Reset_RootBone(m_iPostAnimIndex);
            //}
        }
    }
}

void CImgui_Manager::ImGui_AnimationPlaying()
{
    ImGui::Begin("Animation");

    ImGui_AnimationPlaying_Prev();

    if (m_eToolType == EToolType::EToolType_Animation)
        ImGui_AnimationPlaying_Post();
    else if (m_eToolType == EToolType::EToolType_Camera)
        ImGui_CameraPlaying();

    ImGui_AnimationPlayingButton();

    ImGui::End();
}

void CImgui_Manager::ImGui_AnimationPlaying_Prev()
{
    // PrevAnimation PlayBar
    {
        ImGui::Text("PrevAnimation");

        _double fPrevDuration = m_pPrevAnimation->Get_Duration();
        _double fPrevBlendInTime = static_cast<_double>(m_pPrevAnimation->Get_BlendInTime());
        _double fPrevBlendOutTime = static_cast<_double>(m_pPrevAnimation->Get_BlendOutTime());
        _double fPrevTrackPosition = m_pPrevAnimation->Get_TrackPosition();

        // PrevAnimation Duration Slider
        _double fMixValue{ 0.f };
        _double fTotalTime = fPrevBlendOutTime;
        ImGui::SetNextItemWidth(500);
        if (ImGui::SliderScalar("##PrevDuration", ImGuiDataType_Double, &fPrevTrackPosition, &(fMixValue), &(fTotalTime), "%.2f"))
        {
            if (m_iCurrAnimIndex != m_iPrevAnimIndex)
            {
                m_pCurrAnimation->Set_Active(true);
                m_pAnimObject->SetUp_Animation(m_iPrevAnimIndex, false);
                m_iCurrAnimIndex = m_iPrevAnimIndex;
                m_pCurrAnimation = m_pAnimObject->Get_CurrAnimation();
            }

            m_pCurrAnimation->Set_Active(false);
            m_pAnimObject->Set_Animation_TrackPosition(fPrevTrackPosition);

            //if (m_eToolType == EToolType::EToolType_Camera)
            //{
            //    _double fCurrTime = m_pCurrAnimation->Get_TrackPosition() /
            //        (m_pCurrAnimation->Get_TickPerSecond() * m_pCurrAnimation->Get_BaseAnimSpeed());

            //    m_pCineCamera->Set_CurrTime(static_cast<_float>(fCurrTime));
            //}
        }

        ImGui::SameLine();
        ImGui::Text("%d : %d", static_cast<_uint>(fPrevTrackPosition), static_cast<_uint>(fTotalTime));
    }
}

void CImgui_Manager::ImGui_AnimationPlaying_Post()
{
    // PostAnimation PlayBar
    {
        ImGui::Text("PostAnimation");

        _double fPostDuration = m_pPostAnimation->Get_Duration();
        _double fPostBlendInTime = static_cast<_double>(m_pPostAnimation->Get_BlendInTime());
        _double fPostBlendOutTime = static_cast<_double>(m_pPostAnimation->Get_BlendOutTime());
        _double fPostTrackPosition = m_pPostAnimation->Get_TrackPosition();

        // PrevAnimation Duration Slider
        _double fMixValue{ 0.f };
        _double fTotalTime = fPostBlendOutTime;
        ImGui::SetNextItemWidth(500);
        if (ImGui::SliderScalar("##PostDuration", ImGuiDataType_Double, &fPostTrackPosition, &(fMixValue), &(fTotalTime), "%.2f"))
        {
            if (m_iCurrAnimIndex != m_iPostAnimIndex)
            {
                m_pCurrAnimation->Set_Active(true);
                m_pAnimObject->SetUp_Animation(m_iPostAnimIndex, false);
                m_iCurrAnimIndex = m_iPostAnimIndex;
                m_pCurrAnimation = m_pAnimObject->Get_CurrAnimation();
            }

            m_pCurrAnimation->Set_Active(false);
            m_pAnimObject->Set_Animation_TrackPosition(fPostTrackPosition);
        }

        // TrackPosition : Duration
        ImGui::SameLine();
        ImGui::Text("%d : %d", static_cast<_uint>(fPostTrackPosition), static_cast<_uint>(fTotalTime));
    }
}

void CImgui_Manager::ImGui_AnimationPlayingButton()
{
    // Play / Stop Button
    ImGui::SetCursorPos(ImVec2(350, 120));
    if ((ImGui::Button("Play/Stop") || CGameInstance::GetInstance()->Key_Down(KEY::SPACE)))
    {
        if (m_eToolType == EToolType::EToolType_Animation)
        {
            if (!m_bPrevTyping && !m_bPostTyping && !m_bPrevEditTyping && !m_bPostEditTyping)
                m_pCurrAnimation->Set_Active(!m_pCurrAnimation->Get_Active());
        }
        else if (m_eToolType == EToolType::EToolType_Camera)
        {
            if (!m_bPrevTyping && !m_bPrevEditTyping)
            {
                if (m_pCurrAnimation->Get_Playing() && !m_pCurrAnimation->Get_Active())
                    m_pCurrAnimation->Set_Active(!m_pCurrAnimation->Get_Active());
                else m_pAnimObject->SetUp_Animation(m_iCurrAnimIndex, false);

                m_pCineCamera->Play(m_bCineCamera);
            }
        }
    }
}

void CImgui_Manager::ImGui_AnimNotify()
{
    ImGui_PrevAnimNotify();

    if (m_eToolType == EToolType::EToolType_Animation)
        ImGui_PostAnimNotify();
    else if (m_eToolType == EToolType::EToolType_Camera)
        ImGui_CameraList();
}

void CImgui_Manager::ImGui_PrevAnimNotify()
{
    ImGui::Begin("PrevAnimation Notify");

    // PrevAnimation AnimNotify
    {
        ImGui::Text("PrevAnimation AnimNotify");

        _double fPrevTrackPosition = m_pPrevAnimation->Get_TrackPosition();

        queue<pair<_double, string>> queTemp{};
        queTemp = m_pPrevAnimation->Get_AnimNotify();

        vector<pair<_double, string>> vecAnimNotify;
        vector<string> vecAnimNotifyName;
        vector<const _char*> vecNotifyName;

        static string strPrevName{};

        // AnimNotify 이름 입력 Text 박스
        static _char szPrevBuffer[MAX_PATH]{};
        ImGui::Text("Notify Name");
        if (ImGui::InputText("##PrevNotify Name", szPrevBuffer, IM_ARRAYSIZE(szPrevBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            string strNotifyName = string(szPrevBuffer);
            m_pAnimObject->Add_AnimNotify(m_iPrevAnimIndex, fPrevTrackPosition, strNotifyName);

            m_bPrevTyping = false;
            m_iPrevNotifyIndex = 0;

            strPrevName = strNotifyName;
            m_fPrevNotifyTrackPosition = fPrevTrackPosition;
        }
        if (ImGui::IsItemActive()) m_bPrevTyping = true;
        else m_bPrevTyping = false;

        while (!queTemp.empty())
        {
            pair<_double, string> pairTemp = queTemp.front();
            queTemp.pop();

            vecAnimNotify.push_back(pairTemp);
        }

        for (auto& AnimNotify : vecAnimNotify)
            vecAnimNotifyName.push_back(AnimNotify.second);

        for (auto& NotifyName : vecAnimNotifyName)
            vecNotifyName.push_back(NotifyName.c_str());

        // AnimNotify ListBox
        ImGui::Text("Notify List");
        static _double fPrevCurrPosition{};
        static _char szPrevBufferTemp[MAX_PATH];
        if (ImGui::ListBox("##PrevNotify List", &m_iPrevNotifyIndex, vecNotifyName.data(), static_cast<_int>(vecNotifyName.size())))
        {
            fPrevCurrPosition = m_fPrevNotifyTrackPosition = vecAnimNotify[m_iPrevNotifyIndex].first;
            strPrevName = vecAnimNotify[m_iPrevNotifyIndex].second;
            strcpy_s(szPrevBufferTemp, strPrevName.c_str());

            if (m_iCurrAnimIndex != m_iPrevAnimIndex)
            {
                m_pCurrAnimation->Set_Active(true);
                m_pAnimObject->SetUp_Animation(m_iPrevAnimIndex, false);
                m_iCurrAnimIndex = m_iPrevAnimIndex;
                m_pCurrAnimation = m_pAnimObject->Get_CurrAnimation();
            }

            m_pCurrAnimation->Set_Active(false);
            m_pAnimObject->Set_Animation_TrackPosition(m_fPrevNotifyTrackPosition);
        }

        // AnimNotify의 TrackPosition, Name Text
        if (m_iPrevNotifyIndex >= 0)
        {
            ImGui::NewLine();
            ImGui::Text("Notify Name : %s", strPrevName.c_str());
            ImGui::Text("Notify TrackPosition : %.2lf", fPrevCurrPosition);

            _double fDuration = m_pPrevAnimation->Get_Duration();
            _double fPrevMinValue{};

            // AnimNotify 이름 입력 Text 박스
            static _string strPrevTemp{};
            if (ImGui::InputText("##PrevNotify Edit Name", szPrevBufferTemp, IM_ARRAYSIZE(szPrevBufferTemp)))
            {
                strPrevTemp = string(szPrevBufferTemp);

                m_bPrevEditTyping = false;
            }
            if (ImGui::IsItemActive()) m_bPrevEditTyping = true;
            else m_bPrevEditTyping = false;

            ImGui::DragScalar("##Prev Notify TrackPosition", ImGuiDataType_Double, &m_fPrevNotifyTrackPosition, 0.01f, &fPrevMinValue, &fDuration, "%.2f");

            if (ImGui::Button("Prev Edit"))
            {
                m_pAnimObject->Change_AnimNotify(m_iPrevAnimIndex, strPrevName, strPrevTemp, fPrevCurrPosition, m_fPrevNotifyTrackPosition);
                strPrevName = strPrevTemp;
                m_iPrevNotifyIndex = -1;
            }

            if (ImGui::Button("Prev Delete"))
            {
                m_pAnimObject->Delete_AnimNotify(m_iPrevAnimIndex, strPrevName, m_fPrevNotifyTrackPosition);
                m_iPrevNotifyIndex = -1;
            }
        }
    }

    ImGui::End();
}

void CImgui_Manager::ImGui_PostAnimNotify()
{
    ImGui::Begin("PostAnimation Notify");

    // PostAnimation AnimNotify
    {
        ImGui::Text("PostAnimation AnimNotify");

        _double fPostTrackPosition = m_pPostAnimation->Get_TrackPosition();

        queue<pair<_double, string>> queTemp{};
        queTemp = m_pPostAnimation->Get_AnimNotify();

        vector<pair<_double, string>> vecAnimNotify;
        vector<string> vecAnimNotifyName;
        vector<const _char*> vecNotifyName;

        static string strPostName{};

        // AnimNotify 이름 입력 Text 박스
        static _char szPostBuffer[MAX_PATH]{};
        ImGui::Text("Notify Name");
        if (ImGui::InputText("##PostNotify Name", szPostBuffer, IM_ARRAYSIZE(szPostBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            string strNotifyName = string(szPostBuffer);
            m_pAnimObject->Add_AnimNotify(m_iPostAnimIndex, fPostTrackPosition, strNotifyName);

            m_bPostTyping = false;
            m_iPostNotifyIndex = 0;

            strPostName = strNotifyName;
            m_fPostNotifyTrackPosition = fPostTrackPosition;
        }
        if (ImGui::IsItemActive()) m_bPostTyping = true;
        else m_bPostTyping = false;

        while (!queTemp.empty())
        {
            pair<_double, string> pairTemp = queTemp.front();
            queTemp.pop();

            vecAnimNotify.push_back(pairTemp);
        }

        for (auto& AnimNotify : vecAnimNotify)
            vecAnimNotifyName.push_back(AnimNotify.second);

        for (auto& NotifyName : vecAnimNotifyName)
            vecNotifyName.push_back(NotifyName.c_str());

        // AnimNotify ListBox
        ImGui::Text("Notify List");
        static _double fPostCurrPosition{};
        static _char szPostBufferTemp[MAX_PATH];
        if (ImGui::ListBox("##PostNotify List", &m_iPostNotifyIndex, vecNotifyName.data(), static_cast<_int>(vecNotifyName.size())))
        {
            fPostCurrPosition = m_fPostNotifyTrackPosition = vecAnimNotify[m_iPostNotifyIndex].first;
            strPostName = vecAnimNotify[m_iPostNotifyIndex].second;
            strcpy_s(szPostBufferTemp, strPostName.c_str());

            if (m_iCurrAnimIndex != m_iPostAnimIndex)
            {
                m_pCurrAnimation->Set_Active(true);
                m_pAnimObject->SetUp_Animation(m_iPostAnimIndex, false);
                m_iCurrAnimIndex = m_iPostAnimIndex;
                m_pCurrAnimation = m_pAnimObject->Get_CurrAnimation();
            }

            m_pCurrAnimation->Set_Active(false);
            m_pAnimObject->Set_Animation_TrackPosition(m_fPostNotifyTrackPosition);
        }

        // AnimNotify의 TrackPosition, Name Text
        if (m_iPostNotifyIndex >= 0)
        {
            ImGui::NewLine();
            ImGui::Text("Notify Name : %s", strPostName.c_str());
            ImGui::Text("TrackPosition : %.2lf", fPostCurrPosition);

            _double fDuration = m_pPostAnimation->Get_Duration();
            _double fPostMinValue{};

            // AnimNotify 이름 입력 Text 박스
            static _string strPostTemp{};
            if (ImGui::InputText("##PostNotify Edit Name", szPostBufferTemp, IM_ARRAYSIZE(szPostBufferTemp)))
            {
                strPostTemp = string(szPostBufferTemp);

                m_bPostEditTyping = false;
            }
            if (ImGui::IsItemActive()) m_bPostEditTyping = true;
            else m_bPostEditTyping = false;

            ImGui::DragScalar("##Post Notify TrackPosition", ImGuiDataType_Double, &m_fPostNotifyTrackPosition, 0.01f, &fPostMinValue, &fDuration, "%.2f");

            if (ImGui::Button("Post Edit"))
            {
                m_pAnimObject->Change_AnimNotify(m_iPostAnimIndex, strPostName, strPostTemp, fPostCurrPosition, m_fPostNotifyTrackPosition);
                strPostName = strPostTemp;
                m_iPostNotifyIndex = -1;
            }

            if (ImGui::Button("Post Delete"))
            {
                m_pAnimObject->Delete_AnimNotify(m_iPostAnimIndex, strPostName, m_fPostNotifyTrackPosition);
                m_iPostNotifyIndex = -1;
            }
        }
    }

    ImGui::End();
}

void CImgui_Manager::ImGui_AnimSave()
{
    // Animation 데이터 세이브
    {
        // 단축키 세이브
        if (CGameInstance::GetInstance()->Key_Press(KEY::LCONTROL))
        {
            if (CGameInstance::GetInstance()->Key_Down(KEY::S))
            {
                if (CGameInstance::GetInstance()->Key_Press(KEY::LSHIFT)) Save_Animation_All();
                else Save_Animation();
            }
        }

        // Button 세이브
        if (ImGui::Button("Animation Save"))
            Save_Animation();

        if (ImGui::Button("All Save"))
            Save_Animation_All();
    }
}

HRESULT CImgui_Manager::Save_Animation()
{
    // Animation 세이브
    if (FAILED(m_pAnimObject->Save_Animation(m_iPrevAnimIndex)) ||
        FAILED(m_pAnimObject->Save_Animation(m_iPostAnimIndex)))
    {
        MSG_BOX("Failed To Save Animation");
        return E_FAIL;
    }

    MSG_BOX("Animation Save Success");

    return S_OK;
}

HRESULT CImgui_Manager::Save_Animation_All()
{
    // 전체 Animation 세이브
    vector<CAnimation*> vecAnimation = *m_pAnimObject->Get_Animations();

    for (_uint i = 0; i < m_pAnimObject->Get_NumAnimation(); i++)
    {
        if (FAILED(m_pAnimObject->Save_Animation(i)))
        {
            MSG_BOX("Failed To Save Animations");
            return E_FAIL;
        }
    }

    MSG_BOX("All Animations Save Success");

    return S_OK;
}

void CImgui_Manager::ImGui_PartObject()
{
    ImGui::Begin("PartObject");

    // PartObject ImGui On / Off
    if (ImGui::Button("PartObject"))
    {
        m_bPartObject = !m_bPartObject;
        m_pPartObject->Set_Show(m_bPartObject);
    }

    if (!m_bPartObject)
    {
        ImGui::End();
        return;
    }

    ImGui_PartObject_Property();
    ImGui_PartObject_Transform();
    ImGuiDialogue_PartObject();

    ImGui::End();
}

void CImgui_Manager::ImGui_PartObject_Property()
{
    // PartObject 선택 Combo
    {
        // PartObject
        vector<string> vecPartObjectTag;
        vector<const _char*> vecPartObject;
        static _int iPostPartObject{};

        for (auto& PartObjectTag : m_vecPartObject)
        {
            string strPartObject = CUtils::ConvertToMultibyteChar(PartObjectTag);
            vecPartObjectTag.push_back(strPartObject);
        }

        for (auto& PartObjectString : vecPartObjectTag)
            vecPartObject.push_back(PartObjectString.c_str());

        ImGui::SetNextItemWidth(150);
        if (ImGui::Combo("##PartObject", &iPostPartObject, vecPartObject.data(), static_cast<_int>(vecPartObject.size())))
        {
            m_pPartObject->Change_Model(m_vecPartObject[iPostPartObject], LEVEL_TOOL);
            Set_PartObject(m_pPartObject);
        }

        // SocketBone
        vector<CBone*> vecBones = *m_pAnimObject->Get_Bones();
        vector<const _char*> vecBoneTag;
        static _int iBone{ -1 };

        for (auto& Bone : vecBones)
            vecBoneTag.push_back(Bone->Get_Name());

        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        if (ImGui::Combo("##Bone", &iBone, vecBoneTag.data(), static_cast<_uint>(vecBoneTag.size())))
        {
            m_szBoneName = vecBoneTag[iBone];
            const _float4x4* matSocketMatrix = m_pAnimObject->Get_CombinedMatrix(m_szBoneName);
            m_pPartObject->Set_SocketBone(matSocketMatrix);
        }
    }
}

void CImgui_Manager::ImGui_PartObject_Transform()
{
    CTransform* pTransform = static_cast<CTransform*>(m_pPartObject->Find_Component(g_strTransformTag));

    _vec3 vScale = pTransform->Get_Scale();
    _vec3 vRotation = pTransform->Get_Rotation();
    _vec3 vTranslation = pTransform->Get_State(CTransform::STATE::POSITION);

    ImGui::NewLine();

    _float fScale[3] = { vScale.x, vScale.y, vScale.z };
    _float m_vRotation[3] = { vRotation.x, vRotation.y, vRotation.z };
    _float fTranslation[3] = { vTranslation.x, vTranslation.y, vTranslation.z };

    if (ImGui::DragFloat3("Scale", fScale, 0.01f))
        vScale = _float3(fScale[0], fScale[1], fScale[2]);
    if (ImGui::DragFloat3("Rotation", m_vRotation, 0.01f, -180.f, 180.f))
    {
        vRotation = _float3(m_vRotation[0], m_vRotation[1], m_vRotation[2]);
        pTransform->Rotate(vRotation.x, vRotation.y, vRotation.z);
    }
    if (ImGui::DragFloat3("Translation", fTranslation, 0.01f))
        vTranslation = _float3(fTranslation[0], fTranslation[1], fTranslation[2]);

    pTransform->Set_Scale(vScale.x, vScale.y, vScale.z);
    pTransform->Set_State(CTransform::STATE::POSITION, vTranslation._vector());

    ImGui::NewLine();
}

HRESULT CImgui_Manager::Save_PartObject(string _strFile)
{
    _wstring strFile = CUtils::ConvertToWideChar(_strFile);

    _ulong dwByte = 0;
    HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed To Save PartObject");
        return E_FAIL;
    }

    _char szSocketBone[MAX_PATH]{};
    if (m_szBoneName == nullptr)
        strcpy_s(szSocketBone, "NoSocket");
    else strcpy_s(szSocketBone, m_szBoneName);

    WriteFile(hFile, &szSocketBone, sizeof(_char) * MAX_PATH, &dwByte, nullptr);

    CTransform* pTransform = static_cast<CTransform*>(m_pPartObject->Find_Component(g_strTransformTag));

    _float3 vScale = pTransform->Get_Scale();
    _float3 vRotation = pTransform->Get_Rotation();
    _float3 vTranslation = _vec3(pTransform->Get_State(CTransform::STATE::POSITION))._float3();

    WriteFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
    WriteFile(hFile, &vRotation, sizeof(_float3), &dwByte, nullptr);
    WriteFile(hFile, &vTranslation, sizeof(_float3), &dwByte, nullptr);

    CloseHandle(hFile);

    MSG_BOX("PartObject Save Success");

    return S_OK;
}

HRESULT CImgui_Manager::Load_PartObject(string _strFile)
{
    _wstring strFile = CUtils::ConvertToWideChar(_strFile);

    _bool bResult{};
    _ulong dwByte{};
    HANDLE hFile{};
    hFile = CreateFile(strFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed To Load PartObject Data");
        return E_FAIL;
    }

    _char szSocketBone[MAX_PATH]{};

    bResult = ReadFile(hFile, &szSocketBone, sizeof(_char) * MAX_PATH, &dwByte, nullptr);

    _float3 vScale{};
    _float3 vRotation{};
    _float3 vTranslation{};

    bResult = ReadFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
    bResult = ReadFile(hFile, &vRotation, sizeof(_float3), &dwByte, nullptr);
    bResult = ReadFile(hFile, &vTranslation, sizeof(_float3), &dwByte, nullptr);

    CloseHandle(hFile);

    if (!strcmp(szSocketBone, "NoSocket"))
        m_pPartObject->Set_SocketBone(nullptr);
    else
    {
        const _float4x4* matSocketMatrix = m_pAnimObject->Get_CombinedMatrix(szSocketBone);
        m_pPartObject->Set_SocketBone(matSocketMatrix);
    }

    m_szBoneName = szSocketBone;

    CTransform* pTransform = static_cast<CTransform*>(m_pPartObject->Find_Component(g_strTransformTag));

    pTransform->Set_Scale(vScale);
    pTransform->Rotate(vRotation.x, vRotation.y, vRotation.z);
    pTransform->Set_State(CTransform::STATE::POSITION, XMLoadFloat3(&vTranslation));

    MSG_BOX("PartObject Load Success");

    return S_OK;
}

HRESULT CImgui_Manager::ImGuiDialogue_PartObject()
{
    if (ImGui::Button("Save"))
    {
        IGFD::FileDialogConfig config;
        config.path = "../Bin/DataFiles/PartObjectData";

        ImGuiFileDialog::Instance()->OpenDialog("Save File", "Choose File", ".data", config);
    }

    if (ImGuiFileDialog::Instance()->Display("Save File"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            if (FAILED(Save_PartObject(filePathName)))
                return E_FAIL;
        }

        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::SameLine();

    if (ImGui::Button("Load"))
    {
        IGFD::FileDialogConfig config;
        config.path = "../Bin/DataFiles/PartObjectData";

        ImGuiFileDialog::Instance()->OpenDialog("Load File", "Choose File", ".data", config);
    }

    if (ImGuiFileDialog::Instance()->Display("Load File"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            if (FAILED(Load_PartObject(filePathName)))
                return E_FAIL;
        }

        ImGuiFileDialog::Instance()->Close();
    }

    return S_OK;
}

void CImgui_Manager::ImGui_CameraTool()
{
    ImGui::NewLine();

    CTransform* pTransform = static_cast<CTransform*>(m_pToolCamera->Find_Component(g_strTransformTag));
    _matrix matTransform = pTransform->Get_WorldMatrix();

    _vec3 vPos = pTransform->Get_State(CTransform::STATE::POSITION);
    _vec3 vRot = pTransform->Get_Rotation();

    ImGui::Text("Current Camera");
    ImGui::Text("Pos  : (%.2f, %.2f, %.2f)", vPos.x, vPos.y, vPos.z);
    ImGui::Text("Rot  : (% .2f, % .2f, % .2f)", vRot.x, vRot.y, vRot.z);

    ImGui::NewLine();

    // Time
    static _float fCineActorTime{};
    ImGui::DragFloat("Time", &fCineActorTime, 0.01f, 0.f);

    ImGui::SameLine();

    if (ImGui::Button("##AdjustButton1", ImVec2(20.f, 20.f)))
    {
        _double fAnimTime = m_pCurrAnimation->Get_TrackPosition() /
            (m_pCurrAnimation->Get_TickPerSecond() * m_pCurrAnimation->Get_BaseAnimSpeed());
        fCineActorTime = static_cast<_float>(fAnimTime);
    }

    ImGui::NewLine();

    if (ImGui::Button("Add Camera"))
        m_pCineCamera->Add_CineActor
        (
            0, 
            fCineActorTime,
            0.1f, 
            0,
            vPos._float3(), 
            vRot._float3(), 
            vPos._float3(), 
            vPos._float3(), 
            vPos._float3()
        );

    if (CGameInstance::GetInstance()->Key_Down(KEY::F))
        m_pCineCamera->Add_CineActor
        (
            0, 
            fCineActorTime, 
            0.1f, 
            0,
            vPos._float3(), 
            vRot._float3(), 
            vPos._float3(), 
            vPos._float3(),
            vPos._float3()
        );

    ImGui::NewLine();

    if (ImGui::Checkbox("Camera Preview", &m_bDrawTrajectory))
        m_pCineCamera->Set_Draw(m_bDrawTrajectory);

    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.f);

    _float fDrawDelta = m_pCineCamera->Get_DrawDelta();
    if (ImGui::DragFloat("##DrawDelta", &fDrawDelta, 0.01f, 0.f))
        m_pCineCamera->Set_DrawDelta(fDrawDelta);

    ImGui::Checkbox("CineCamera", &m_bCineCamera);

    ImGui::NewLine();
    if (ImGui::Button("Clear"))
        m_pCineCamera->Clear();

    if (ImGui::Button("Reset Camera"))
        CGameInstance::GetInstance()->Camera_Transition(L"Camera_Free", (_uint)CCameraManager::LERP::LERP_EASEINOUT, 0.5f);
}

void CImgui_Manager::ImGui_CameraList()
{
    ImGui::Begin("CameraList");

    list<CCineActor_Tool*>* listCineActor = m_pCineCamera->Get_CineActorList();
    vector<CCineActor_Tool*> vecCineActor;
    vector<const _char*> vecCineActorName;

    for (auto& CineActor : *listCineActor)
    {
        vecCineActor.push_back(CineActor);
        vecCineActorName.push_back(CineActor->Get_CineActor()->strName.c_str());
    }

    // CineActor ListBox
    ImGui::Text("CineActor List");
    static string strCameraName{};
    static _int iCineActorIndex = -1;
    static CCineActor_Tool* pCineActor{};

    ImGui::BeginChild("CineActorListChild", ImVec2(0, 200), true);
    for (int i = 0; i < static_cast<int>(vecCineActorName.size()); ++i)
    {
        _bool bSelected = (iCineActorIndex == i);

        if (ImGui::Selectable(vecCineActorName[i], bSelected))
        {
            if (m_iPrevCineActorIndex != -1)
                vecCineActor[m_iPrevCineActorIndex]->Change_ColliderColor(false);

            iCineActorIndex = i;
            strCameraName = vecCineActorName[i];
            pCineActor = vecCineActor[i];

            vecCineActor[i]->Change_ColliderColor(true);
            m_iPrevCineActorIndex = iCineActorIndex;
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            if (pCineActor)
            {
                _vec3 vPosition = pCineActor->Get_Transform()->Get_State(CTransform::STATE::POSITION);
                _vec3 vRotation = pCineActor->Get_Transform()->Get_Rotation();
                m_pToolCamera->ReLocate(vPosition, vRotation);
            }
        }
    }

    ImGui::EndChild();

    // AnimNotify의 TrackPosition, Name Text
    if (iCineActorIndex >= 0)
    {
        ImGui::NewLine();
        ImGui::Text("Camera Name : %s", strCameraName.c_str());

        // Position
        _vec3 vPos = pCineActor->Get_Transform()->Get_State(CTransform::STATE::POSITION);
        _float fPos[3] = { vPos.x, vPos.y, vPos.z };
        if (ImGui::DragFloat3("Pos", fPos, 0.01f))
        {
            vPos = XMVectorSet(fPos[0], fPos[1], fPos[2], 1.f);
            pCineActor->Get_Transform()->Set_State(CTransform::STATE::POSITION, vPos._vector());
        }

        // Rotation
        _vec3 vRot = pCineActor->Get_Transform()->Get_Rotation();
        _float fRot[3] = { vRot.x, vRot.y, vRot.z };
        if (ImGui::DragFloat3("Rot", fRot, 0.01f))
        {
            vPos = XMVectorSet(fRot[0], fRot[1], fRot[2], 0.f);
            pCineActor->Get_Transform()->Rotate(fRot[0], fRot[1], fRot[2]);
        }

        ImGui::NewLine();

        ImGui::Text("PathType");
        _int iPathType = static_cast<_int>(pCineActor->Get_CineActor()->ePathType);
        if (ImGui::RadioButton("Path_Linear", iPathType == 0))
            pCineActor->Set_PathType(0);
        if (ImGui::RadioButton("Path_Bazier", iPathType == 1))
            pCineActor->Set_PathType(1);
        if (ImGui::RadioButton("Path_CatmullRom", iPathType == 2))
            pCineActor->Set_PathType(2);

        ImGui::NewLine();

        _float fCurrTime = pCineActor->Get_CineActor()->fTime;
        if (ImGui::DragFloat("Time", &fCurrTime, 0.01f, 0.f))
            pCineActor->Get_CineActor()->fTime = fCurrTime;

        ImGui::SameLine();

        if (ImGui::Button("##AdjustButton2", ImVec2(20.f, 20.f)))
        {
            _double fAnimTime = m_pCurrAnimation->Get_TrackPosition() /
                (m_pCurrAnimation->Get_TickPerSecond() * m_pCurrAnimation->Get_BaseAnimSpeed());
            pCineActor->Get_CineActor()->fTime = static_cast<_float>(fAnimTime);
        }

        ImGui::NewLine();

        ImGui::Text("LerpType");
        _int iLerpType = static_cast<_int>(pCineActor->Get_CineActor()->eLerpType);
        if (ImGui::RadioButton("Lerp_Linear", iLerpType == 0))
            pCineActor->Set_LerpType(0);
        if (ImGui::RadioButton("Lerp_Accel", iLerpType == 1))
            pCineActor->Set_LerpType(1);
        if (ImGui::RadioButton("Lerp_Decel", iLerpType == 2))
            pCineActor->Set_LerpType(2);
        if (ImGui::RadioButton("Lerp_Sine", iLerpType == 3))
            pCineActor->Set_LerpType(3);
        if (ImGui::RadioButton("Lerp_ArcSine", iLerpType == 4))
            pCineActor->Set_LerpType(4);

        ImGui::NewLine();

        _float fLerp = pCineActor->Get_CineActor()->fLerp;
        if (ImGui::DragFloat("Lerp", &fLerp, 0.01f, 0.f, 1.f))
            pCineActor->Get_CineActor()->fLerp = fLerp;

        ImGui::NewLine();

        if (iPathType == static_cast<_uint>(PATHTYPE::PATHTYPE_LINEAR))
        {
        }
        else if (iPathType == static_cast<_uint>(PATHTYPE::PATHTYPE_BAZIER))
        {
            _vec3 vPos = pCineActor->Get_CineActor()->vBazierControl;
            _float fPos[3] = { vPos.x, vPos.y, vPos.z };
            if (ImGui::DragFloat3("BazierControl", fPos, 0.01f))
            {
                vPos = XMVectorSet(fPos[0], fPos[1], fPos[2], 1.f);
                pCineActor->Set_BazierPos(vPos._vector());
            }
        }
        else if (iPathType == static_cast<_uint>(PATHTYPE::PATHTYPE_CATMULLROM))
        {
            _vec3 vStartPos = pCineActor->Get_CineActor()->vCatmullRomControlStart;
            _float fStartPos[3] = { vStartPos.x, vStartPos.y, vStartPos.z };
            if (ImGui::DragFloat3("CatmullRom1", fStartPos, 0.01f))
            {
                vStartPos = XMVectorSet(fStartPos[0], fStartPos[1], fStartPos[2], 1.f);
                pCineActor->Set_CatmullRomStart(vStartPos._vector());
            }

            _vec3 vEndPos = pCineActor->Get_CineActor()->vCatmullRomControlEnd;
            _float fEndPos[3] = { vEndPos.x, vEndPos.y, vEndPos.z };
            if (ImGui::DragFloat3("CatmullRom2", fEndPos, 0.01f))
            {
                vEndPos = XMVectorSet(fEndPos[0], fEndPos[1], fEndPos[2], 1.f);
                pCineActor->Set_CatmullRomEnd(vEndPos._vector());
            }
        }

        if (ImGui::Button("Edit Complete"))
            m_pCineCamera->Arrange_CineActorList();

        // Delete
        if (ImGui::Button("Delete Camera"))
        {
            m_pCineCamera->Delete_CineActor(strCameraName.c_str());
            iCineActorIndex = -1;
            m_iPrevCineActorIndex = -1;
        }
    }

    ImGui::End();
}

void CImgui_Manager::ImGui_CameraPlaying()
{
    // Camera PlayBar
    {
        ImGui::Text("Camera");

        _float fDuration = m_pCineCamera->Get_Duration();
        _float fCurrTime = m_pCineCamera->Get_CurrTime();

        // Camera Duration Slider
        ImGui::SetNextItemWidth(500);
        if (ImGui::SliderFloat("##CameraDuration", &fCurrTime, 0.0f, fDuration, "%.2f"))
        {
            m_pCineCamera->Stop();
            m_pCineCamera->Set_CurrTime(fCurrTime);

            //m_pCurrAnimation->Set_Active(false);

            //_double fTrackPosition = static_cast<_double>(m_pCurrAnimation->Get_TickPerSecond() * m_pCurrAnimation->Get_BaseAnimSpeed() * fCurrTime);
            //m_pAnimObject->Set_Animation_TrackPosition(fTrackPosition);
        }

        // TrackPosition : Duration
        ImGui::SameLine();
        ImGui::Text("%.2f : %.2f", fCurrTime, fDuration);
    }
}

HRESULT CImgui_Manager::Save_CameraData(string _strFile)
{
    _wstring strFile = CUtils::ConvertToWideChar(_strFile);

    _ulong dwByte = 0;
    HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed To Save CameraData");
        return E_FAIL;
    }

    m_pCineCamera->Save(hFile);

    CloseHandle(hFile);

    MSG_BOX("CameraData Save Success");

    return S_OK;
}

HRESULT CImgui_Manager::Load_CameraData(string _strFile)
{
    _wstring strFile = CUtils::ConvertToWideChar(_strFile);

    _bool bResult{};
    _ulong dwByte{};
    HANDLE hFile{};
    hFile = CreateFile(strFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed To Load CameraData");
        return E_FAIL;
    }

    m_pCineCamera->Load(hFile);

    CloseHandle(hFile);

    MSG_BOX("CameraData Load Success");

    return S_OK;
}

HRESULT CImgui_Manager::ImGuiDialogue_Camera()
{
    if (ImGui::Button("Save"))
    {
        IGFD::FileDialogConfig config;
        config.path = "../Bin/DataFiles/CameraData";

        ImGuiFileDialog::Instance()->OpenDialog("Save File", "Choose File", ".data", config);
    }

    if (ImGuiFileDialog::Instance()->Display("Save File"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            if (FAILED(Save_CameraData(filePathName)))
                return E_FAIL;
        }

        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::SameLine();

    if (ImGui::Button("Load"))
    {
        IGFD::FileDialogConfig config;
        config.path = "../Bin/DataFiles/CameraData";

        ImGuiFileDialog::Instance()->OpenDialog("Load File", "Choose File", ".data", config);
    }

    if (ImGuiFileDialog::Instance()->Display("Load File"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            if (FAILED(Load_CameraData(filePathName)))
                return E_FAIL;
        }

        ImGuiFileDialog::Instance()->Close();
    }

    return S_OK;
}

void CImgui_Manager::Free()
{
    __super::Free();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}