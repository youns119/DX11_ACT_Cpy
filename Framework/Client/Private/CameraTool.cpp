#include "pch.h"
#include "CameraTool.h"

#ifdef _DEBUG

#include "GameInstance.h"

#include "Camera_Free.h"
#include "CineCameraTool.h"
#include "CineActorTool.h"

CCameraTool::CCameraTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CCameraTool::CCameraTool(const CCameraTool& Prototype)
	: super(Prototype)
{
}

HRESULT CCameraTool::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCameraTool::Initialize(void* pArg)
{
    if (FAILED(super::Initialize(pArg)) ||
        FAILED(Ready_PartObject()))
        return E_FAIL;

	return S_OK;
}

void CCameraTool::Priority_Update(_float fTimeDelta)
{
    super::Priority_Update(fTimeDelta);
}

void CCameraTool::Update(_float fTimeDelta)
{
    if (m_pToolCamera == nullptr)
        m_pToolCamera = static_cast<CCamera_Free*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Camera"));
    else
    {
        ImGui_CameraTool();
        if (m_bImGui)
        {
            ImGui_CameraList();
            ImGui_CameraPlaying();
            ImGui_Notify();
        }
    }

    super::Update(fTimeDelta);
}

void CCameraTool::Late_Update(_float fTimeDelta)
{
    super::Late_Update(fTimeDelta);
}

HRESULT CCameraTool::Render()
{
	return S_OK;
}

void CCameraTool::ImGui_CameraTool()
{
    ImGui::Begin("CameraTool");

    if (ImGui::Button("Activate"))
        m_bImGui = !m_bImGui;

    if (!m_bImGui)
    {
        ImGui::End();
        return;
    }

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

    if (CGameInstance::GetInstance()->Mouse_Down(MOUSEKEYSTATE::DIM_RB))
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
        m_pGameInstance->Camera_Transition(L"Camera_Free", (_uint)CCameraManager::LERP::LERP_LINEAR, 0.5f);

    ImGuiDialogue_Camera();

    ImGui::End();
}

