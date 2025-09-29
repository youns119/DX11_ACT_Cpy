
#include "pch.h"
#include "NPC_ShellSplitter.h"

#include "GameInstance.h"
#include "Enemy_Splitter.h"
#include "Animation.h"

#include "UI_Manager.h"
#include "CutScene_Manager.h"
#include "CineCamera.h"

#include "NPC_ShellSplitter_Fear.h"
#include "Splitter_Event_Fear.h"
#include "Splitter_Event_Effect_LockHit.h"

CNPC_ShellSplitter::CNPC_ShellSplitter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext) 
{
}

CNPC_ShellSplitter::CNPC_ShellSplitter(const CNPC_ShellSplitter& Prototype)
    : super(Prototype)
{
}

HRESULT CNPC_ShellSplitter::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_ShellSplitter::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    pDesc->GameObjectDesc.fSpeedPerSec = 15.f;

    if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObject()))
        return E_FAIL;

    if (FAILED(Ready_Event()))
        return E_FAIL;

    m_pModelCom->SetUp_Animation((_uint)ANIM_RUN, true, 0.2f);
    m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat3(&m_vStartPosition));

    m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    m_pCutSceneManager = static_cast<CCutScene_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CutSceneManager", 0));

    return S_OK;
}

void CNPC_ShellSplitter::Priority_Update(_float fTimeDelta)
{
    super::Priority_Update(fTimeDelta);
}

void CNPC_ShellSplitter::Update(_float fTimeDelta)
{
    //super 업뎃도 안돈다 이러면
    if (m_pCutSceneManager->Get_CurPlaying() != L"ShellSplitter")
        m_isRender = false;
    else
    {
        m_isRender = true;

        if (!m_isCutSceneStart)
        {
            m_isCutSceneStart = true;

            m_pGameInstance->Camera_Transition(L"Splitter_Camera", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
            m_pCineCamera->Play(L"Splitter", L"Splitter_Kril", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
            m_pGameInstance->Play_Sound(L"Field_Splitter_00.wav", (_uint)SOUND_CHANNEL::CONVERSATION2, 1.f);
            m_pUIManager->Set_CutSceneUI_Caption(L"쉘스플리터: 집게발 떼, 범법자!!!");
            m_pCineCamera->Pause();
        }
    }

    if (!m_isCutSceneStart)
        return;

    if (m_pGameInstance->Key_Down(KEY::EQUALS))
    {
        m_pModelCom->SetUp_Animation((_uint)ANIM_FEAR, false, 0.2f);
        m_pCineCamera->Stop();
        m_eCurState = STATE_END;
    }

    if (m_eCurState == STATE_RUN)
    {
        _vec3 vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
        _vec3 vGoal = XMLoadFloat3(&m_vGoalPosition);
        
        _float fDistance = (vGoal - vPosition).Length();
        
        if (fDistance < 0.1f)
        {
            m_pModelCom->SetUp_Animation((_uint)ANIM_HIT, false, 0.2f);
            m_eCurState = STATE_HIT;
        }
        else
        {
            m_pTransformCom->LookAt(vGoal._vector());
            m_pTransformCom->Go_Straight(fTimeDelta);
        }

    }
    else if (m_eCurState == STATE_HIT)
    {
        if (m_pModelCom->Get_CurrAnimation()->Get_Blendable())
        {
            m_pCineCamera->Stop();
            m_pCineCamera->Play(L"Splitter_Kril", L"Splitter", 0.1f);
            m_pCineCamera->Pause();
            m_pGameInstance->Play_Sound(L"Field_Kril_00.wav", (_uint)SOUND_CHANNEL::CONVERSATION1, 1.f);
            m_pUIManager->Set_CutSceneUI_Caption(L"크릴: 진짜 죄송해요! 돌아갈게요, 됐죠?");


            m_pModelCom->SetUp_Animation((_uint)STATE_TALK, true, 0.2f);
            m_eCurState = STATE_TALK;
        }
    }
    else if (m_eCurState == STATE_TALK)
    {
        if (!m_pGameInstance->IsPlaying((_uint)SOUND_CHANNEL::CONVERSATION1))
        {
            m_pCineCamera->Stop();
            m_pCineCamera->Play(L"Splitter", L"Camera_Player", 0.1f);
            m_pCineCamera->Pause();
            m_pGameInstance->Play_Sound(L"Field_Splitter_01.wav", (_uint)SOUND_CHANNEL::CONVERSATION2, 1.f);
            m_pUIManager->Set_CutSceneUI_Caption(L"쉘스플리터: 하! 그렇게는 안 될걸!!! 다른 쓸모없는 범법자들처럼 네게도 엄중한 처벌을 내리겠다");

            m_eCurState = STATE_FEAR;
            
        }
    }
    else if (m_eCurState == STATE_FEAR)
    {
        if (!m_pGameInstance->IsPlaying((_uint)SOUND_CHANNEL::CONVERSATION2))
        {
            m_pModelCom->SetUp_Animation((_uint)ANIM_FEAR, false, 0.2f);
            m_eCurState = STATE_END;
        }
    }
    else if(STATE_END)
    {
        if (m_pModelCom->Get_CurrAnimation()->Get_Blendable())
        {
            if (!m_isCutSceneEnd)
            {
                m_isCutSceneEnd = true;
                m_pCutSceneManager->End_CutScene(L"ShellSplitter");
                // 페이드 인 -> npc 없애고 몬스터 위치 옮기기 -> 페이드 아웃
                m_pCineCamera->Stop();
                CEnemy_Splitter* pSplitter = static_cast<CEnemy_Splitter*>(m_pGameInstance->Find_GameObject_With_Name(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Monster"), TEXT("Splitter")));
                CTransform* pTransform = static_cast<CTransform*>(pSplitter->Find_Component(g_strTransformTag));
                pTransform->Set_State(CTransform::STATE::LOOK, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::LOOK)));
                pTransform->Set_State(CTransform::STATE::UP, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::UP)));
                pTransform->Set_State(CTransform::STATE::RIGHT, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::RIGHT)));
                pSplitter->Locate(m_pTransformCom->Get_State(CTransform::STATE::POSITION));
                
                m_pGameInstance->FadeSwap_Sound(L"Spliter_BGM.wav", (_uint)SOUND_CHANNEL::BGM, 1.f, 0.5f);
            }
        }
    }
    m_pModelCom->Play_Animation(fTimeDelta);

    super::Update(fTimeDelta);
}

