#include "Animation.h"

#include "GameObject.h"
#include "Channel.h"
#include "Model.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& Prototype)
    : m_Channels{ Prototype.m_Channels }
    , m_CurrentKeyFrameIndices{ Prototype.m_CurrentKeyFrameIndices }
    , m_Duration{ Prototype.m_Duration }
    , m_TicksPerSecond{ Prototype.m_TicksPerSecond }
    , m_eRootMotion{ Prototype.m_eRootMotion }
    , m_queAnimNotify{ Prototype.m_queAnimNotify }
    , m_fBaseLerpTime{ Prototype.m_fBaseLerpTime }
    , m_fBaseAnimSpeed{ Prototype.m_fBaseAnimSpeed }
    , m_fBlendOutTime{ Prototype.m_fBlendOutTime }
    , m_fBlendInTime{ Prototype.m_fBlendInTime }
{
    strcpy_s(m_szAnimPath, Prototype.m_szAnimPath);
    strcpy_s(m_szName, Prototype.m_szName);

    for (auto& pChannel : m_Channels)
        Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const CModel* pModel)
{
    m_Duration = static_cast<KEYFRAME::_scaleUnit>(pAIAnimation->mDuration);
    m_TicksPerSecond = static_cast<KEYFRAME::_scaleUnit>(pAIAnimation->mTicksPerSecond);

    strcpy_s(m_szName, pAIAnimation->mName.data);
    string strAnimName = string(m_szName);
    CUtils::ValidateFileName(strAnimName);
    strcpy_s(m_szName, strAnimName.c_str());

    m_fBlendOutTime = static_cast<_float>(m_Duration);

    /* 이 애니메이션을 구동하는데 있어 필요한 뼈의 갯수 */
    _uint iNumChannels = pAIAnimation->mNumChannels;

    m_Channels.reserve(iNumChannels);
    for (size_t i = 0; i < iNumChannels; ++i)
    {
        CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel);
        if (nullptr == pChannel)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }

    m_CurrentKeyFrameIndices.resize(iNumChannels);

    return S_OK;
}

HRESULT CAnimation::Initialize_From_File(const HANDLE& hFile, const CModel* pModel, _wstring strAnimName)
{
    assert(INVALID_HANDLE_VALUE != hFile);

    // m_szName
    {
        const _tchar* szAnim = strAnimName.c_str();
        _tchar szAnimName[MAX_PATH]{};

        _wsplitpath_s(szAnim, nullptr, 0, nullptr, 0, szAnimName, MAX_PATH, nullptr, 0);

        _int iSize = WideCharToMultiByte(CP_UTF8, 0, szAnim, -1, NULL, 0, NULL, NULL);
        WideCharToMultiByte(CP_UTF8, 0, szAnim, -1, m_szAnimPath, iSize, NULL, NULL);

        iSize = WideCharToMultiByte(CP_UTF8, 0, szAnimName, -1, NULL, 0, NULL, NULL);
        WideCharToMultiByte(CP_UTF8, 0, szAnimName, -1, m_szName, iSize, NULL, NULL);
    }

    _ulong dwByte = 0;
    _bool bResult = FALSE;

    _double dDuration = 0;
    _double dTicksPerSecond = 0;

    bResult = ReadFile(hFile, &dDuration, sizeof(_double), &dwByte, nullptr);
    bResult = ReadFile(hFile, &dTicksPerSecond, sizeof(_double), &dwByte, nullptr);

    bResult = ReadFile(hFile, &m_fBaseLerpTime, sizeof(_float), &dwByte, nullptr);
    bResult = ReadFile(hFile, &m_fBaseAnimSpeed, sizeof(_float), &dwByte, nullptr);
    bResult = ReadFile(hFile, &m_fBlendInTime, sizeof(_float), &dwByte, nullptr);
    bResult = ReadFile(hFile, &m_fBlendOutTime, sizeof(_float), &dwByte, nullptr);

    _uint iRootMotion{};
    bResult = ReadFile(hFile, &iRootMotion, sizeof(_uint), &dwByte, nullptr);
    m_eRootMotion = static_cast<ROOTMOTION>(iRootMotion);

    _uint iAnimNotifySize{};
    bResult = ReadFile(hFile, &iAnimNotifySize, sizeof(_uint), &dwByte, nullptr);

    for (_uint i = 0; i < iAnimNotifySize; i++)
    {
        _double fTrackPosition{};
        _char szNotifyName[MAX_PATH]{};

        bResult = ReadFile(hFile, &fTrackPosition, sizeof(_double), &dwByte, nullptr);
        bResult = ReadFile(hFile, &szNotifyName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);

        string strNotify = string(szNotifyName);

        Add_AnimNotify(fTrackPosition, strNotify);
    }

    m_Duration = (KEYFRAME::_scaleUnit)dDuration;
    m_TicksPerSecond = (KEYFRAME::_scaleUnit)dTicksPerSecond;

    _uint iNumChannels = 0;
    bResult = ReadFile(hFile, &iNumChannels, sizeof(_uint), &dwByte, nullptr);
    m_Channels.reserve(iNumChannels);

    for (_uint i = 0; i < iNumChannels; ++i)
    {
        CChannel* pChannel = CChannel::Create_From_File(hFile, pModel);
        if (nullptr == pChannel)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }

    m_CurrentKeyFrameIndices.resize(iNumChannels);

    return S_OK;
}

