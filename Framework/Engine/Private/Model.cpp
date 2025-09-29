#include "Model.h"

#include "Mesh.h"
#include "Material.h"
#include "Bone.h"
#include "Animation.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
}

CModel::CModel(const CModel& Prototype)
	: CComponent { Prototype }
	, m_Meshes { Prototype.m_Meshes }
	, m_Materials { Prototype.m_Materials }
	//, m_Bones { Prototype.m_Bones } // 깊은 복사
	//, m_Animations { Prototype.m_Animations } // 깊은 복사
	, m_eType { Prototype.m_eType }
    , m_PreTransformMatrix { Prototype.m_PreTransformMatrix }
    , m_szRootBone { Prototype.m_szRootBone }
{
    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);

    for (auto& pMesh : m_Materials)
        Safe_AddRef(pMesh);

    for (auto& pPrototypeBone : Prototype.m_Bones)
        m_Bones.push_back(pPrototypeBone->Clone());

    for (auto& pPrototypeAnimation : Prototype.m_Animations)
        m_Animations.push_back(pPrototypeAnimation->Clone());
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _string& strFilePath, _fmatrix PreTransformMatrix, _bool isNeedOfRecalculation)
{
    string strAssetPath = strFilePath;
    size_t DotPosition = strAssetPath.find_last_of('.');
    strAssetPath = strAssetPath.substr(0, DotPosition);
    strAssetPath += ".model";

    // ㅎㅎ A 쓰는거 되냐? 조금거시기하네
    _ulong dwAttributes = GetFileAttributesA(strAssetPath.c_str());

    _bool isExist = (dwAttributes != INVALID_FILE_ATTRIBUTES);
    if (isExist)
    //if (false)
    {
		_bool isDirectory = (dwAttributes & FILE_ATTRIBUTE_DIRECTORY);

        if (isDirectory)
        {
            MSG_BOX("Check File : there is Directory with same name");
            return E_FAIL;
        }

		_wstring strFilePathWideChar = CUtils::ConvertToWideChar(strAssetPath);
        // 아직 로드 구현 안ㅇ해서 Not Imple 오류 날거임 이제 이거 구현해얗마
		return Load_Model(strFilePathWideChar, PreTransformMatrix, isNeedOfRecalculation);
    }

    m_eType = eType;
    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

    _uint iFlag { 0 };
    iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

    if (TYPE::PRE_TRANSFORM == eType)
        iFlag |= aiProcess_PreTransformVertices;

    /* 이것만으로 모든 로드작업은 끝난거다. */
    m_pAIScene = m_Importer.ReadFile(strFilePath, iFlag);
    if (nullptr == m_pAIScene)
    {
        MSG_BOX("Failed To Read FBX File");
        return E_FAIL;
    }
   
    /* 모델클래스의 주목적 : 모델을 움직이고 그린다. */
    /* 로드해준 데이터를 베이스로해서 움직이고 그리기위해 내 구조에 맞게 정리하는 작업이 필요하다. */
    if (FAILED(Ready_Bones(m_pAIScene->mRootNode)))
        return E_FAIL;

    if (FAILED(Ready_Meshes(PreTransformMatrix)))
        return E_FAIL;

    if (FAILED(Ready_Materials(strFilePath)))
        return E_FAIL;

    if (FAILED(Ready_Animations()))
        return E_FAIL;



    if (!isExist)
    //if (true)
    {
        // 위에서 디렉토리가 아님은 이미 검사했다. 크게 상관없긴하다.
        Save_Model(CUtils::ConvertToWideChar(strAssetPath));
    }

    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    
    if (nullptr == pDesc)
		return S_OK;

    for (_uint iIndex : pDesc->InvisibleMeshIndices)
        m_Meshes[iIndex]->Is_Render(false);

    for (size_t i = 0; i < pDesc->ShaderPasses.size(); ++i)
        m_Materials[i]->Set_ShaderPass(pDesc->ShaderPasses[i]);

    m_pOwner = pDesc->pOwner;
    m_szRootBone = pDesc->pRootBone;

    return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
    if (iMeshIndex >= m_Meshes.size())
        return E_FAIL;

    if (FAILED(m_Meshes[iMeshIndex]->Bind_InputAssembler()))
        return E_FAIL;
    if (FAILED(m_Meshes[iMeshIndex]->Render()))
        return E_FAIL;

    return S_OK;
}

