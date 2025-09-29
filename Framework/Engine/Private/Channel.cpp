#include "Channel.h"

#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const CModel* pModel)
{
    strcpy_s(m_szName, pAIChannel->mNodeName.C_Str());

    m_iBoneIndex = pModel->Get_BoneIndex_By_Name(m_szName);

    _uint iNumKeyFrames = max(max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys), pAIChannel->mNumPositionKeys);
    m_KeyFrames.reserve(iNumKeyFrames);

    _float3 vScale{};
    _float4 vRotation{};
    _float3 vTranslation{};

    for (size_t i = 0; i < iNumKeyFrames; ++i)
    {
        KEYFRAME KeyFrame{};

        if (pAIChannel->mNumScalingKeys > i)
        {
            aiVectorKey AIScalingKey = pAIChannel->mScalingKeys[i];
            vScale.x = AIScalingKey.mValue.x;
            vScale.y = AIScalingKey.mValue.y;
            vScale.z = AIScalingKey.mValue.z;

            KeyFrame.dTrackPosition = AIScalingKey.mTime;
        }

        if (pAIChannel->mNumRotationKeys > i)
        {
            aiQuatKey AIRotationKey = pAIChannel->mRotationKeys[i];
            vRotation.x = AIRotationKey.mValue.x;
            vRotation.y = AIRotationKey.mValue.y;
            vRotation.z = AIRotationKey.mValue.z;
            vRotation.w = AIRotationKey.mValue.w;

            KeyFrame.dTrackPosition = AIRotationKey.mTime;
            if (KeyFrame.dTrackPosition != AIRotationKey.mTime)
                assert(!"key frame track position != AIKey.mTime");
        }

        if (pAIChannel->mNumPositionKeys > i)
        {
            aiVectorKey AIPositionKey = pAIChannel->mPositionKeys[i];
            vTranslation.x = AIPositionKey.mValue.x;
            vTranslation.y = AIPositionKey.mValue.y;
            vTranslation.z = AIPositionKey.mValue.z;

            KeyFrame.dTrackPosition = AIPositionKey.mTime;
            if (KeyFrame.dTrackPosition != AIPositionKey.mTime)
                assert(!"key frame track position != AIKey.mTime");
        }

        KeyFrame.vScale = vScale;
        KeyFrame.vRotation = vRotation;
        KeyFrame.vTranslation = vTranslation;

        m_KeyFrames.push_back(KeyFrame);
    }

    return S_OK;
}

