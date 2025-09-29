#include "pch.h"
#include "NPC_Inkerton_Final.h"

#include "GameInstance.h"
#include "CallbackUpdate.h"
#include "Boss_Inkerton.h"
#include "Animation.h"
#include "Player.h"

#include "UI_Manager.h"
#include "CutScene_Manager.h"

#include "CineCamera.h"
#include "NPC_Inkerton_Final_Look.h"

CNPC_Inkerton_Final::CNPC_Inkerton_Final(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CNPC_Inkerton_Final::CNPC_Inkerton_Final(const CNPC_Inkerton_Final& Prototype)
    : super(Prototype)
{
}

HRESULT CNPC_Inkerton_Final::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Inkerton_Final::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    if (FAILED(Load_Data(*pDesc)))
        return E_FAIL;

    pDesc->GameObjectDesc.TransformDesc.fRotationPerSec = XM_2PI;

    if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObject()))
        return E_FAIL;

    if (FAILED(Ready_Event()))
        return E_FAIL;

    m_pModelCom->SetUp_Animation((_uint)ANIM_IDLE, true, 0.2f);
    m_pModelCom->Play_Animation(1.f / 200.f);

    m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

    m_pCutSceneManager = static_cast<CCutScene_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CutSceneManager", 0));

    m_vecSoundKeys.push_back(L"CrabTomb_Roland_00.wav");
    m_vecSoundKeys.push_back(L"CrabTomb_Inkerton_00.wav");
    m_vecSoundKeys.push_back(L"CrabTomb_Roland_01.wav");
    m_vecSoundKeys.push_back(L"CrabTomb_Roland_02.wav");
    m_vecSoundKeys.push_back(L"CrabTomb_Roland_03.wav");
    m_vecSoundKeys.push_back(L"CrabTomb_Inkerton_01.wav");

    m_vecCaptions.push_back(L"롤랜드: 이쪽으로 가는 게 확실한가?");
    m_vecCaptions.push_back(L"잉커톤: 제가 어떻게 알겠어요? 제가 주말마다 하수구 클럽에서 놀기라도 하는 줄 아세요?");
    m_vecCaptions.push_back(L"롤랜드: 그러면 도대게 내가 당신에게 급여를 줄 이유가 뭐지?");
    m_vecCaptions.push_back(L"롤랜드: 사실상, 당신이 망할 바닷가재 한 마리 이기지 못해서 우리가 이 지경이 된 거잖나!");
    m_vecCaptions.push_back(L"롤랜드: 아무짝에도 쓸모없는... 널 발견했던 그 해저에 내버려 둘 걸 그랬어, 이 자식-");
    m_vecCaptions.push_back(L"잉커톤: 너구나...");

    return S_OK;
}

void CNPC_Inkerton_Final::Priority_Update(_float fTimeDelta)
{
    super::Priority_Update(fTimeDelta);
}

void CNPC_Inkerton_Final::Update(_float fTimeDelta)
{
    if (m_pCutSceneManager->Get_CurPlaying() == L"CrabTomb_Start" && !m_bStart)
    {
        m_pGameInstance->Camera_Transition(L"Inkerton_Camera", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
        m_pCineCamera->Play(L"CrabTomb_Start", L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);

        m_bStart = true;
    }

    if (!m_isActive)
        return;

    if (m_bStart)
    {
        if (m_pGameInstance->Key_Down(KEY::EQUALS))
        {
            m_pGameInstance->Stop_Sound_All();
            m_pCutSceneManager->Begin_CutScene(L"CrabTomb_Start");
            m_iCurPlaySoundIndex = 0;
        }

        m_pModelCom->Play_Animation(fTimeDelta);

        if (!m_pGameInstance->IsPlaying((_uint)SOUND_CHANNEL::CONVERSATION1) && !m_pGameInstance->IsPlaying((_uint)SOUND_CHANNEL::CONVERSATION2))
        {
            // 탕!
            if (m_iCurPlaySoundIndex == 5 && m_pCutSceneManager->Get_CurPlaying() != L"CrabTomb_Roland_Die")
            {
                if (!m_pCutSceneManager->Is_CutSceneFinished(L"CrabTomb_Roland_Die"))
                {
                    m_pGameInstance->Play_Sound(L"Inkerton_Fire_1.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK1, 1.f);
                    m_pCutSceneManager->Begin_CutScene(L"CrabTomb_Roland_Die");
                    m_pCineCamera->PositionShake(0.1f, 20.f, 0.5);
                    m_pCineCamera->RotationShake(0.1f, 20.f, 0.5);
                }
            }

            if (m_iCurPlaySoundIndex >= m_vecSoundKeys.size())
            {
                m_pCutSceneManager->End_CutScene(L"CrabTomb_Start");
                m_pCutSceneManager->End_CutScene(L"CrabTomb_End");
                m_pGameInstance->Play_Sound_Loop(L"Inkerton_BGM1.wav", (_uint)SOUND_CHANNEL::BGM, 0.7f);

                CBoss_Inkerton* pInkerton = static_cast<CBoss_Inkerton*>(m_pGameInstance->Find_GameObject_With_Name(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Monster"), TEXT("Boss_Inkerton")));
                CTransform* pTransform = static_cast<CTransform*>(pInkerton->Find_Component(g_strTransformTag));

                pTransform->Set_State(CTransform::STATE::LOOK, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::LOOK)));
                pTransform->Set_State(CTransform::STATE::UP, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::UP)));
                pTransform->Set_State(CTransform::STATE::RIGHT, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::RIGHT)));
                pInkerton->Locate(m_pTransformCom->Get_State(CTransform::STATE::POSITION));

                m_isActive = false;
                m_pCineCamera->Stop();
            }
            else if (m_pCutSceneManager->Get_CurPlaying() != L"CrabTomb_Roland_Die")
            {
                if(m_iCurPlaySoundIndex == 1 || m_iCurPlaySoundIndex == 5)
                    m_pGameInstance->Play_Sound(m_vecSoundKeys[m_iCurPlaySoundIndex], (_uint)SOUND_CHANNEL::CONVERSATION1, 1.f);
                else 
                    m_pGameInstance->Play_Sound(m_vecSoundKeys[m_iCurPlaySoundIndex], (_uint)SOUND_CHANNEL::CONVERSATION2, 1.f);

                m_pUIManager->Set_CutSceneUI_Caption(m_vecCaptions[m_iCurPlaySoundIndex].c_str());

                m_iCurPlaySoundIndex++;
            }
        }
    }
    
    super::Update(fTimeDelta);
}