void CModel::SetUp_Animation(_uint iAnimIndex, _bool isLoop, _float fLerpTime)
{
    //if (iAnimIndex != m_iCurrentAnimIndex)
    {
        m_eState = STATE::TRANSITION;

        m_Animations[m_iCurrentAnimIndex]->Reset();
        m_Animations[m_iCurrentAnimIndex]->Set_Playing(false);
        m_Animations[m_iCurrentAnimIndex]->Arrange_AnimNotify();

        m_iCurrentAnimIndex = iAnimIndex;
        m_iCurrPlayingAnimIndex = m_iCurrentAnimIndex;
        m_isAnimLoop = isLoop;

        m_fAnimLerpTime = fLerpTime;

        // transition 중에 또 한번 transition 을 하게 되는 경우를 처리하기 위해
        m_Animations[m_iCurrentAnimIndex]->Reset();
        m_Animations[m_iCurrentAnimIndex]->Set_Playing(true);
        if (isLoop) m_Animations[m_iCurrentAnimIndex]->Set_BlendOutTime(0.f);
        m_CurrentChannelKeyFrames = m_Animations[m_iCurrentAnimIndex]->Get_CurrentChannelKeyFrames(m_Bones); // 이결과물을 어디에 저장할지 model , animation 에서 할지 // 지금은 모델에서 하는게 좋지 안흥ㄹ까?
    }
}

void CModel::SetUp_Animation(_uint iAnimIndex, _bool isLoop)
{
    SetUp_Animation(iAnimIndex, isLoop, 0.f);
    m_fAnimLerpTime = m_Animations[m_iCurrentAnimIndex]->Get_BaseLerpTime();
}

_bool CModel::Play_Animation(_float fTimeDelta, _float fAnimSpeed)
{
    _bool isAnimFinished{ FALSE };

    m_iCurrPlayingAnimIndex = m_iCurrentAnimIndex;

    switch (m_eState)////////////////
    {
    case STATE::ANIMATION:
        /* 현재 애니메이션의 재생시간에 맞는 뼈의 상태를 받아와서 뼈들이 가지고 있는 TrnasformationMatrix를 갱신해준다. */
        isAnimFinished = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_isAnimLoop, fAnimSpeed, this);

        m_fPreProgressRate = m_fProgressRate;
        m_fProgressRate = m_Animations[m_iCurrentAnimIndex]->Compute_Progress();

        break;

    case STATE::TRANSITION:
    {
        _bool isTransitionFinished =
            m_Animations[m_iCurrentAnimIndex]->
            Update_TransformationMatrix_While_Transition(fTimeDelta, m_fAnimLerpTime, m_Bones, m_CurrentChannelKeyFrames, this);

        if (isTransitionFinished)
            m_eState = STATE::ANIMATION;

        break;
    }

    default:
        break;
    }

    for (_uint i = 0; i < m_vecSlotAnimActive.size(); i++)
    {
        if(m_vecSlotAnimActive[i])
            isAnimFinished = Play_SlotAnimation(i, fTimeDelta, fAnimSpeed);
    }

    /* 모든 뼈의 CombinedTransformationMatrix를 갱신한다. */
    /* CombinedTransformationMatrix = 나의 TrnasformationMatrix * 부모의 Combined */
    for (auto& pBone : m_Bones)
    {
        pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix), this, m_Animations[m_iCurrentAnimIndex]);
    }

    return isAnimFinished;
}

HRESULT CModel::Bind_Material(CShader* pShader, _uint iMeshIndex, MATERIAL_TEXTURE eType, const _char* pConstantName, _uint iTextureIndex)
{
    _uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

    return m_Materials[iMaterialIndex]->Bind_Material(pShader, static_cast<aiTextureType>(eType), pConstantName, iTextureIndex);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, _uint iMeshIndex, const _char* pConstantName)
{
    if (iMeshIndex >= m_Meshes.size())
        return E_FAIL;

    return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);
}

HRESULT CModel::Bind_BaseColor(CShader* pShader, _uint iMeshIndex, const _char* pConstantName) const
{
    _uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

    return m_Materials[iMaterialIndex]->Bind_BaseColor(pShader, pConstantName);
}

HRESULT CModel::Bind_EmissionColor(CShader* pShader, _uint iMeshIndex, const _char* pConstantName) const
{
    _uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

    return m_Materials[iMaterialIndex]->Bind_EmissionColor(pShader, pConstantName);
}

