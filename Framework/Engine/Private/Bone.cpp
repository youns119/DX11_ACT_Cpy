#include "Bone.h"

#include "GameObject.h"
#include "Transform.h"
#include "Model.h"
#include "Animation.h"

CBone::CBone()
{
}

CBone::CBone(const CBone& Prototype)
    : m_CombinedTransformationMatrix{ Prototype.m_CombinedTransformationMatrix }
    , m_PreCombinedTransformationMatrix{ Prototype.m_PreCombinedTransformationMatrix }
    , m_iParentBoneIndex{ Prototype.m_iParentBoneIndex }
    , m_TransformationMatrix{ Prototype.m_TransformationMatrix }
{
    strcpy_s(m_szName, Prototype.m_szName);

    CTransform::TRANSFORM_DESC Desc{};
    Desc.fRotationPerSec = 1.f;
    Desc.fSpeedPerSec = 1.f;
    m_pTransformCom = static_cast<CTransform*>(Prototype.m_pTransformCom->Clone(&Desc));
}

HRESULT CBone::Initialize(const aiNode* pAINode, _int iParentBoneIndex, const vector<CBone*>& Bones)
{
    strcpy_s(m_szName, pAINode->mName.C_Str());

    memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
    // aiMatrix 에 직접 접근하면 col major 행렬이다. 그래서 우리 왼손 좌표계로 변환 하려고 ..
    XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

    XMStoreFloat4x4(&m_PreCombinedTransformationMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    m_iParentBoneIndex = iParentBoneIndex;


    // combined matrix 미리 만들어두기 왜냐믄,, 애니메이션이 없지만 ai pre process 를 사용하지 않는 놈들을 위해 메쉬 데이터를 좀 더 자세히 조작하기 위해
    if (m_iParentBoneIndex == -1)
        XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    else
        XMStoreFloat4x4(&m_CombinedTransformationMatrix,
            XMLoadFloat4x4(&m_TransformationMatrix) * Bones[m_iParentBoneIndex]->Get_CombindedTransformationMatrix());


    m_pTransformCom = CTransform::Create(nullptr, nullptr); // 일단 이거 이상하고
    CTransform::TRANSFORM_DESC Desc{};
    {
        Desc.fRotationPerSec = 1.f;
        Desc.fSpeedPerSec = 1.f;
    }
    m_pTransformCom->Initialize(&Desc);

    return S_OK;
}

HRESULT CBone::Initialize_From_File(const HANDLE& hFile, const vector<CBone*>& Bones)
{
    assert(!(INVALID_HANDLE_VALUE == hFile));
    _ulong dwByte = 0;
    _bool bResult = FALSE;

    bResult = ReadFile(hFile, m_szName, sizeof(m_szName), &dwByte, nullptr);
    if (sizeof(m_szName) != dwByte)
        return E_FAIL;

    bResult = ReadFile(hFile, &m_iParentBoneIndex, sizeof(m_iParentBoneIndex), &dwByte, nullptr);

    bResult = ReadFile(hFile, &m_TransformationMatrix, sizeof(m_TransformationMatrix), &dwByte, nullptr);

    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    // combined matrix 미리 만들어두기 왜냐믄,, 애니메이션이 없지만 ai pre process 를 사용하지 않는 놈들을 위해 메쉬 데이터를 좀 더 자세히 조작하기 위해
    if (m_iParentBoneIndex == -1)
        XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    else
        XMStoreFloat4x4(&m_CombinedTransformationMatrix,
            XMLoadFloat4x4(&m_TransformationMatrix) * Bones[m_iParentBoneIndex]->Get_CombindedTransformationMatrix());



    m_pTransformCom = CTransform::Create(nullptr, nullptr);
    CTransform::TRANSFORM_DESC Desc{};
    {
        Desc.fRotationPerSec = 1.f;
        Desc.fSpeedPerSec = 1.f;
    }
    m_pTransformCom->Initialize(&Desc);


    return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix, CModel* pModel, CAnimation* pAnimation)
{
    const _char* pRootBone = pModel->Get_RootBoneName();

    _matrix SeperateTransformMatrix = m_pTransformCom->Get_WorldMatrix();
    // -1을 예외 처리하는 것보다. 벡터의 첫 번째는 항등 행렬로 두고 일괄적으로 처리하는 것은 어때?
    // 이젠 조금 늦은듯 vector<CBone*> 을 이제는 mesh 도 참조해야하고 이걸 이름도 있어야 하니깐 네 조금 애매하네요
    if (-1 == m_iParentBoneIndex)
    {
        XMStoreFloat4x4(&m_CombinedTransformationMatrix, PreTransformMatrix * SeperateTransformMatrix * XMLoadFloat4x4(&m_TransformationMatrix));
    }
    else
    {
        if (pRootBone != nullptr && !strcmp(pRootBone, m_szName))
            RootAnimation(Bones, PreTransformMatrix, pModel, pAnimation);
        else
        {
            XMStoreFloat4x4(&m_CombinedTransformationMatrix,
                XMLoadFloat4x4(&m_TransformationMatrix) * SeperateTransformMatrix * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
        }
    }
}

HRESULT CBone::Save(HANDLE& hFile, const _wstring& strFilePath)
{
    assert(!(INVALID_HANDLE_VALUE == hFile));
    _ulong dwByte = 0;

    WriteFile(hFile, m_szName, sizeof(m_szName), &dwByte, nullptr);
    if (sizeof(m_szName) != dwByte)
        return E_FAIL;

    WriteFile(hFile, &m_iParentBoneIndex, sizeof(m_iParentBoneIndex), &dwByte, nullptr);

    WriteFile(hFile, &m_TransformationMatrix, sizeof(m_TransformationMatrix), &dwByte, nullptr);

    return S_OK;
}

void CBone::Reset_Transform(const vector<CBone*>& Bones, CAnimation* pAnimation)
{
    CAnimation::ROOTMOTION eRootMotion = pAnimation->Get_RootAnimation();

    _matrix SeperateTransformMatrix = m_pTransformCom->Get_WorldMatrix();

    if (eRootMotion == CAnimation::ROOTMOTION::ROOT_NONE)
        XMStoreFloat4x4(&m_PreCombinedTransformationMatrix, XMMatrixIdentity());
    else XMStoreFloat4x4(&m_PreCombinedTransformationMatrix,
        XMLoadFloat4x4(&m_TransformationMatrix) * SeperateTransformMatrix * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
}

void CBone::RootAnimation(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix, CModel* pModel, CAnimation* pAnimation)
{
    CAnimation::ROOTMOTION eRootMotion = pAnimation->Get_RootAnimation();
    _matrix SeperateTransformMatrix = m_pTransformCom->Get_WorldMatrix();

    //m_PreCombinedTransformationMatrix = m_CombinedTransformationMatrix;

    XMStoreFloat4x4(&m_CombinedTransformationMatrix,
        XMLoadFloat4x4(&m_TransformationMatrix) * SeperateTransformMatrix * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));

    if (eRootMotion != CAnimation::ROOTMOTION::ROOT_NONE)
    {
        _vector vPrevScale{}, vPrevRotation{}, vPrevTranslation{};
        _vector vInScale{}, vInRotation{}, vInTranslation{};

        XMMatrixDecompose(&vPrevScale, &vPrevRotation, &vPrevTranslation, XMLoadFloat4x4(&m_PreCombinedTransformationMatrix));
        XMMatrixDecompose(&vInScale, &vInRotation, &vInTranslation, XMLoadFloat4x4(&m_CombinedTransformationMatrix));

        _vector vScale = vInScale;
        _vector vRotation = vInRotation;
        _vector vTranslation = vInTranslation - vPrevTranslation;

        _matrix matTransform =
            XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorSet(0.f, 0.f, 0.f, 1.f));

        XMStoreFloat4x4(&m_CombinedTransformationMatrix, matTransform);

        if (eRootMotion == CAnimation::ROOTMOTION::ROOT_TRANSFORM)
        {
            CGameObject* pGameObject = pModel->Get_Owner();
            CTransform* pTransform = static_cast<CTransform*>(pGameObject->Find_Component(g_strTransformTag));

            _vector vPrevPosition = pTransform->Get_State(CTransform::STATE::POSITION);
            _vector vDeltaPosition = vPrevPosition + vTranslation;

            pTransform->Set_State(CTransform::STATE::POSITION, vDeltaPosition * 0.9f);
        }
    }

    m_PreCombinedTransformationMatrix = m_CombinedTransformationMatrix;
}

CBone* CBone::Create(const aiNode* pAINode, _int iParentBoneIndex, const vector<CBone*>& Bones)
{
    CBone* pInstance = new CBone();

    if (FAILED(pInstance->Initialize(pAINode, iParentBoneIndex, Bones)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Create : CBone");
    }

    return pInstance;
}

CBone* CBone::Create_From_File(const HANDLE& hFile, const vector<CBone*>& Bones)
{
    CBone* pInstance = new CBone();

    if (FAILED(pInstance->Initialize_From_File(hFile, Bones)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Create : CBone");
    }

    return pInstance;
}

CBone* CBone::Clone() const
{
    return new CBone(*this);
}

void CBone::Free()
{
    super::Free();

    Safe_Release(m_pTransformCom);
}
