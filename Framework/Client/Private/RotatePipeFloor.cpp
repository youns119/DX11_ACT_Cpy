#include "pch.h"
#include "RotatePipeFloor.h"
#include "GameInstance.h"

CRotatePipeFloor::CRotatePipeFloor(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : super{ _pDevice, _pContext }
{
}

CRotatePipeFloor::CRotatePipeFloor(const CRotatePipeFloor& _Prototype)
    : super{ _Prototype }
{
}

HRESULT CRotatePipeFloor::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRotatePipeFloor::Initialize(void* _pArg)
{
    DESC* pDesc = static_cast<DESC*>(_pArg);
    //pDesc->TransformDesc.fSpeedPerSec = 5.f;1 
    m_strName = (pDesc->GameObjectDesc).strName;

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    XMStoreFloat4x4(&m_OriginMat, m_pTransformCom->Get_WorldMatrix());

    return S_OK;
}

void CRotatePipeFloor::Priority_Update(_float _fTimeDelta)
{
}

void CRotatePipeFloor::Update(_float _fTimeDelta)
{
    if (m_bRender)
    {
        m_fAccTimeDelta += _fTimeDelta;

        _float fRadian = XMConvertToRadians(sinf(m_fAccTimeDelta) * 30.f);
        _matrix RotMat = XMMatrixRotationX(fRadian);
        _matrix OriginMat = XMLoadFloat4x4(&m_OriginMat);
        RotMat = RotMat * OriginMat;

        m_pTransformCom->Set_State(CTransform::STATE::RIGHT, RotMat.r[0]);
        m_pTransformCom->Set_State(CTransform::STATE::UP, RotMat.r[1]);
        m_pTransformCom->Set_State(CTransform::STATE::LOOK, RotMat.r[2]);
        m_pTransformCom->Set_State(CTransform::STATE::POSITION, RotMat.r[3]);

        m_pPhysicsCom->Manipulate_Physics(RotMat);
    }



}

void CRotatePipeFloor::Late_Update(_float _fTimeDelta)
{
    if (true == m_pGameInstance->Is_InFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE::POSITION), 40.f))
    {// 절두체 안에 있을 때만 컬링 

        _float fMaxRange = m_pGameInstance->Get_FogParams().vRange.y;
        super::Compute_CamDistance();

        if (fMaxRange > m_fCamDistance + 40.f)
        {
            m_bRender = true;
            m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
            m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);
#ifdef _DEBUG
            //if (nullptr != m_pPhysicsCom && m_pPhysicsCom->Get_Count() > 0)
            if (nullptr != m_pPhysicsCom && m_pPhysicsCom->Is_Simulated())
            {
                m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pPhysicsCom);
            }
#endif // _DEBUG
        }
    }
    else
        m_bRender = false;

}

HRESULT CRotatePipeFloor::Render()
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

HRESULT CRotatePipeFloor::Render_Shadow()
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

HRESULT CRotatePipeFloor::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Pipe_T_RotateFloor",
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))

    {
        if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), L"Prototype_Component_Model_Pipe_T_RotateFloor",
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }

    {
        CPhysics_Mesh::DESC Desc{};
        Desc.PhysicsDesc.fStaticFriction = 0.8f;
        Desc.PhysicsDesc.fDynamicFriction = 0.7f;
        Desc.PhysicsDesc.fRestitution = 0.2f;
        Desc.PhysicsDesc.isStatic = true;
        Desc.PhysicsDesc.iFilter = (_uint)PHYSICS_FILTER::DEFAULT;
        Desc.PhysicsDesc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());

        Desc.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
        Desc.isInChunk = true;

        if (FAILED(Add_Component(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_Pipe_T_RotateFloor",
            L"Com_Physics", reinterpret_cast<CComponent**>(&m_pPhysicsCom), &Desc)))
            return E_FAIL;
    }
    return S_OK;
}

CRotatePipeFloor* CRotatePipeFloor::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CRotatePipeFloor* pInstance = new CRotatePipeFloor(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CRotatePipeFloor");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CRotatePipeFloor::Clone(void* _pArg)
{
    CRotatePipeFloor* pInstance = new CRotatePipeFloor(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Clone : CRotatePipeFloor");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRotatePipeFloor::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pPhysicsCom);
}