_uint CModel::Get_BoneIndex_By_Name(const _char* pBoneName) const
{
    _uint iBoneIndex { 0 };

	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
        {
            if (false == strcmp(pBoneName, pBone->Get_Name()))
                return true;

            ++iBoneIndex;
            return false;
        });

    if (iter == m_Bones.end())
    {
        MSG_BOX("There is NO Bone ");
        __debugbreak();
    }

    return iBoneIndex;
}

/*const*/ _float4x4& CModel::Get_BoneCombinedMatrix_By_Name(const _char* pBoneName) const
{
    auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&pBoneName](CBone* pBone)->_bool
        {
            return (false == strcmp(pBoneName, pBone->Get_Name()));
        });

    if (iter == m_Bones.end())
    {
        MSG_BOX("There is No Bone");
        // what happen when (*m_Bones.end())->member_Function() ? 
            // vector 가 알아서 관리해주니까(프로젝트 터트려 주니까) 괜찮지 않을까?
            // 없으면 당연히 안되니까 어디서 에러나는지 알려줘야지
       
    }

    return (*iter)->Get_CombindedTransformationMatrix_Ref();
}

void CModel::Is_Render(_uint iMeshIndex, _bool isRender)
{
	m_Meshes[iMeshIndex]->Is_Render(isRender);
}

_bool CModel::Is_Render(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Is_Render();
}

_string CModel::Get_MeshName(_uint iMeshIndex) const
{
    return m_Meshes[iMeshIndex]->Get_Name();
}

_uint CModel::Get_MaterialIndex_In_Mesh(_uint iMeshIndex)
{
    return m_Meshes[iMeshIndex]->Get_MaterialIndex();
}

const vector<_uint>& CModel::Get_BoneIndices_In_Mesh(_uint iMeshIndex)
{
    return m_Meshes[iMeshIndex]->Get_BoneIndices();
}

const vector<VTXANIMMESH>& CModel::Get_VertexData(_uint iMeshIndex)
{
    return m_Meshes[iMeshIndex]->Get_VertexData();
}

const vector<_uint>& CModel::Get_IndexData(_uint iMeshIndex)
{
    return m_Meshes[iMeshIndex]->Get_IndexData();
}

const _float4& CModel::Get_BaseColor(_uint iMtrlIdx) const
{
    return m_Materials[iMtrlIdx]->Get_BaseColor();
}

void CModel::Set_BaseColor(_uint iMtrlIdx, const _float4& vColor)
{
    m_Materials[iMtrlIdx]->Set_BaseColor(vColor);
}


_uint CModel::Get_ShaderPass(_uint iMaterialIndex)
{
    return m_Materials[iMaterialIndex]->Get_ShaderPass();
}

void CModel::Set_ShaderPass(_uint iMaterialIndex, _uint iShaderPass)
{
    m_Materials[iMaterialIndex]->Set_ShaderPass(iShaderPass);
}

const vector<pair<_uint, _tchar[MAX_PATH]>>& CModel::Get_TextureFiles_By_Type(_uint iMaterialIndex)
{
    return m_Materials[iMaterialIndex]->Get_TextureFiles_By_Type();
}

const vector<ID3D11ShaderResourceView*>& CModel::Get_Mtrltextures(_uint iMaterialIndex, _uint iType)
{
    return m_Materials[iMaterialIndex]->Get_MtrlTextures(iType);
}

_bool CModel::Has_Texture(_uint iMaterialIndex, MATERIAL_TEXTURE eTexture) const
{
    return m_Materials[iMaterialIndex]->Has_Texture(eTexture);
}

_string CModel::Get_BoneName(_uint iBoneIndex) const
{
    // 이러면 임시 객체 아니냐? 괜찮냐
    return _string(m_Bones[iBoneIndex]->Get_Name());
}

CTransform* CModel::Get_BoneSperateTransformCom(_uint iIndex)
{
    return m_Bones[iIndex]->Get_SeperateTransformCom();
}

_bool CModel::Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist)
{
    vector<_float> Distances;

    switch (m_eType)
    {
    case TYPE::PRE_TRANSFORM:
    case TYPE::NON_ANIM:
        for (auto& pMesh : m_Meshes)
        {
            _float fDistance;
            // todo : make ray to mesh local space
            if (pMesh->Ray_Cast(vRayOrigin, vRayDirection, fDistance))
            {
                Distances.push_back(fDistance);
            }
        }

        if (Distances.empty())
            return FALSE;
        else
        {
            _float fMinDist = *std::min_element(Distances.begin(), Distances.end());
            fDist = fMinDist;
            return TRUE;
        }
		break;

	case TYPE::ANIM:
        MSG_BOX("Anim Mesh Cannot Ray Cast Yet");
		return FALSE;
		break;

    default:
        assert(!"NO Type In Model");
        return FALSE;
        break;
    }
}