void CNPC_Inkerton_Final::Late_Update(_float fTimeDelta)
{
    if (!m_bStart || !m_isActive)
        return;

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

    super::Late_Update(fTimeDelta);
}

HRESULT CNPC_Inkerton_Final::Render()
{
#ifdef _DEBUG
    if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID")))
        return E_FAIL;
#endif // _DEBUG

    if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
        return E_FAIL;

    for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        _uint iShaderPass = 0;
        if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE))
        {
            iShaderPass = (_uint)SHADER_PASS_ANIMMESH::DEFAULT;

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
                return E_FAIL;
        }
        else
        {
            iShaderPass = (_uint)SHADER_PASS_ANIMMESH::BASE_COLOR;

            if (FAILED(m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor")))
                return E_FAIL;
        }

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::NORMAL, "g_NormalTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::METALNESS, "g_MetalicTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::SHININESS, "g_SmoothnessTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::EMISSIVE, "g_EmissiveMaskTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_EmissionColor(m_pShaderCom, i, "g_vEmissionColor")))
            return E_FAIL;

        m_pShaderCom->Begin(iShaderPass);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CNPC_Inkerton_Final::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix_Ref())))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_ShadowMatrices_To_Shader(m_pShaderCom, "g_LightViewMatrices", "g_LightProjMatrices")))
        return E_FAIL;

    for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
    {

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        m_pShaderCom->Begin((_uint)SHADER_PASS_ANIMMESH::SHADOW);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CNPC_Inkerton_Final::Look()
{
    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player"));
    CTransform* pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(g_strTransformTag));

    m_pTransformCom->LookAt(pPlayerTransform->Get_State(CTransform::STATE::POSITION));
}

HRESULT CNPC_Inkerton_Final::Load_Data(DESC& Desc)
{
    HRESULT hr{};
    DWORD dwByte{};
    HANDLE hFile = CreateFile(Desc.szFileName, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File");
        CloseHandle(hFile);
        return E_FAIL;
    }

    _float4x4 WorldMat = {}; // 월드 행렬
    _bool bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);

    Desc.GameObjectDesc.WorldMatrix = WorldMat;

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CNPC_Inkerton_Final::Ready_Components()
{
    /* Com_Model */
    CModel::DESC ModelDesc{};
    ModelDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_NPC_Inkerton",
        L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &ModelDesc)))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
        L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Inkerton_Final::Ready_PartObject()
{
    // CineCamera
    {
        CCineCamera::DESC CineCameraDesc{};

        CineCameraDesc.CameraDesc.fFovY = XMConvertToRadians(60.f);
        CineCameraDesc.CameraDesc.fNear = 0.1f;
        CineCameraDesc.CameraDesc.fFar = 1000.f;

        CineCameraDesc.strName = L"Inkerton_Camera";
        CineCameraDesc.pOwner = this;

        if (FAILED(Add_PartObject(L"Part_CineCamera", LEVEL_STATIC, L"Prototype_GameObject_CineCamera", &CineCameraDesc)))
            return E_FAIL;

        m_pCineCamera = static_cast<CCineCamera*>(Find_PartObject({ L"Part_CineCamera" }));
        m_pCineCamera->Add_Cinematic(L"CrabTomb_Start", "../Bin/DataFiles/CameraData/CrabTomb2.data");
    }

    return S_OK;
}

HRESULT CNPC_Inkerton_Final::Ready_Event()
{
    if (FAILED(Add_Event(L"Look", CNPC_Inkerton_Final_Look::Create(this))))
        return E_FAIL;

    return S_OK;
}

CNPC_Inkerton_Final* CNPC_Inkerton_Final::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_Inkerton_Final* pInstance = new CNPC_Inkerton_Final(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNPC_Inkerton_Final");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CNPC_Inkerton_Final::Clone(void* pArg)
{
    CNPC_Inkerton_Final* pInstance = new CNPC_Inkerton_Final(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CNPC_Inkerton_Final");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_Inkerton_Final::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