void CNPC_ShellSplitter::Late_Update(_float fTimeDelta)
{
    if (m_isRender)
    {
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);
    }

    super::Late_Update(fTimeDelta);
}

HRESULT CNPC_ShellSplitter::Render()
{
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

HRESULT CNPC_ShellSplitter::Render_Shadow()
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

HRESULT CNPC_ShellSplitter::Ready_Components()
{
    /* Com_Model */
    CModel::DESC tModelDesc{};
    tModelDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_NPC_ShellSplitter",
        L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &tModelDesc)))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
        L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_ShellSplitter::Ready_Event()
{
    // 공포
    if (FAILED(Add_Event(L"Event_Effect_Splitter_Fear", CSplitter_Event_Fear::Create(this))))
        return E_FAIL;

    if (FAILED(Add_Event(L"Fear", CNPC_ShellSplitter_Fear::Create(this))))
        return E_FAIL;

    // 자물쇠 내려찍기
    if (FAILED(Add_Event(L"Hit_Effect_Lock", CSplitter_Event_Effect_LockHit::Create(this))))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_ShellSplitter::Ready_PartObject()
{
    // CineCamera
    {
        CCineCamera::DESC CineCameraDesc{};

        CineCameraDesc.CameraDesc.fFovY = XMConvertToRadians(60.f);
        CineCameraDesc.CameraDesc.fNear = 0.1f;
        CineCameraDesc.CameraDesc.fFar = 1000.f;

        CineCameraDesc.strName = L"Splitter_Camera";
        CineCameraDesc.pOwner = this;

        if (FAILED(Add_PartObject(L"Part_CineCamera", LEVEL_STATIC, L"Prototype_GameObject_CineCamera", &CineCameraDesc)))
            return E_FAIL;

        m_pCineCamera = static_cast<CCineCamera*>(Find_PartObject({ L"Part_CineCamera" }));
        m_pCineCamera->Add_Cinematic(L"Splitter", "../Bin/DataFiles/CameraData/Splitter.data");
        m_pCineCamera->Add_Cinematic(L"Splitter_Kril", "../Bin/DataFiles/CameraData/Splitter_Kril.data");
    }

    return S_OK;
}

CNPC_ShellSplitter* CNPC_ShellSplitter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_ShellSplitter* pInstance = new CNPC_ShellSplitter(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNPC_ShellSplitter");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CNPC_ShellSplitter::Clone(void* pArg)
{
    CNPC_ShellSplitter* pInstance = new CNPC_ShellSplitter(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CNPC_ShellSplitter");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_ShellSplitter::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
