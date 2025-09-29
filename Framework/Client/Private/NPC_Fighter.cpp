#include "pch.h"
#include "NPC_Fighter.h"

#include "GameInstance.h"
#include "Animation.h"
#include "Player.h"

#include "UI_Manager.h"
#include "CutScene_Manager.h"

#include "CineCamera.h"

#include "Event_NPC_Effect_Inkerton.h"
#include "Event_NPC_Effect_Chitan.h"
#include "NPC_Chitan_KnockBack.h"

CNPC_Fighter::CNPC_Fighter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CNPC_Fighter::CNPC_Fighter(const CNPC_Fighter& Prototype)
    : super(Prototype)
{
}

HRESULT CNPC_Fighter::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Fighter::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    lstrcpy(m_szFightFileName, pDesc->szFightFileName);
    m_strFightModelTag = pDesc->strFightModelTag;

    if (FAILED(Load_Data(*pDesc)))
        return E_FAIL;

    if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_PartObject()))
        return E_FAIL;

    if (FAILED(Ready_Event()))
        return E_FAIL;

    m_pModelCom->SetUp_Animation(0, true, 0.2f);

    m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    m_pCutSceneManager = static_cast<CCutScene_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CutSceneManager", 0));


    return S_OK;
}

void CNPC_Fighter::Priority_Update(_float fTimeDelta)
{
    super::Priority_Update(fTimeDelta);
}

void CNPC_Fighter::Update(_float fTimeDelta)
{
    if (m_pCutSceneManager->Get_CurPlaying() == L"PinBall_Fight")
    {
        m_pUIManager->Set_CutSceneUI_Caption(L"");
        Load_Data();

        if (m_pModelCom->Get_CurrAnimation()->Get_Blendable())
        {
            m_pCutSceneManager->End_CutScene(L"PinBall_Fight");
            m_pCutSceneManager->Begin_CutScene(L"PinBall_End");
        }
    }
    m_pModelCom->Play_Animation(fTimeDelta);

    super::Update(fTimeDelta);
}

void CNPC_Fighter::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

    super::Late_Update(fTimeDelta);
}

HRESULT CNPC_Fighter::Render()
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

HRESULT CNPC_Fighter::Render_Shadow()
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

void CNPC_Fighter::KnockBack_Player()
{
    m_pGameInstance->Play_Sound(L"Chitan_Ha.wav", (_uint)SOUND_CHANNEL::CONVERSATION1, 1.f);
    m_pGameInstance->Play_Sound(L"Inkerton_HaHa.wav", (_uint)SOUND_CHANNEL::CONVERSATION2, 1.f);

    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player"));
    if (pPlayer)
    {
        pPlayer->UnGrab(50.f, IDamaged::DAMAGE::DAMAGE_HEAVY, this);
    }
}

void CNPC_Fighter::Call_InkertonSquid()
{
    if (!lstrcmp(m_strFightModelTag.c_str(), L"Prototype_Component_Model_NPC_Inkerton_Fight"))
    {
        _vector vPos{}, vLook{}, vUp{}, vEffectLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
        vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
        vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK);
        vUp = m_pTransformCom->Get_State(CTransform::STATE::UP);
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENEMY_SPRITE_SQUID, vPos + vLook * 5.f + vUp * 5.f, vEffectLook, false, nullptr);
    }
}

HRESULT CNPC_Fighter::Load_Data(DESC& Desc)
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

HRESULT CNPC_Fighter::Load_Data()
{
    if (m_bLoaded)
        return S_OK;

    Call_InkertonSquid();

    HRESULT hr{};
    DWORD dwByte{};
    HANDLE hFile = CreateFile(m_szFightFileName, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        MSG_BOX("Can Not Open File");
        CloseHandle(hFile);
        return E_FAIL;
    }

    _float4x4 WorldMat = {}; // 월드 행렬
    _bool bRead = ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);

    m_pTransformCom->Set_State(CTransform::STATE::LOOK, XMLoadFloat4x4(&WorldMat).r[(_uint)CTransform::STATE::LOOK]);
    m_pTransformCom->Set_State(CTransform::STATE::UP, XMLoadFloat4x4(&WorldMat).r[(_uint)CTransform::STATE::UP]);
    m_pTransformCom->Set_State(CTransform::STATE::RIGHT, XMLoadFloat4x4(&WorldMat).r[(_uint)CTransform::STATE::RIGHT]);
    m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat4x4(&WorldMat).r[(_uint)CTransform::STATE::POSITION]);

    CloseHandle(hFile);

    Delete_Component(TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom));
    Safe_Release(m_pModelCom);

    CModel::DESC ModelDesc{};
    ModelDesc.pOwner = this;
    Add_Component((_uint)LEVEL_STATIC, m_strFightModelTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &ModelDesc);
    m_pModelCom->SetUp_Animation(0, false, 0.2f);

    m_bLoaded = true;

    return S_OK;
}