HRESULT CChannel::Initialize_From_File(const HANDLE& hFile, const CModel* pModel)
{
    _ulong dwByte = 0;
    _bool bResult = FALSE;

    bResult = ReadFile(hFile, m_szName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
    bResult = ReadFile(hFile, &m_iBoneIndex, sizeof(_uint), &dwByte, nullptr);

    _uint iNumKeyFrames = 0;
    bResult = ReadFile(hFile, &iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);
    m_KeyFrames.resize(iNumKeyFrames);
    bResult = ReadFile(hFile, m_KeyFrames.data(), sizeof(KEYFRAME) * iNumKeyFrames, &dwByte, nullptr);

    return S_OK;
}

void CChannel::Update_TransformationMatrix(KEYFRAME::_scaleUnit fCurrentTrackPosition, _uint& iCurrentKeyFrameIndex, const vector<CBone*>& Bones, CModel* pModel)
{
    _bool bSlotAnim{};

    if (pModel->Is_SlotPlaying())
        bSlotAnim = true;
    else bSlotAnim = false;

    if (bSlotAnim)
    {
        if (pModel->Is_CurrSlotAnim(pModel->Get_CurrPlayingAnimIndex()))
        {
            if (!pModel->Is_SlotBone(m_iBoneIndex))
                return;
        }
    }
    else
    {
        if (pModel->Is_SlotBone_Total(m_iBoneIndex))
            return;
    }

    if (0.f == fCurrentTrackPosition)
        iCurrentKeyFrameIndex = 0;

    _vector vScale{};
    _vector vRotation{};
    _vector vTranslation{};

    const KEYFRAME& LastKeyFrame = m_KeyFrames.back();

    /* 현재 재생위치가 마지막 키프레임의 위치를 넘어가면.: 걍 마지막 키프레임의 상태를 구하면 된다. */
    if (fCurrentTrackPosition >= LastKeyFrame.dTrackPosition)
    {
        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
    }
    /* 선형 보간이 필요한 경우들 */
    else
    {
        while (fCurrentTrackPosition >= m_KeyFrames[iCurrentKeyFrameIndex + 1].dTrackPosition)
            ++iCurrentKeyFrameIndex;

        KEYFRAME& CurrentKeyFrame = m_KeyFrames[iCurrentKeyFrameIndex];
        KEYFRAME& NextKeyFrame = m_KeyFrames[iCurrentKeyFrameIndex + 1];

        _float fRatio = static_cast<_float>((fCurrentTrackPosition - CurrentKeyFrame.dTrackPosition) / (NextKeyFrame.dTrackPosition - CurrentKeyFrame.dTrackPosition));

        vScale = XMVectorLerp(XMLoadFloat3(&CurrentKeyFrame.vScale), XMLoadFloat3(&NextKeyFrame.vScale), fRatio);
        vRotation = XMQuaternionSlerp(XMLoadFloat4(&CurrentKeyFrame.vRotation), XMLoadFloat4(&NextKeyFrame.vRotation), fRatio);
        vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&CurrentKeyFrame.vTranslation), XMLoadFloat3(&NextKeyFrame.vTranslation), fRatio), 1.f);

    }

    _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Update_TransformationMatrix_While_Transition(_float fRatio, _float fBlendInTime, _uint& iCurrentKeyIndex, const vector<CBone*>& Bones, const KEYFRAME& SourKeyFrame, CModel* pModel)
{
    _bool bSlotAnim{};

    if (pModel->Is_SlotPlaying())
        bSlotAnim = true;
    else bSlotAnim = false;

    if (bSlotAnim)
    {
        if (pModel->Is_CurrSlotAnim(pModel->Get_CurrPlayingAnimIndex()))
        {
            if (!pModel->Is_SlotBone(m_iBoneIndex))
                return;
        }
    }
    else
    {
        if (pModel->Is_SlotBone_Total(m_iBoneIndex))
            return;
    }

    _uint iCurrKeyFrameIndex{};
    if (m_KeyFrames.size() <= 1) iCurrKeyFrameIndex = 0;
    else
    {
        while (fBlendInTime >= m_KeyFrames[iCurrKeyFrameIndex + 1].dTrackPosition)
            ++iCurrKeyFrameIndex;
    }

    KEYFRAME& CurrentKeyFrame = m_KeyFrames[iCurrKeyFrameIndex];

    _vector vPrevRotation{};
    _vector vPostRotation{};

    vPrevRotation = XMLoadFloat4(&SourKeyFrame.vRotation);
    vPostRotation = XMLoadFloat4(&CurrentKeyFrame.vRotation);

    _float fDot = _vec4(XMQuaternionDot(vPrevRotation, vPostRotation)).x;
    if (fDot < 0.f) vPostRotation *= -1.f;

    _vector vScale{};
    _vector vRotation{};
    _vector vTranslation{};

    if (_vec3(SourKeyFrame.vScale).Length() == 0.f)
    {
        vScale = XMLoadFloat3(&CurrentKeyFrame.vScale);
        vRotation = vPostRotation;
        vTranslation = XMLoadFloat3(&CurrentKeyFrame.vTranslation);
    }
    else
    {
        vScale = XMVectorLerp(XMLoadFloat3(&SourKeyFrame.vScale), XMLoadFloat3(&CurrentKeyFrame.vScale), fRatio);
        vRotation = XMQuaternionSlerp(vPrevRotation, vPostRotation, fRatio);
        vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&SourKeyFrame.vTranslation), XMLoadFloat3(&CurrentKeyFrame.vTranslation), fRatio), 1.f);
    }

    _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

HRESULT CChannel::Save(HANDLE hFile)
{
    _ulong dwByte = 0;

    WriteFile(hFile, m_szName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
    WriteFile(hFile, &m_iBoneIndex, sizeof(_uint), &dwByte, nullptr);

    _uint iNumKeyFrames = (_uint)m_KeyFrames.size();
    WriteFile(hFile, &iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);
    WriteFile(hFile, m_KeyFrames.data(), sizeof(KEYFRAME) * iNumKeyFrames, &dwByte, nullptr);

    return S_OK;
}

KEYFRAME CChannel::Get_CurrentKeyFrame(const vector<CBone*>& Bones) const
{
    _vector vScale{};
    _vector vRotation{};
    _vector vTranslation{};

    XMMatrixDecompose(&vScale, &vRotation, &vTranslation, Bones[m_iBoneIndex]->Get_TransformationMatrix());

    KEYFRAME Current{};
    XMStoreFloat3(&Current.vScale, vScale);
    XMStoreFloat4(&Current.vRotation, vRotation);
    XMStoreFloat3(&Current.vTranslation, vTranslation);

    return Current;
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, const CModel* pModel)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(pAIChannel, pModel)))
    {
        MSG_BOX("Failed To Create : CChannel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CChannel* CChannel::Create_From_File(const HANDLE& hFile, const CModel* pModel)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize_From_File(hFile, pModel)))
    {
        MSG_BOX("Failed To Create : CChannel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChannel::Free()
{
    super::Free();
}