_bool CModel::Is_Progress(_float fRate) const
{
    return m_fPreProgressRate < fRate && m_fProgressRate >= fRate;
}

_bool CModel::Has_Texture_In_MeshMaterial(_uint iMeshIndex, MATERIAL_TEXTURE eTexture) const
{
    _uint iMaterialIndex= m_Meshes[iMeshIndex]->Get_MaterialIndex();
    return Has_Texture(iMaterialIndex, eTexture);
}

_bool CModel::Get_IsPlaying(_uint iAnimIndex)
{
    return m_Animations[iAnimIndex]->Get_Playing();
}

_uint CModel::Get_AnimationIndex(const _char* pAnimationName)
{
    _uint iAnimIndex{};

    auto iter = find_if(m_Animations.begin(), m_Animations.end(), [&](CAnimation* pAnim)->bool
        {
            if (strcmp(pAnim->Get_Name(), pAnimationName) == false)
                return true;

            iAnimIndex++;
            return false;
        });

    return iAnimIndex;
}

void CModel::Set_AnimationFrame_Specific(_double fTrackPosition)
{
    Reset_Animation();

    m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix_Specific(fTrackPosition, m_Bones, this);
    for (auto& pBone : m_Bones)
        pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix), this, m_Animations[m_iCurrentAnimIndex]);
}

void CModel::Reset_Animation()
{
    m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix_Specific(0.f, m_Bones, this);

    for (auto& pBone : m_Bones)
        pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix), this, m_Animations[m_iCurrentAnimIndex]);
}

HRESULT CModel::Save_Animation(_uint iAnimIndex)
{
    if (FAILED(m_Animations[iAnimIndex]->Save()))
        return E_FAIL;

    return S_OK;
}

void CModel::Add_AnimNotify(_uint iAnimIndex, _double fTrackPosition, string& strNotifyName)
{
    m_Animations[iAnimIndex]->Add_AnimNotify(fTrackPosition, strNotifyName);
}

void CModel::Change_AnimNotify(_uint iAnimIndex, string& strNotifyName, string& strChangeName, _double fTrackPosition, _double fChangeTrack)
{
    m_Animations[iAnimIndex]->Change_AnimNotify(strNotifyName, strChangeName, fTrackPosition, fChangeTrack);
}

void CModel::Delete_AnimNotify(_uint iAnimIndex, string& strNotifyName, _double fTrackPosition)
{
    m_Animations[iAnimIndex]->Delete_AnimNotify(strNotifyName, fTrackPosition);
}

void CModel::Reset_RootBone(_uint iAnimIndex)
{
    _uint iBoneIndex = Get_BoneIndex_By_Name(m_szRootBone);

    CBone* pRootBone = m_Bones[iBoneIndex];
    pRootBone->Reset_Transform(m_Bones, m_Animations[iAnimIndex]);
}

void CModel::Add_AnimSlot(const _char* szStartBone, const _char* szEndBone)
{
    _uint iStartIndex{}, iEndIndex{};

    for (_uint i = 0; i < m_Bones.size(); i++)
    {
        if (!strcmp(m_Bones[i]->Get_Name(), szStartBone))
            iStartIndex = i;
        else if (!strcmp(m_Bones[i]->Get_Name(), szEndBone))
            iEndIndex = i;
    }

    vector<_bool> vecSlot(m_Bones.size(), false);

    for (_uint i = iStartIndex; i <= iEndIndex; i++)
        vecSlot[i] = true;

    m_vecSlotBones.push_back(vecSlot);
    m_vecSlotAnimation.push_back(nullptr);
    m_vecSlotState.push_back(STATE::ANIMATION);
    m_vecSlotAnimIndex.push_back(0);
    m_vecSlotAnimLoop.push_back(false);
    m_vecSlotAnimActive.push_back(false);
    m_vecSlotAnimLerpTime.push_back(0.2f);
}

void CModel::Activate_SlotAnim(_uint iSlotIndex, _bool bActive)
{
    m_vecSlotAnimActive[iSlotIndex] = bActive;
}