HRESULT CNPC_Fighter::Ready_Components(DESC* pDesc)
{
    /* Com_Model */
    CModel::DESC ModelDesc{};
    ModelDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_STATIC, pDesc->strModelTag,
        L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &ModelDesc)))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
        L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Fighter::Ready_Event()
{
    /* Inkerton Effect Event*/
    if (FAILED(Add_Event(L"Effect_Fly", CEvent_NPC_Effect_Inkerton::Create(this, CEvent_NPC_Effect_Inkerton::FLY))))
        return E_FAIL;

    if (FAILED(Add_Event(L"Effect_Charge", CEvent_NPC_Effect_Inkerton::Create(this, CEvent_NPC_Effect_Inkerton::CHARGE))))
        return E_FAIL;

    if (FAILED(Add_Event(L"Effect_Impact", CEvent_NPC_Effect_Inkerton::Create(this, CEvent_NPC_Effect_Inkerton::IMPACT))))
        return E_FAIL;
    
    /* Chitan Effect Event*/
    if (FAILED(Add_Event(L"Effect_SandBurst", CEvent_NPC_Effect_Chitan::Create(this, CEvent_NPC_Effect_Chitan::SANDBURST))))
        return E_FAIL;

    if (FAILED(Add_Event(L"Effect_ImpactDouble", CEvent_NPC_Effect_Chitan::Create(this, CEvent_NPC_Effect_Chitan::IMPACT_DOUBLE))))
        return E_FAIL;

    // CutScene_Player_KnockBack
    if (FAILED(Add_Event(L"CutScene_Player_KnockBack", CNPC_Chitan_KnockBack::Create(this))))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Fighter::Ready_PartObject()
{
    //// CineCamera
    //{
    //    CCineCamera::DESC CineCameraDesc{};

    //    CineCameraDesc.CameraDesc.fFovY = XMConvertToRadians(60.f);
    //    CineCameraDesc.CameraDesc.fNear = 0.1f;
    //    CineCameraDesc.CameraDesc.fFar = 1000.f;

    //    CineCameraDesc.strName = L"Roland_Camera";
    //    CineCameraDesc.pOwner = this;

    //    if (FAILED(Add_PartObject(L"Part_CineCamera", LEVEL_STATIC, L"Prototype_GameObject_CineCamera", &CineCameraDesc)))
    //        return E_FAIL;

    //    m_pCineCamera = static_cast<CCineCamera*>(Find_PartObject({ L"Part_CineCamera" }));
    //    m_pCineCamera->Add_Cinematic(L"Roland", "../Bin/DataFiles/CameraData/Roland_Death.data");
    //    m_pCineCamera->Add_Cinematic(L"Pinball1", "../Bin/DataFiles/CameraData/Roland_Death_1.data");
    //    m_pCineCamera->Add_Cinematic(L"Pinball2", "../Bin/DataFiles/CameraData/Roland_Death_2.data");
    //    m_pCineCamera->Add_Cinematic(L"Pinball3", "../Bin/DataFiles/CameraData/Roland_Death_3.data");
    //    m_pCineCamera->Add_Cinematic(L"Pinball4", "../Bin/DataFiles/CameraData/Roland_Death_4.data");
    //}

    return S_OK;
}

CNPC_Fighter* CNPC_Fighter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_Fighter* pInstance = new CNPC_Fighter(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNPC_Fighter");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CNPC_Fighter::Clone(void* pArg)
{
    CNPC_Fighter* pInstance = new CNPC_Fighter(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CNPC_Fighter");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_Fighter::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