_bool CAnimation::Update_TransformationMatrix(_float fTimeDelta, const vector<CBone*> Bones, _bool isLoop, _float fAnimSpeed, CModel* pModel)
{
    if (!m_bActive)
        return false;

    m_PrevTrackPosition = m_CurrentTrackPosition;
    m_CurrentTrackPosition += m_TicksPerSecond * fTimeDelta * m_fBaseAnimSpeed * fAnimSpeed;

    if (isLoop) m_bBlendable = true;
    else
    {
        if (m_fBlendOutTime <= m_CurrentTrackPosition) m_bBlendable = true;
        else m_bBlendable = false;
    }

    Call_AnimNotify(pModel);

    _bool isFinished{};
    if (m_CurrentTrackPosition >= m_Duration)
    {
        isFinished = true;
        m_bPlaying = false;
    }

    if (isFinished)
    {
        if (isLoop)
            m_CurrentTrackPosition = m_fBlendInTime;
        else return TRUE;
    }

    for (size_t i = 0; i < m_Channels.size(); ++i)
        m_Channels[i]->Update_TransformationMatrix(m_CurrentTrackPosition, m_CurrentKeyFrameIndices[i], Bones, pModel);

    return FALSE;
}

_bool CAnimation::Update_TransformationMatrix_While_Transition(_float fTimeDelta, _float fLerpTime, const vector<CBone*> Bones, const vector<KEYFRAME>& SourKeyFrames, CModel* pModel)
{
    m_fLerpTime += fTimeDelta; // transition 상태에서는 TicksPerSecond <- 1.f 로 설정하겠다. (계산의 편의)
    // transition 이 시작할때 m_CurrentTrackPosition 이 항상 0임을 보장받ㅈ지 못한다. 하다 중간에 넘어다면
    _float fRatio = static_cast<_float>(m_fLerpTime / fLerpTime);
    if (fRatio > 1.f)
    {
        Reset();
        return TRUE;
    }

    for (size_t i = 0; i < m_Channels.size(); ++i)
        m_Channels[i]->Update_TransformationMatrix_While_Transition(fRatio, m_fBlendInTime, m_CurrentKeyFrameIndices[i], Bones, SourKeyFrames[i], pModel);

    return FALSE;
}

void CAnimation::Update_TransformationMatrix_Specific(_double fTrackPosition, const vector<CBone*> Bones, CModel* pModel)
{
    m_CurrentTrackPosition = fTrackPosition;

    for (size_t i = 0; i < m_Channels.size(); ++i)
        m_Channels[i]->Update_TransformationMatrix(m_CurrentTrackPosition, m_CurrentKeyFrameIndices[i], Bones, pModel);
}

