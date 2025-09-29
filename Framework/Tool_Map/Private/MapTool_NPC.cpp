#include "pch.h"
#include "MapTool_NPC.h"
#include "GameInstance.h"

CMapTool_NPC::CMapTool_NPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CMapTool_NPC::CMapTool_NPC(const CMapTool_NPC& Prototype)
    : super(Prototype)
{
}

HRESULT CMapTool_NPC::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapTool_NPC::Initialize(void* pArg)
{
    CGameObject::DESC* pDesc = static_cast<CGameObject::DESC*>(pArg);

    if (nullptr == pDesc)
        return E_FAIL;

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc->strName)))
        return E_FAIL;

    m_pModelCom->SetUp_Animation(0, true, 0.2f);

    return S_OK;
}

void CMapTool_NPC::Priority_Update(_float fTimeDelta)
{
    super::Priority_Update(fTimeDelta);
}

void CMapTool_NPC::Update(_float fTimeDelta)
{
    m_pModelCom->Play_Animation(fTimeDelta);
    super::Update(fTimeDelta);
}

void CMapTool_NPC::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
    super::Late_Update(fTimeDelta);
}

HRESULT CMapTool_NPC::Render()
{
    if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID")))
        return E_FAIL;

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
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
        {
            iShaderPass = (_uint)SHADER_PASS_ANIMMESH::BASE_COLOR; // pass base color;
            m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor");
        }
        else if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::NORMAL))
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::NORMAL, "g_NormalTexture")))
                return E_FAIL;
        }
        else
        {
            iShaderPass = (_uint)SHADER_PASS_ANIMMESH::NON_NORMAL_MAPPING;
        }

        m_pShaderCom->Begin(iShaderPass);
        m_pModelCom->Render(i);
    }

    return S_OK;
}


HRESULT CMapTool_NPC::Ready_Components(const _wstring& strModelTag)
{
    /* Com_Model */
    if (FAILED(Add_Component(LEVEL_STATIC, strModelTag,
        L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
        L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}


CMapTool_NPC* CMapTool_NPC::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMapTool_NPC* pInstance = new CMapTool_NPC(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CMapTool_NPC");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CMapTool_NPC::Clone(void* pArg)
{
    CMapTool_NPC* pInstance = new CMapTool_NPC(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CMapTool_NPC");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapTool_NPC::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