void CModel::SetUp_SlotAnimation(_uint iSlotIndex, _uint iAnimIndex, _bool isLoop, _float fLerpTime)
{
    //if (iAnimIndex != m_iCurrentAnimIndex)
    {
        m_vecSlotState[iSlotIndex] = STATE::TRANSITION;

        m_vecSlotAnimIndex[iSlotIndex] = iAnimIndex;
        m_vecSlotAnimLoop[iSlotIndex] = isLoop;

        m_vecSlotAnimLerpTime[iSlotIndex] = fLerpTime;

        m_vecSlotAnimation[iSlotIndex] = m_Animations[iAnimIndex];

        m_vecSlotAnimation[iSlotIndex]->Reset();
        m_vecSlotAnimation[iSlotIndex]->Set_Playing(true);
        if (isLoop) m_vecSlotAnimation[iSlotIndex]->Set_BlendOutTime(0.f);
        m_CurrentChannelKeyFrames = m_vecSlotAnimation[iSlotIndex]->Get_CurrentChannelKeyFrames(m_Bones);
    }
}

void CModel::SetUp_SlotAnimation(_uint iSlotIndex, _uint iAnimIndex, _bool isLoop)
{
    SetUp_SlotAnimation(iSlotIndex, iAnimIndex, isLoop, 0.f);
    m_vecSlotAnimLerpTime[iSlotIndex] = m_vecSlotAnimation[iSlotIndex]->Get_BaseLerpTime();
}

_bool CModel::Play_SlotAnimation(_uint iSlotIndex, _float fTimeDelta, _float fAnimSpeed)
{
    _bool isAnimFinished{ FALSE };

    m_bSlotPlaying = true;
    m_iCurrSlotIndex = iSlotIndex;
    m_iCurrPlayingAnimIndex = m_vecSlotAnimIndex[iSlotIndex];

    switch (m_vecSlotState[iSlotIndex])
    {
    case STATE::ANIMATION:
        /* 현재 애니메이션의 재생시간에 맞는 뼈의 상태를 받아와서 뼈들이 가지고 있는 TrnasformationMatrix를 갱신해준다. */
        isAnimFinished = m_vecSlotAnimation[iSlotIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_vecSlotAnimLoop[iSlotIndex], fAnimSpeed, this);

        break;

    case STATE::TRANSITION:
    {
        _bool isTransitionFinished =
            m_vecSlotAnimation[iSlotIndex]->
            Update_TransformationMatrix_While_Transition(fTimeDelta, m_vecSlotAnimLerpTime[iSlotIndex], m_Bones, m_CurrentChannelKeyFrames, this);

        if (isTransitionFinished)
            m_vecSlotState[iSlotIndex] = STATE::ANIMATION;

        break;
    }

    default:
        break;
    }

    m_bSlotPlaying = false;

    return isAnimFinished;
}

_bool CModel::Is_SlotBone_Total(_uint iBoneIndex)
{
    for (_uint i = 0; i < m_vecSlotAnimActive.size(); i++)
    {
        if (m_vecSlotAnimActive[i] && m_vecSlotBones[i][iBoneIndex])
            return true;
    }

    return false;
}

_bool CModel::Is_SlotBone(_uint iBoneIndex) const
{
    _bool bResult{};
    for (_uint i = 0; i < m_vecSlotAnimActive.size(); i++)
    {
        if (i == m_iCurrSlotIndex)
            bResult = m_vecSlotBones[m_iCurrSlotIndex][iBoneIndex];
        else if (m_vecSlotAnimActive[i] && m_vecSlotBones[i][iBoneIndex])
            return false;
    }

    return bResult;
}

_bool CModel::Is_CurrSlotAnim(_uint iAnimIndex)
{
    return m_vecSlotAnimation[m_iCurrSlotIndex] == m_Animations[iAnimIndex];
}

HRESULT CModel::Ready_Meshes(_fmatrix PreTransformMatrix) /* 이것도 m_PreTransformMatrix이 있으니까 파라미터 필요 없긴한데 우선 냅두고 넘어가자 */
{
    m_Meshes.resize(m_pAIScene->mNumMeshes);

    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, this, m_pAIScene->mMeshes[i], PreTransformMatrix, m_Bones);
        if (nullptr == pMesh)
            return E_FAIL;

        m_Meshes[i] = pMesh;
    }

    return S_OK;
}

