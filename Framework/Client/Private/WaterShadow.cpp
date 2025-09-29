#include "pch.h"
#include "WaterShadow.h"

#include "GameInstance.h"

CWaterShadow::CWaterShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CWaterShadow::CWaterShadow(const CWaterShadow& Prototype)
    : super(Prototype)
{
}

HRESULT CWaterShadow::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CWaterShadow::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
        return E_FAIL; 

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_CausticsParams = pDesc->CauticsParams;

    return S_OK;
}

void CWaterShadow::Priority_Update(_float fTimeDelta)
{
}

void CWaterShadow::Update(_float fTimeDelta)
{
    m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));

    _float& fTime = m_CausticsParams.fTime;
    fTime = fmod(fTime + fTimeDelta, 1000.f);

#ifdef _DEBUG
    //ImGui::Begin("Caustics");

    //ImGui::DragFloat("Intensity", &m_CausticsParams.fIntensity, 0.005f, 0.f, 2.f);

    //ImGui::End();
#endif // _DEBUG
}

void CWaterShadow::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::BLEND, this);

#ifdef _DEBUG
    /* vtx texCUBE 는 로컬 범위가 -1 ~ 1 이다. 따라서 스케일 * 2 를 해야 진짜 크기가 나옴 */
	//_mat DecalBox = _mat::ScalingMatrix({ 2.f, 2.f, 2.f }) * m_pTransformCom->Get_WorldMatrix();
 //   m_pGameInstance->Add_DebugRender_Box(DecalBox, _vec4(1.f, 0.f, 1.f, 1.f));
#endif // _DEBUG
}

HRESULT CWaterShadow::Render()
{
    //if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID")))
    //    return E_FAIL;

    /* Texture */
    if (FAILED(m_pTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_Texture2D")))
        return E_FAIL;
    if (FAILED(m_pTextureCom_Distortion->Bind_Texture_To_Shader(m_pShaderCom, "g_Texture2D_Distortion")))
        return E_FAIL;

    /* RenderTarget */
    if (m_pGameInstance->Bind_RT_To_Shader(L"Target_Normal", m_pShaderCom, "g_NormalTexture"))
        return E_FAIL;
    if (m_pGameInstance->Bind_RT_To_Shader(L"Target_Depth", m_pShaderCom, "g_DepthTexture"))
        return E_FAIL;
    if (m_pGameInstance->Bind_RT_To_Shader(L"Target_ShadowMask_Final", m_pShaderCom, "g_ShadowMaskTexture"))
        return E_FAIL;

    /* Panning */
    if (FAILED(m_pShaderCom->Bind_RawValue("g_CausticsParams", &m_CausticsParams, sizeof(CAUSTICS_PARAMS))))
        return E_FAIL;

    /* Light */
    _vec3 vLightDir = _vec4(m_pGameInstance->Get_LightDesc(L"Light_Global").vDirection);
    vLightDir = vLightDir.Normalized();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_LightParams", &vLightDir, sizeof(_vec3))))
        return E_FAIL;

    /* Matrices*/
    if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->
        Bind_Matrix("g_WorldMatrixInv", _mat(m_pTransformCom->Get_WorldMatrix_Inversed())._float4x4())))
        return E_FAIL;

    if (FAILED(m_pShaderCom->
        Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->
        Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->
        Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORM::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->
        Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORM::PROJ))))
        return E_FAIL;

    m_pShaderCom->Begin(static_cast<_uint>(SHADER_PASS_CUBE::SCREENSPACE_DECAL_CAUSTICS));

    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CWaterShadow::Ready_Component()
{
    /* Com_VIBuffer */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Cube",
        L"Com_VIBuffer", reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Texture_WaterShadow",
        L"Com_Texture", reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* Com_Texture_Distortion */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Texture_WaterCausticsDistortion",
        L"Com_Texture_Distortion", reinterpret_cast<CComponent**>(&m_pTextureCom_Distortion))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxCube",
        L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

CWaterShadow* CWaterShadow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWaterShadow* pInstance = new CWaterShadow(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CWaterShadow");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CWaterShadow::Clone(void* pArg)
{
    CWaterShadow* pInstance = new CWaterShadow(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CWaterShadow");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWaterShadow::Free()
{
    super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTextureCom_Distortion);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
}
