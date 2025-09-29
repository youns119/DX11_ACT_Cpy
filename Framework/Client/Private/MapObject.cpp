
#include "pch.h"
#include "MapObject.h"

#include "GameInstance.h"

CMapObject::CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super{ pDevice, pContext }
{
}

CMapObject::CMapObject(const CMapObject& Prototype)
    : super{ Prototype }
{
}

HRESULT CMapObject::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapObject::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    //pDesc->TransformDesc.fSpeedPerSec = 5.f;
    m_fFrustumRange = pDesc->GameObjectDesc.fSpeedPerSec;//따로 변수 만들기 싫어서 그냥 스피드에 담았어요저장하기도 그나마 쉽고
    m_strName = (pDesc->GameObjectDesc).strName;
    m_iPhysicsState = pDesc->ePhysicsType;

    m_iLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CMapObject::Priority_Update(_float fTimeDelta)
{
}

void CMapObject::Update(_float fTimeDelta)
{

}

void CMapObject::Late_Update(_float fTimeDelta)
{
    
    if (true == m_pGameInstance->Is_InFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE::POSITION), m_fFrustumRange))
    {// 절두체 안에 있을 때만 컬링 

        _float fMaxRange = m_pGameInstance->Get_FogParams().vRange.y;
        super::Compute_CamDistance();

        if (m_iLevelIndex == (_uint)LEVEL_FIELD && fMaxRange > m_fCamDistance)
        {
            m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
#ifdef _DEBUG
            //if (nullptr != m_pPhysicsCom && m_pPhysicsCom->Get_Count() > 0)
            if (nullptr != m_pPhysicsCom && m_pPhysicsCom->Is_Simulated())
            {
                m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pPhysicsCom);
            }

#endif // _DEBUG
        }
        else if (m_iLevelIndex != (_uint)LEVEL_FIELD && fMaxRange + m_fFrustumRange > m_fCamDistance)
        {
            m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
#ifdef _DEBUG
            if (nullptr != m_pPhysicsCom && m_pPhysicsCom->Is_Simulated())
            {
                m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pPhysicsCom);
            }
#endif // _DEBUG
        }
    }

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);
}

HRESULT CMapObject::Render()
{
    if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID"))) // 즁요 ㅋㅋ
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
        return E_FAIL;

    for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
    {
        _uint iMaterialIndex = m_pModelCom->Get_MaterialIndex_In_Mesh(i);
        _uint iShaderPass = m_pModelCom->Get_ShaderPass(iMaterialIndex);

        if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE))
        {
            iShaderPass = (_uint)SHADER_PASS_MESH::DEFAULT;

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
                return E_FAIL;
        }
        else
        {
            iShaderPass = (_uint)SHADER_PASS_MESH::BASE_COLOR;

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

HRESULT CMapObject::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix_Ref())))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_ShadowMatrices_To_Shader(m_pShaderCom, "g_LightViewMatrices", "g_LightProjMatrices")))
        return E_FAIL;

    for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
    {
        m_pShaderCom->Begin((_uint)SHADER_PASS_MESH::SHADOW);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CMapObject::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    _uint iLevelID = LEVEL_STATIC;
    /* Com_Model */
    if (FAILED(__super::Add_Component(iLevelID, m_strName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))

    {
        iLevelID = m_pGameInstance->Get_CurrentLevelIndex();
        if (FAILED(__super::Add_Component(iLevelID, m_strName,
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }

    /* Com_Physics */
    if (PHYSCIS_NONE != m_iPhysicsState)
    {
        CPhysics_Mesh::DESC Desc {};
        Desc.PhysicsDesc.fStaticFriction = 0.8f;
        Desc.PhysicsDesc.fDynamicFriction = 0.7f;
        Desc.PhysicsDesc.fRestitution = 0.2f;
        Desc.PhysicsDesc.isStatic = true;
        Desc.PhysicsDesc.iFilter = (_uint)PHYSICS_FILTER::DEFAULT;
        Desc.PhysicsDesc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());

        Desc.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
        Desc.isInChunk = true;

        const size_t iPrefixLen = wcslen(L"Prototype_Component_Model_");
        const _wstring& strPhysicsPrototypeTag = L"Prototype_Component_Physics_Mesh_" + m_strName.substr(iPrefixLen);

        if (FAILED(Add_Component(iLevelID, strPhysicsPrototypeTag,
            L"Com_Physics", reinterpret_cast<CComponent**>(&m_pPhysicsCom), &Desc)))
        {
#ifdef _DEBUG
            OutputDebugString(_wstring(L"Prototype Not Ready : " + strPhysicsPrototypeTag + L"\n").c_str());
#endif
            return S_OK;
        }
            //MessageBox(nullptr, strPhysicsPrototypeTag.c_str(), L"System Message", MB_OK);
    }

    return S_OK;
}

CMapObject* CMapObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMapObject* pInstance = new CMapObject(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CMapObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CMapObject::Clone(void* pArg)
{
    CMapObject* pInstance = new CMapObject(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CMapObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapObject::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pPhysicsCom);
}