HRESULT CModel::Ready_Materials(const _string& strModelFilePath)
{
    _uint iNumMaterials = m_pAIScene->mNumMaterials;
    m_Materials.reserve(iNumMaterials);

    for (size_t i = 0; i < iNumMaterials; ++i)
    {
        CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, m_pAIScene->mMaterials[i], strModelFilePath);
        if (nullptr == pMaterial)
            return E_FAIL;

        m_Materials.push_back(pMaterial);
    }

    return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
    CBone* pBone = CBone::Create(pAINode, iParentIndex, m_Bones);
    if (nullptr == pBone)
        return E_FAIL;

    m_Bones.push_back(pBone);

    _int iCurrentIndex = static_cast<_int>(m_Bones.size()) - 1;

    for (size_t i = 0; i < pAINode->mNumChildren; ++i)
    {
        Ready_Bones(pAINode->mChildren[i], iCurrentIndex);
    }

    return S_OK;
}

HRESULT CModel::Ready_Animations()
{
    _uint iNumAnimations = m_pAIScene->mNumAnimations;
    m_Animations.reserve(iNumAnimations);

    for (size_t i = 0; i < iNumAnimations; ++i)
    {
        CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], this);
        if (nullptr == pAnimation)
            return E_FAIL;

        m_Animations.push_back(pAnimation);
    }
    return S_OK;
}

HRESULT CModel::Save_Model(const _wstring& strFilePath)
{
    // open file
    _ulong dwByte = 0;
    HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File : CModel");
        return E_FAIL;
    }

    WriteFile(hFile, &m_eType, sizeof(TYPE), &dwByte, nullptr);
    //WriteFile(hFile, &m_PreTransformMatrix, sizeof(_float4x4), &dwByte, nullptr);

    _uint iSize = 0;

	// save num of bones; 
    iSize = (_uint)m_Bones.size();
    WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

    for (auto& pBones : m_Bones)
    {
        pBones->Save(hFile, strFilePath);
        // name
        // parrent bone index
        // transformation matrix
    }


    iSize = (_uint)m_Meshes.size();
    WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

    for (auto& pMeshes : m_Meshes)
    {
        pMeshes->Save(hFile, this, XMLoadFloat4x4(&m_PreTransformMatrix));
    }


    iSize = (_uint)m_Materials.size();
    WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

    for (auto& pMaterial : m_Materials)
    {
        pMaterial->Save(hFile);
    }

    CloseHandle(hFile);

    if (FAILED(Save_Animation(strFilePath)))
        return E_FAIL;

    return S_OK;
}

HRESULT CModel::Load_Model(const _wstring& strFilePath, _fmatrix PreTransformMatrix, _bool isNeedOfRecalculation)
{
    _ulong dwByte = 0;
    _bool bReadFileResult = FALSE;

    HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (INVALID_HANDLE_VALUE == hFile)
        return E_FAIL;

    bReadFileResult = ReadFile(hFile, &m_eType, sizeof(TYPE), &dwByte, nullptr);
    //ReadFile(hFile, &m_PreTransformMatrix, sizeof(_float4x4), &dwByte, nullptr);
    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);


    _uint iNumElements = 0;


    bReadFileResult = ReadFile(hFile, &iNumElements, sizeof(_uint), &dwByte, nullptr);
    for (_uint i = 0; i < iNumElements; ++i)
    {
        CBone* pBone = CBone::Create_From_File(hFile, m_Bones);
        m_Bones.push_back(pBone);
    }

    bReadFileResult = ReadFile(hFile, &iNumElements, sizeof(_uint), &dwByte, nullptr);
    //vector<CMesh*> Meshes(iNumElements); // 생성자가 private 인데 .. 왜 컴파일 오류가 없지? // 메모리만 잡아두는건가?
    vector<CMesh*> Meshes;
    Meshes.reserve(iNumElements);
    for (_uint i = 0; i < iNumElements; ++i)
    {
        CMesh* pMesh = CMesh::Create_From_File(m_pDevice, m_pContext, this, hFile, PreTransformMatrix, isNeedOfRecalculation);
        m_Meshes.push_back(pMesh);
    }

    bReadFileResult = ReadFile(hFile, &iNumElements, sizeof(_uint), &dwByte, nullptr);
    for (_uint i = 0; i < iNumElements; ++i)
    {
        CMaterial* pMaterial = CMaterial::Create_From_File(m_pDevice, m_pContext, hFile, strFilePath);
        m_Materials.push_back(pMaterial);
    }

    CloseHandle(hFile);

    if (FAILED(Load_Animation(strFilePath)))
        return E_FAIL;

    return S_OK;
}

