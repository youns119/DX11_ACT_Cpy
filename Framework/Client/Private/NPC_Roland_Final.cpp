#include "pch.h"
#include "NPC_Roland_Final.h"

#include "GameInstance.h"
#include "Animation.h"

#include "UI_Manager.h"
#include "CutScene_Manager.h"
#include "CineCamera.h"

CNPC_Roland_Final::CNPC_Roland_Final(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CNPC_Roland_Final::CNPC_Roland_Final(const CNPC_Roland_Final& Prototype)
    : super(Prototype)
{
}

HRESULT CNPC_Roland_Final::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Roland_Final::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    if (FAILED(Load_Data(*pDesc)))
        return E_FAIL;

    if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->SetUp_Animation((_uint)ANIM_TALK, true, 0.2f);
    m_pModelCom->Play_Animation(1.f / 200.f);

    m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    m_pCutSceneManager = static_cast<CCutScene_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CutSceneManager", 0));

    return S_OK;
}

void CNPC_Roland_Final::Priority_Update(_float fTimeDelta)
{
    super::Priority_Update(fTimeDelta);
}

void CNPC_Roland_Final::Update(_float fTimeDelta)
{
    if (m_pCutSceneManager->Get_CurPlaying() == L"CrabTomb_Start" || m_pCutSceneManager->Get_CurPlaying() == L"CrabTomb_Roland_Die")
        m_isRender = true;

    if (m_pCutSceneManager->Get_CurPlaying() == L"CrabTomb_Roland_Die")
    {
        // 원래는 죽는 애니메이션 넣고 사라지면 
        m_fFrame += fTimeDelta;

        if (m_fFrame >= 3.f)
        {
            m_fFrame = 0.f;
            m_pCutSceneManager->End_CutScene(L"CrabTomb_Roland_Die");
            m_pCutSceneManager->Begin_CutScene(L"CrabTomb_End");
            m_isRender = false;
        }
    }

    if (!m_isRender)
        return;

    if (!m_pGameInstance->IsPlaying((_uint)SOUND_CHANNEL::CONVERSATION2))
    {
        if ((_uint)ANIM_IDLE != m_pModelCom->Get_CurrAnimationIndex())
            m_pModelCom->SetUp_Animation((_uint)ANIM_IDLE, true, 0.2f);
    }
    else
    {
        if ((_uint)ANIM_TALK != m_pModelCom->Get_CurrAnimationIndex())
            m_pModelCom->SetUp_Animation((_uint)ANIM_TALK, true, 0.2f);
    }

    m_pModelCom->Play_Animation(fTimeDelta);

    super::Update(fTimeDelta);
}

void CNPC_Roland_Final::Late_Update(_float fTimeDelta)
{
    if (m_isRender)
    {
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);
    }

    super::Late_Update(fTimeDelta);
}

HRESULT CNPC_Roland_Final::Render()
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

HRESULT CNPC_Roland_Final::Render_Shadow()
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

HRESULT CNPC_Roland_Final::Load_Data(DESC& Desc)
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

HRESULT CNPC_Roland_Final::Ready_Components()
{
    /* Com_Model */
    CModel::DESC ModelDesc{};
    ModelDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_NPC_Roland",
        L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &ModelDesc)))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
        L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

CNPC_Roland_Final* CNPC_Roland_Final::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_Roland_Final* pInstance = new CNPC_Roland_Final(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNPC_Roland_Final");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CNPC_Roland_Final::Clone(void* pArg)
{
    CNPC_Roland_Final* pInstance = new CNPC_Roland_Final(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CNPC_Roland_Final");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_Roland_Final::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
