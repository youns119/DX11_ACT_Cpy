#include "pch.h"
#include "NPC_DanceCrab.h"
#include "GameInstance.h"
#include "CutScene_Manager.h"

CNPC_DanceCrab::CNPC_DanceCrab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CNPC_DanceCrab::CNPC_DanceCrab(const CNPC_DanceCrab& Prototype)
    : super(Prototype)
{
}

HRESULT CNPC_DanceCrab::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_DanceCrab::Initialize(void* pArg)
{
    CGameObject::DESC* pDesc = static_cast<CGameObject::DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pCutSceneManager = static_cast<CCutScene_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CutSceneManager", 0));

    m_iDanceIndex = CUtils::Compute_Random(0, 2);
    m_pModelCom->SetUp_Animation(m_iDanceIndex, true, 0.2f);


    return S_OK;
}

void CNPC_DanceCrab::Priority_Update(_float fTimeDelta)
{
}

void CNPC_DanceCrab::Update(_float fTimeDelta)
{
    if (m_pCutSceneManager->Get_CurPlaying() == L"Dance_Fail")
        m_bAnimStop = true;
    else
    {
        m_bAnimStop = false;

        if (m_pModelCom->Get_CurrAnimationIndex() != m_iDanceIndex)
            m_pModelCom->SetUp_Animation(m_iDanceIndex, true, 0.2f);
    }

    if(!m_bAnimStop)
        m_pModelCom->Play_Animation(fTimeDelta);
}

void CNPC_DanceCrab::Late_Update(_float fTimeDelta)
{
    if (m_pGameInstance->Is_InFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE::POSITION), 60.f))
    {
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
    }
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);
}

HRESULT CNPC_DanceCrab::Render()
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

HRESULT CNPC_DanceCrab::Render_Shadow()
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

HRESULT CNPC_DanceCrab::Ready_Components()
{
    /* Com_Model */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_NPC_DanceCrab",
        L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
        L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

CNPC_DanceCrab* CNPC_DanceCrab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_DanceCrab* pInstance = new CNPC_DanceCrab(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNPC_DanceCrab");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CNPC_DanceCrab::Clone(void* pArg)
{
    CNPC_DanceCrab* pInstance = new CNPC_DanceCrab(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CNPC_DanceCrab");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_DanceCrab::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