HRESULT CAnimation::Save(const _char* szFilePath)
{
    _double dDuration = (_double)m_Duration;
    _double dTicksPerSecond = (_double)m_TicksPerSecond;

    _char szName[MAX_PATH]{};
    strcpy_s(szName, m_szName);

    // Animation 파일 세이브
    {
        _char szPath[MAX_PATH]{};
        strcat_s(szPath, szFilePath);
        strcat_s(szPath, "/");
        strcat_s(szPath, szName);
        strcat_s(szPath, ".ANIM");

        string strPath = string(szPath);
        _wstring strFilePath = CUtils::ConvertToWideChar(strPath);

        _ulong dwByte = 0;
        HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

        if (hFile == INVALID_HANDLE_VALUE)
        {
            MSG_BOX("Can Not Open File : CModel");
            return E_FAIL;
        }

        WriteFile(hFile, &dDuration, sizeof(_double), &dwByte, nullptr);
        WriteFile(hFile, &dTicksPerSecond, sizeof(_double), &dwByte, nullptr);

        WriteFile(hFile, &m_fBaseLerpTime, sizeof(_float), &dwByte, nullptr);
        WriteFile(hFile, &m_fBaseAnimSpeed, sizeof(_float), &dwByte, nullptr);
        WriteFile(hFile, &m_fBlendInTime, sizeof(_float), &dwByte, nullptr);
        WriteFile(hFile, &m_fBlendOutTime, sizeof(_float), &dwByte, nullptr);

        _uint iRootMotion = static_cast<_uint>(m_eRootMotion);
        WriteFile(hFile, &iRootMotion, sizeof(_uint), &dwByte, nullptr);

        _uint iAnimNotifySize = static_cast<_uint>(m_queAnimNotify.size());
        WriteFile(hFile, &iAnimNotifySize, sizeof(_uint), &dwByte, nullptr);

        while (!m_queAnimNotify.empty())
        {
            pair<_double, string> pairTemp = m_queAnimNotify.front();
            m_queAnimNotify.pop();

            _double fTrackPosition = pairTemp.first;
            const _char* szNotifyName = pairTemp.second.c_str();

            _char szNotify[MAX_PATH]{};
            strcpy_s(szNotify, szNotifyName);

            WriteFile(hFile, &fTrackPosition, sizeof(_double), &dwByte, nullptr);
            WriteFile(hFile, &szNotify, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
        }

        _uint iNumChannels = (_uint)m_Channels.size();
        WriteFile(hFile, &iNumChannels, sizeof(_uint), &dwByte, nullptr);
        for (auto& pChannel : m_Channels)
        {
            if (FAILED(pChannel->Save(hFile)))
                return E_FAIL;
        }

        CloseHandle(hFile);
    }

    // AnimData 세이브
    if (FAILED(Save_AnimData(szFilePath)))
        return E_FAIL;

    return S_OK;
}

HRESULT CAnimation::Save()
{
    _double dDuration = (_double)m_Duration;
    _double dTicksPerSecond = (_double)m_TicksPerSecond;

    // Animation 세이브
    {
        string strPath = string(m_szAnimPath);
        _wstring strFilePath = CUtils::ConvertToWideChar(strPath);

        _ulong dwByte = 0;
        HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

        if (hFile == INVALID_HANDLE_VALUE)
        {
            MSG_BOX("Can Not Save Animation");
            return E_FAIL;
        }

        WriteFile(hFile, &dDuration, sizeof(_double), &dwByte, nullptr);
        WriteFile(hFile, &dTicksPerSecond, sizeof(_double), &dwByte, nullptr);

        WriteFile(hFile, &m_fBaseLerpTime, sizeof(_float), &dwByte, nullptr);
        WriteFile(hFile, &m_fBaseAnimSpeed, sizeof(_float), &dwByte, nullptr);
        WriteFile(hFile, &m_fBlendInTime, sizeof(_float), &dwByte, nullptr);
        WriteFile(hFile, &m_fBlendOutTime, sizeof(_float), &dwByte, nullptr);

        _uint iRootMotion = static_cast<_uint>(m_eRootMotion);
        WriteFile(hFile, &iRootMotion, sizeof(_uint), &dwByte, nullptr);

        _uint iAnimNotifySize = static_cast<_uint>(m_queAnimNotify.size());
        WriteFile(hFile, &iAnimNotifySize, sizeof(_uint), &dwByte, nullptr);

        queue<pair<_double, string>> queTemp = m_queAnimNotify;

        while (!queTemp.empty())
        {
            pair<_double, string> pairTemp = queTemp.front();
            queTemp.pop();

            _double fTrackPosition = pairTemp.first;
            const _char* szNotifyName = pairTemp.second.c_str();

            _char szNotify[MAX_PATH]{};
            strcpy_s(szNotify, szNotifyName);

            WriteFile(hFile, &fTrackPosition, sizeof(_double), &dwByte, nullptr);
            WriteFile(hFile, &szNotify, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
        }

        _uint iNumChannels = (_uint)m_Channels.size();
        WriteFile(hFile, &iNumChannels, sizeof(_uint), &dwByte, nullptr);
        for (auto& pChannel : m_Channels)
        {
            if (FAILED(pChannel->Save(hFile)))
                return E_FAIL;
        }

        CloseHandle(hFile);
    }

    // AnimData 세이브
    if (FAILED(Save_AnimData()))
        return E_FAIL;

    return S_OK;
}

HRESULT CAnimation::Save_AnimData(const _char* szFilePath)
{
    _double dDuration = (_double)m_Duration;
    _double dTicksPerSecond = (_double)m_TicksPerSecond;

    _char szName[MAX_PATH]{};
    strcpy_s(szName, m_szName);

    _char szDataPath[MAX_PATH]{};
    strcat_s(szDataPath, szFilePath);
    strcat_s(szDataPath, "/AnimData");

    _string strDataPath = _string(szDataPath);
    if (!CUtils::CreateDirectories(strDataPath))
        return E_FAIL;

    strcat_s(szDataPath, "/");
    strcat_s(szDataPath, szName);
    strcat_s(szDataPath, ".AnimData");

    string strPath = string(szDataPath);
    _wstring strFilePath = CUtils::ConvertToWideChar(strPath);

    _ulong dwByte = 0;
    HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    WriteFile(hFile, &dDuration, sizeof(_double), &dwByte, nullptr);
    WriteFile(hFile, &dTicksPerSecond, sizeof(_double), &dwByte, nullptr);

    WriteFile(hFile, &m_fBaseLerpTime, sizeof(_float), &dwByte, nullptr);
    WriteFile(hFile, &m_fBaseAnimSpeed, sizeof(_float), &dwByte, nullptr);
    WriteFile(hFile, &m_fBlendInTime, sizeof(_float), &dwByte, nullptr);
    WriteFile(hFile, &m_fBlendOutTime, sizeof(_float), &dwByte, nullptr);

    _uint iRootMotion = static_cast<_uint>(m_eRootMotion);
    WriteFile(hFile, &iRootMotion, sizeof(_uint), &dwByte, nullptr);

    _uint iAnimNotifySize = static_cast<_uint>(m_queAnimNotify.size());
    WriteFile(hFile, &iAnimNotifySize, sizeof(_uint), &dwByte, nullptr);

    while (!m_queAnimNotify.empty())
    {
        pair<_double, string> pairTemp = m_queAnimNotify.front();
        m_queAnimNotify.pop();

        _double fTrackPosition = pairTemp.first;
        const _char* szNotifyName = pairTemp.second.c_str();

        _char szNotify[MAX_PATH]{};
        strcpy_s(szNotify, szNotifyName);

        WriteFile(hFile, &fTrackPosition, sizeof(_double), &dwByte, nullptr);
        WriteFile(hFile, &szNotify, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
    }

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CAnimation::Save_AnimData()
{
    _double dDuration = (_double)m_Duration;
    _double dTicksPerSecond = (_double)m_TicksPerSecond;

    _char szPath[MAX_PATH]{};

    _splitpath_s(m_szAnimPath, nullptr, 0, szPath, MAX_PATH, nullptr, 0, nullptr, 0);

    strcat_s(szPath, "AnimData/");
    strcat_s(szPath, m_szName);
    strcat_s(szPath, ".AnimData");

    string strPath = string(szPath);
    _wstring strFilePath = CUtils::ConvertToWideChar(strPath);

    _ulong dwByte = 0;
    HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    WriteFile(hFile, &dDuration, sizeof(_double), &dwByte, nullptr);
    WriteFile(hFile, &dTicksPerSecond, sizeof(_double), &dwByte, nullptr);

    WriteFile(hFile, &m_fBaseLerpTime, sizeof(_float), &dwByte, nullptr);
    WriteFile(hFile, &m_fBaseAnimSpeed, sizeof(_float), &dwByte, nullptr);
    WriteFile(hFile, &m_fBlendInTime, sizeof(_float), &dwByte, nullptr);
    WriteFile(hFile, &m_fBlendOutTime, sizeof(_float), &dwByte, nullptr);

    _uint iRootMotion = static_cast<_uint>(m_eRootMotion);
    WriteFile(hFile, &iRootMotion, sizeof(_uint), &dwByte, nullptr);

    _uint iAnimNotifySize = static_cast<_uint>(m_queAnimNotify.size());
    WriteFile(hFile, &iAnimNotifySize, sizeof(_uint), &dwByte, nullptr);

    queue<pair<_double, string>> queTemp = m_queAnimNotify;

    while (!queTemp.empty())
    {
        pair<_double, string> pairTemp = queTemp.front();
        queTemp.pop();

        _double fTrackPosition = pairTemp.first;
        const _char* szNotifyName = pairTemp.second.c_str();

        _char szNotify[MAX_PATH]{};
        strcpy_s(szNotify, szNotifyName);

        WriteFile(hFile, &fTrackPosition, sizeof(_double), &dwByte, nullptr);
        WriteFile(hFile, &szNotify, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
    }

    return S_OK;
}

HRESULT CAnimation::Load_AnimData(const HANDLE& hFile)
{
    _ulong dwByte = 0;
    _bool bResult = FALSE;

    _double dDuration = 0;
    _double dTicksPerSecond = 0;

    bResult = ReadFile(hFile, &dDuration, sizeof(_double), &dwByte, nullptr);
    bResult = ReadFile(hFile, &dTicksPerSecond, sizeof(_double), &dwByte, nullptr);

    bResult = ReadFile(hFile, &m_fBaseLerpTime, sizeof(_float), &dwByte, nullptr);
    bResult = ReadFile(hFile, &m_fBaseAnimSpeed, sizeof(_float), &dwByte, nullptr);
    bResult = ReadFile(hFile, &m_fBlendInTime, sizeof(_float), &dwByte, nullptr);
    bResult = ReadFile(hFile, &m_fBlendOutTime, sizeof(_float), &dwByte, nullptr);

    _uint iRootMotion{};
    bResult = ReadFile(hFile, &iRootMotion, sizeof(_uint), &dwByte, nullptr);
    m_eRootMotion = static_cast<ROOTMOTION>(iRootMotion);

    _uint iAnimNotifySize{};
    bResult = ReadFile(hFile, &iAnimNotifySize, sizeof(_uint), &dwByte, nullptr);

    while (!m_queAnimNotify.empty())
        m_queAnimNotify.pop();

    for (_uint i = 0; i < iAnimNotifySize; i++)
    {
        _double fTrackPosition{};
        _char szNotifyName[MAX_PATH]{};

        bResult = ReadFile(hFile, &fTrackPosition, sizeof(_double), &dwByte, nullptr);
        bResult = ReadFile(hFile, &szNotifyName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);

        string strNotify = string(szNotifyName);

        Add_AnimNotify(fTrackPosition, strNotify);
    }

    m_Duration = (KEYFRAME::_scaleUnit)dDuration;
    m_TicksPerSecond = (KEYFRAME::_scaleUnit)dTicksPerSecond;

    return S_OK;
}

vector<KEYFRAME> CAnimation::Get_CurrentChannelKeyFrames(const vector<CBone*>& Bones) const
{
    vector<KEYFRAME> CurrentChannelKeyFramse;
    CurrentChannelKeyFramse.reserve(m_Channels.size());

    for (auto& pChannel : m_Channels)
        CurrentChannelKeyFramse.push_back(pChannel->Get_CurrentKeyFrame(Bones));

    return CurrentChannelKeyFramse;
}

void CAnimation::Reset()
{
    m_CurrentTrackPosition = m_fBlendInTime;
    m_fLerpTime = 0.f;
    m_bBlendable = false;
    for (auto& iIndex : m_CurrentKeyFrameIndices)
        iIndex = 0;
}

void CAnimation::Add_AnimNotify(_double fTrackPosition, string& strNotifyName)
{
    m_queAnimNotify.push(make_pair(fTrackPosition, strNotifyName));

    Arrange_AnimNotify();
}

void CAnimation::Change_AnimNotify
(string& strNotifyName, string& strChangeName, _double fTrackPosition, _double fChangePosition)
{
    queue<pair<_double, string>> queTemp = m_queAnimNotify;

    while (!m_queAnimNotify.empty())
        m_queAnimNotify.pop();

    while (!queTemp.empty())
    {
        pair<_double, string> pairTemp = queTemp.front();
        queTemp.pop();

        if (pairTemp.first == fTrackPosition && pairTemp.second == strNotifyName) 
            Add_AnimNotify(fChangePosition, strChangeName);
        else Add_AnimNotify(pairTemp.first, pairTemp.second);
    }
}

void CAnimation::Delete_AnimNotify(string& strNotifyName, _double fTrackPosition)
{
    queue<pair<_double, string>> queTemp;

    while (!m_queAnimNotify.empty())
    {
        pair<_double, string> pairTemp = m_queAnimNotify.front();
        m_queAnimNotify.pop();

        if (pairTemp.first != fTrackPosition || pairTemp.second != strNotifyName)
            queTemp.push(pairTemp);
    }

    m_queAnimNotify = queTemp;
}

void CAnimation::Arrange_AnimNotify()
{
    vector<pair<_double, string>> vecTemp;

    while (!m_queAnimNotify.empty())
    {
        pair<_double, string> pairTemp = m_queAnimNotify.front();
        m_queAnimNotify.pop();

        vecTemp.push_back(pairTemp);
    }

    sort(vecTemp.begin(), vecTemp.end(), [](const auto& PrevPair, const auto& NextPair)
        {
            return PrevPair.first < NextPair.first;
        });

    for (auto& Pair : vecTemp)
        m_queAnimNotify.push(Pair);
}

void CAnimation::Call_AnimNotify(CModel* pModel)
{
    for (size_t i = 0; i < m_queAnimNotify.size(); i++)
    {
        pair<_double, string> pairFront = m_queAnimNotify.front();

        if (pairFront.first >= m_PrevTrackPosition &&
            pairFront.first <= m_CurrentTrackPosition)
        {
            const _char* szNotifyName = pairFront.second.c_str();

            pModel->Get_Owner()->Call_Event(szNotifyName);

            m_queAnimNotify.pop();
            m_queAnimNotify.push(pairFront);
        }
        else break;
    }
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const CModel* pModel)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Create : CAnimation");
    }

    return pInstance;
}

CAnimation* CAnimation::Create_From_File(const HANDLE& hFile, const CModel* pModel, _wstring strAnimName)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Initialize_From_File(hFile, pModel, strAnimName)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Create : CAnimation");
    }

    return pInstance;
}

CAnimation* CAnimation::Clone() const
{
    return new CAnimation(*this);
}

void CAnimation::Free()
{
    super::Free();

    for (auto& pChannel : m_Channels)
        Safe_Release(pChannel);
    m_Channels.clear();
}