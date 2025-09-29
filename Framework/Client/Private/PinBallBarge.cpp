#include "pch.h"
#include "PinBallBarge.h"

#include "GameInstance.h"

CPinBallBarge::CPinBallBarge(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : super{ _pDevice, _pContext }
{
}

CPinBallBarge::CPinBallBarge(const CPinBallBarge& _Prototype)
    : super{ _Prototype }
{
}

HRESULT CPinBallBarge::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPinBallBarge::Initialize(void* _pArg)
{
    CGameObject::DESC* pDesc = static_cast<CGameObject::DESC*>(_pArg);
    pDesc->TransformDesc.fSpeedPerSec = 5.f;

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;
    for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
    {
        if (m_pModelCom->Get_MeshName(i) == "eea.001")
        {
            m_iEmissiveMeshIdx = i;
            break;
        }
    }
    return S_OK;
}

void CPinBallBarge::Priority_Update(_float _fTimeDelta)
{
}

void CPinBallBarge::Update(_float _fTimeDelta)
{
    m_fAccTimeDelta += _fTimeDelta;
}

void CPinBallBarge::Late_Update(_float _fTimeDelta)
{
    if (true == m_pGameInstance->Is_InFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE::POSITION), 200.f))
    {// 절두체 안에 있을 때만 컬링 
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
    }
}

HRESULT CPinBallBarge::Render()
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
        //Get_TextureFiles_By_Type
        if (i == m_iEmissiveMeshIdx ) 
        {
            iShaderPass = (_uint)SHADER_PASS_MESH::BASE_COLOR_UV_TRANSFORM;

            //if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexCoordOffset", &)))
            if (m_fAccTimeDelta >= 0.8f)
            {
                m_fAccTimeDelta = 0.f;

                if (m_iY < 8)
                {
                    m_vTexCoord.y += 0.125f;
                    m_iY++;
                }
                else
                {
                    m_iX = (m_iX + 1) % 2;
                    m_vTexCoord.x = (m_iX == 0) ? 0.5f : 1.f;
                    m_vTexCoord.y = 0.125f;
                    m_iY = 0;
                }
            }

            if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexCoordOffset", &m_vTexCoord, sizeof(_float2))))
            {
                MSG_BOX("바인드 실패...");
                return E_FAIL;
            }
        }
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::EMISSIVE, "g_EmissiveMaskTexture")))
                return E_FAIL;
        if (FAILED(m_pModelCom->Bind_EmissionColor(m_pShaderCom, i, "g_vEmissionColor")))
            return E_FAIL;

        m_pShaderCom->Begin(iShaderPass);
        m_pModelCom->Render(i);
    }

    return S_OK;

}

HRESULT CPinBallBarge::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_PINBALL, TEXT("Prototype_Component_Model_PinBallBarge"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    
    /* Com_Physics */
    //if (PHYSCIS_NONE != m_iPhysicsState)
    {
        if (FAILED(Ready_MeshPhysics()))
            return E_FAIL;
    }
     
    return S_OK;
}

HRESULT CPinBallBarge::Ready_MeshPhysics()
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

    if (FAILED(Add_Component(LEVEL_PINBALL, L"Prototype_Component_Physics_Mesh_PinBallBarge",
        L"Com_Physics", reinterpret_cast<CComponent**>(&m_pPhysicsCom), &Desc)))
        return E_FAIL;

    return S_OK;
}

CPinBallBarge* CPinBallBarge::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPinBallBarge* pInstance = new CPinBallBarge(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CPinBallBarge");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CPinBallBarge::Clone(void* _pArg)
{
    CPinBallBarge* pInstance = new CPinBallBarge(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Clone : CPinBallBarge");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPinBallBarge::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pPhysicsCom);
}