HRESULT CModel::Save_Animation(const _wstring& strFilePath)
{
    string strPath = CUtils::ConvertToMultibyteChar(strFilePath);
    _char szDirectory[MAX_PATH]{};

    _splitpath_s(strPath.c_str(), nullptr, 0, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

    _char szPath[MAX_PATH]{};
    strcat_s(szPath, szDirectory);
    strcat_s(szPath, "Animation");

    string strDirPath = string(szPath);

    if (!CUtils::CreateDirectories(strDirPath))
        return E_FAIL;

    for (auto& pAnimation : m_Animations)
        pAnimation->Save(szPath);

    return S_OK;
}

HRESULT CModel::Load_Animation(const _wstring& strFilePath)
{
    if (TYPE::ANIM == m_eType)
    {
        _string strPath = CUtils::ConvertToMultibyteChar(strFilePath);
        _char szDirectory[MAX_PATH]{};

        _splitpath_s(strPath.c_str(), nullptr, 0, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

        _char szPath[MAX_PATH]{};
        strcat_s(szPath, szDirectory);
        strcat_s(szPath, "Animation");

        strPath = _string(szPath);
        _wstring strDirPath = CUtils::ConvertToWideChar(strPath);

        vector<_wstring> vecPath;
        vector<_wstring> vecExt = { L".ANIM" };
        CUtils::TraverseDirectory_Ext(strDirPath, vecPath, vecExt);

        CAnimation* pAnimation{};

        for (size_t i = 0; i < static_cast<size_t>(vecPath.size()); i++)
        {
            _uint dwByte{};
            HANDLE hFile{};

            // Animation 로드
            {
                hFile = CreateFile(vecPath[i].c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
                if (hFile == INVALID_HANDLE_VALUE)
                    return E_FAIL;

                pAnimation = CAnimation::Create_From_File(hFile, this, vecPath[i]);
                CloseHandle(hFile);
            }

            m_Animations.push_back(pAnimation);

            // AnimData 로드
            {
                _string strDataPath = CUtils::ConvertToMultibyteChar(vecPath[i]);

                _char szAnimName[MAX_PATH]{};
                _splitpath_s(strDataPath.c_str(), nullptr, 0, nullptr, 0, szAnimName, MAX_PATH, nullptr, 0);

                _char szDataPath[MAX_PATH]{};
                _char szFolderPath[MAX_PATH]{};
                strcat_s(szDataPath, szPath);
                strcat_s(szDataPath, "/AnimData");

                strcat_s(szFolderPath, szDataPath);
                _string strFolderPath = _string(szDataPath);

                strcat_s(szDataPath, "/");
                strcat_s(szDataPath, szAnimName);
                strcat_s(szDataPath, ".AnimData");

                strDataPath = _string(szDataPath);
                _wstring wstrDataPath = CUtils::ConvertToWideChar(strDataPath);

                hFile = CreateFile(wstrDataPath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
                if (hFile == INVALID_HANDLE_VALUE)
                {
                    if (!CUtils::CreateDirectories(strFolderPath))
                        return E_FAIL;

                    pAnimation->Save_AnimData(szPath);
                }
                else
                {
                    pAnimation->Load_AnimData(hFile);
                    CloseHandle(hFile);
                }
            }
        }
    }

    return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _string& strFilePath, TYPE eType, _fmatrix PreTransformMatrix, _bool isNeedOfRecalculate)
{
    CModel* pInstance = new CModel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eType, strFilePath, PreTransformMatrix, isNeedOfRecalculate)))
    {
        MSG_BOX("Failed To Create : CModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CModel::Clone(void* pArg)
{
    CModel* pInstance = new CModel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModel::Free()
{
    super::Free();

    for (auto& pMesh : m_Meshes)
        Safe_Release(pMesh);
    m_Meshes.clear();

    for (auto& pMaterial : m_Materials)
        Safe_Release(pMaterial);
    m_Materials.clear();

    for (auto& pBone : m_Bones)
        Safe_Release(pBone);
    m_Bones.clear();

    for (auto& pAnimation : m_Animations)
        Safe_Release(pAnimation);
    m_Animations.clear();
}