void CCameraTool::ImGui_CameraList()
{
    ImGui::Begin("CameraList");

    list<CCineActorTool*>* listCineActor = m_pCineCamera->Get_CineActorList();
    vector<CCineActorTool*> vecCineActor;
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
    static CCineActorTool* pCineActor{};

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

void CCameraTool::ImGui_CameraPlaying()
{
    // Camera PlayBar
    {
        ImGui::Text("Camera");

        _float fDuration = m_pCineCamera->Get_Duration();
        _float fCurrTime = m_pCineCamera->Get_CurrTime();

        // Camera Duration Slider
        ImGui::SetNextItemWidth(650);
        if (ImGui::SliderFloat("##CameraDuration", &fCurrTime, 0.0f, fDuration, "%.2f"))
        {
            m_pCineCamera->Stop();
            m_pCineCamera->Set_CurrTime(fCurrTime);
        }

        // TrackPosition : Duration
        ImGui::SameLine();
        ImGui::Text("%.2f : %.2f", fCurrTime, fDuration);

        ImGui::SetCursorPos(ImVec2(350, 120));
        if (ImGui::Button("Play/Stop"))
            m_pCineCamera->Play(m_bCineCamera);
    }
}

void CCameraTool::ImGui_Notify()
{
    ImGui::Begin("Notify");

    // Notify
    {
        ImGui::Text("Notify");

        _float fTrackPosition = m_pCineCamera->Get_CurrTime();

        queue<pair<_float, string>> queTemp{};
        queTemp = m_pCineCamera->Get_CamNotify();

        vector<pair<_float, string>> vecNotify;
        vector<string> vecCamNotifyName;
        vector<const _char*> vecNotifyName;

        static string strName{};

        // Notify 이름 입력 Text 박스
        static _char szBuffer[MAX_PATH]{};
        ImGui::Text("Notify Name");
        if (ImGui::InputText("##Notify Name", szBuffer, IM_ARRAYSIZE(szBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            string strNotifyName = string(szBuffer);
            m_pCineCamera->Add_Notify(fTrackPosition, strNotifyName);

            m_bTyping = false;
            m_iNotifyIndex = 0;

            strName = strNotifyName;
            m_fNotifyTrackPosition = fTrackPosition;
        }
        if (ImGui::IsItemActive())
            m_bTyping = true;
        else m_bTyping = false;

        m_pToolCamera->Set_Typing(m_bTyping);

        while (!queTemp.empty())
        {
            pair<_float, string> pairTemp = queTemp.front();
            queTemp.pop();

            vecNotify.push_back(pairTemp);
        }

        for (auto& Notify : vecNotify)
            vecCamNotifyName.push_back(Notify.second);

        for (auto& NotifyName : vecCamNotifyName)
            vecNotifyName.push_back(NotifyName.c_str());

        // Notify ListBox
        ImGui::Text("Notify List");
        static _float fCurrPosition{};
        static _char szBufferTemp[MAX_PATH];
        if (ImGui::ListBox("##Notify List", &m_iNotifyIndex, vecNotifyName.data(), static_cast<_int>(vecNotifyName.size())))
        {
            fCurrPosition = m_fNotifyTrackPosition = vecNotify[m_iNotifyIndex].first;
            strName = vecNotify[m_iNotifyIndex].second;
            strcpy_s(szBufferTemp, strName.c_str());

            m_pCineCamera->Set_CurrTime(fCurrPosition);
        }

        // Notify의 TrackPosition, Name Text
        if (m_iNotifyIndex >= 0)
        {
            ImGui::NewLine();
            ImGui::Text("Notify Name : %s", strName.c_str());
            ImGui::Text("Notify TrackPosition : %.2lf", fCurrPosition);

            _float fMinValue{};

            // Notify 이름 입력 Text 박스
            _bool bTyping{};
            static _string strTemp{};
            if (ImGui::InputText("##Notify Edit Name", szBufferTemp, IM_ARRAYSIZE(szBufferTemp)))
            {
                strTemp = string(szBufferTemp);

                bTyping = false;
            }
            if (ImGui::IsItemActive()) bTyping = true;
            else bTyping = false;

            if (m_bTyping || bTyping)
            {
                m_bTyping = true;
                m_pToolCamera->Set_Typing(m_bTyping);
            }
            else 
            {
                m_bTyping = false;
                m_pToolCamera->Set_Typing(m_bTyping);
            }

            _float fDuration = m_pCineCamera->Get_Duration();
            ImGui::DragFloat("##Notify TrackPosition", &m_fNotifyTrackPosition, 0.01f, 0.f, fDuration);

            if (ImGui::Button("Notify Edit"))
            {
                m_pCineCamera->Change_Notify(strName, strTemp, fCurrPosition, m_fNotifyTrackPosition);
                strName = strTemp;
                m_iNotifyIndex = -1;
            }

            if (ImGui::Button("Notify Delete"))
            {
                m_pCineCamera->Delete_Notify(strName, m_fNotifyTrackPosition);
                m_iNotifyIndex = -1;
            }
        }
    }

    ImGui::End();
}

HRESULT CCameraTool::Save_CameraData(string _strFile)
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

HRESULT CCameraTool::Load_CameraData(string _strFile)
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

HRESULT CCameraTool::ImGuiDialogue_Camera()
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

HRESULT CCameraTool::Ready_PartObject()
{
    // CineCamera
    {
        CCineCameraTool::DESC tCineCameraDesc{};
        tCineCameraDesc.CameraDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();

        if (FAILED(Add_PartObject(L"CineCamera", LEVEL_STATIC, L"Prototype_GameObject_CineCameraTool", &tCineCameraDesc)))
            return E_FAIL;

        m_pCineCamera = static_cast<CCineCameraTool*>(Find_PartObject({ L"CineCamera" }));
    }

    return S_OK;
}

CCameraTool* CCameraTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCameraTool* pInstance = new CCameraTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CCameraTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCameraTool::Clone(void* pArg)
{
	CCameraTool* pInstance = new CCameraTool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCameraTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCameraTool::Free()
{
	super::Free();
}

#endif // _DEBUG