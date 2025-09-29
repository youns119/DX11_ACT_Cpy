#include "Renderer.h"

#include "GameObject.h"
#include "GameInstance.h"
#include "DebugRender.h"

#include "RenderPass_SSAO.h"
#include "RenderPass_Bloom.h"
#include "RenderPass_RadialBlur.h"
#include "RenderPass_ShadowMask.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
    , m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderer::Initialize()
{
    _uint iNumViewports = { 1 };
    D3D11_VIEWPORT ViewportDesc {};

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
    _uint iViewportWidth = (_uint)ViewportDesc.Width;
    _uint iViewportHeight = (_uint)ViewportDesc.Height;
    /* ���۵� �������� ���� ����Ÿ�ٵ��� �߰��Ѵ�. */

    m_iOriginViewportWidth = iViewportWidth;
    m_iOriginViewportHeight = iViewportHeight;

    if (FAILED(Ready_RenderTargets()))
        return E_FAIL;

    if (FAILED(Ready_MRTs()))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_RenderPasses()))
        return E_FAIL;

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f)); // �������� ����Ʈ�� ������ �׸� �༮
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

#ifdef _DEBUG
    if (FAILED(Ready_DebugRenders()))
        return E_FAIL;

    if (FAILED(Ready_DebugDepthStencilState()))
        return E_FAIL;
#endif

    return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eGroup, CGameObject* pGameObject)
{
    if (nullptr == pGameObject)
        return E_FAIL;

    m_RenderObjects[(_uint)eGroup].push_back(pGameObject);
    Safe_AddRef(pGameObject);

    return S_OK;
}

HRESULT CRenderer::Render()
{
    PROFILE_SCOPE_TAG;
    _wstring strFinalTag;
    _wstring strMRTTag;

    if (FAILED(Render_Priority()))
        return E_FAIL;

    if (FAILED(Render_Shadow()))
        return E_FAIL;

    if (FAILED(Render_NonBlend()))
        return E_FAIL;

    if (FAILED(Render_ShadowMask()))
        return E_FAIL;

    if (FAILED(Render_LightAcc()))
        return E_FAIL;

    if (FAILED(Render_Deferred()))
        return E_FAIL;

    if (FAILED(Render_NonLight()))
        return E_FAIL;

    strMRTTag = L"MRT_Final";
    strFinalTag = L"Target_Final";

    if (FAILED(Render_SSAO(strMRTTag, strFinalTag)))
        return E_FAIL;

    if (FAILED(Render_Blend(strMRTTag, strFinalTag)))
        return E_FAIL;

    /* g-buffer�� ��� �������� �Ǿ� �־�� �� */
    if (FAILED(Render_Fog(strFinalTag)))
        return E_FAIL;

    if (FAILED(Render_Blend_Effect()))
        return E_FAIL;

    if (FAILED(Render_Blend_Weighted()))
        return E_FAIL;

    if (FAILED(Render_Distortion()))
        return E_FAIL;

    if (FAILED(Render_Effect_Final(strFinalTag)))
        return E_FAIL;

    if (FAILED(Render_PostProcess_Distortion(strFinalTag)))
        return E_FAIL;

    if (FAILED(Render_PostProcess_Bloom(strFinalTag)))
        return E_FAIL;

    if (FAILED(Render_PostProcess_ColorCorrection(strFinalTag)))
        return E_FAIL;

    if (FAILED(Render_PostProcess_RadialBlur()))
        return E_FAIL;

    if (FAILED(Render_PostProcess_Final()))
        return E_FAIL;

    if (m_isRenderBlur)
    {
        if (FAILED(Render_Blur()))
            return E_FAIL;
    }

    if (FAILED(Render_UI_Anim()))
        return E_FAIL;

    if (FAILED(Render_UI()))
        return E_FAIL;

#ifdef _DEBUG

    if (m_pGameInstance->Key_Down(KEY::F3))
        m_iDebugMRTID = ++m_iDebugMRTID % m_DebugMRTTagsByIndex.size();

	if (FAILED(m_pDebugShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDebugShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
		return E_FAIL;
    for (auto& strMRTTag : m_DebugMRTTagsByIndex[m_iDebugMRTID])
    {
        if (FAILED(m_pGameInstance->Render_MRT_Debug(strMRTTag, m_pDebugShaderCom, m_pVIBufferCom)))
            return E_FAIL;
    }

    static _bool s_isRenderDebug = false;
    if (m_pGameInstance->Key_Down(KEY::F2))
        s_isRenderDebug = !s_isRenderDebug;

    if (s_isRenderDebug)
    {
        if (FAILED(Render_Debug()))
            return E_FAIL;
    }
    m_pDebugRender->Clear();
#endif // _DEBUG


    Clear();

    return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugRenderComponent(DEBUGRENDERGROUP eGroup, CComponent* pDebugCom)
{
    m_DebugComponents[(_uint)eGroup].push_back(pDebugCom);
    Safe_AddRef(pDebugCom);

    return S_OK;
}

#pragma region DEBUG_RENDER
void CRenderer::Add_DebugRender_Sphere(DirectX::BoundingSphere Sphere, _float4 vColor)
{
    m_pDebugRender->Add_DebugRender_Sphere(Sphere, vColor);
}
void CRenderer::Add_DebugRender_Triangle(_float3 Points[3], _float4 vColor)
{
    m_pDebugRender->Add_DebugRender_Triangle(Points, vColor);
}
void CRenderer::Add_DebugRender_Quad(_vec3 Points[4], _vec4 vColor)
{
    m_pDebugRender->Add_DebugRender_Quad(Points, vColor);
}
void CRenderer::Add_DebugRender_Box(_mat WorldMatrix, _vec4 vColor)
{
    m_pDebugRender->Add_DebugRender_Box(WorldMatrix, vColor);
}
void CRenderer::Add_DebugRender_Ray(_vec3 vOrigin, _vec3 vDir, _bool isNomalize, _vec4 vColor)
{
    m_pDebugRender->Add_DebugRender_Ray(vOrigin, vDir, isNomalize, vColor);
}
void CRenderer::Add_DebugRender_Grid(_vec3 vOrigin, _vec3 vAxisX, _vec3 vAxisY, _float fUnitX, _float fUnitY, _vec4 vColor)
{
    m_pDebugRender->Add_DebugRender_Grid(vOrigin, vAxisX, vAxisY, fUnitX, fUnitY, vColor);
}
void CRenderer::Add_DebugRender_Capsule(_vec3 vPosition, _vec3 vDirection, _float fRadius, _float fHeight, _float4 vColor)
{
    m_pDebugRender->Add_DebugRender_Capsule(vPosition, vDirection, fRadius, fHeight, vColor);
}
void CRenderer::Add_DebugRender_Function(function<HRESULT()> RenderFunction)
{
    m_pDebugRender->Add_DebugRender_Function(RenderFunction);
}
#pragma endregion
#endif // _DEBUG

HRESULT CRenderer::Render_Priority()
{
    // �������� Render, Release �� �ݺ��ϼ����� ���� �� Render ���� �� Release �غ�����
    // ����� �� ����׳� �̷� ���� ȯ�濡 �־ ������������ ��

    //for (auto& pRenderObject : m_RenderObjects[(_uint)RENDERGROUP::PRIORITY])
    //{
    //    if (FAILED(pRenderObject->Render()))
    //        return E_FAIL;
    // 
    //    Safe_Release(pRenderObject);
    //}
    //m_RenderObjects[(_uint)RENDERGROUP::PRIORITY].clear();

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Final"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[(_uint)RENDERGROUP::PRIORITY])
    {
        if (FAILED(pRenderObject->Render()))
            return E_FAIL;
    }

    m_pGameInstance->End_MRT();

    return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
    //// ����Ʈ�� depth stencil buffer �� ũ�Ⱑ ���ƾ��Ѵ�.
    //if (FAILED(SetUp_Viewport(g_iMaxWidth, g_iMaxHeight)))
    //    return E_FAIL;

    /* shadow*/
    if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Shadow", true, m_pShadowDSV)))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[(_uint)RENDERGROUP::SHADOW])
    {
        if (FAILED(pRenderObject->Render_Shadow()))
            return E_FAIL;
    }

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    //if (FAILED(SetUp_Viewport(m_iOriginViewportWidth, m_iOriginViewportHeight)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_ShadowMask()
{
    CRenderPass_ShadowMask::RENDERPASS_PARAMS Params {};
    Params.strFinalMRTTag = L"MRT_ShadowMask_Final";

    Params.strNormalSRVTag = L"Target_Normal";
    Params.strDepthSRVTag = L"Target_Depth";
    Params.strShadowMapSRVTag = L"Target_Shadow";

    Params.pVIBuffer = m_pVIBufferCom;
    Params.pWorldMatrix = &m_WorldMatrix;
    Params.pViewMatrix = &m_ViewMatrix;
    Params.pProjMatrix = &m_ProjMatrix;

    Params.pParams = &m_ShadowMaskParams;
    Params.pBilateralBlueParams = &m_BilateralBlurParams;

    Params.isHardShadow = m_RenderOption.isHardShadow;

    if (FAILED(m_pRenderPass_ShadowMask->Render(Params)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
    /* Diffuse + Normal */
    if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_GameObjects")))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[(_uint)RENDERGROUP::NONBLEND])
    {
        if (FAILED(pRenderObject->Render()))
            return E_FAIL;
    }

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Final_Forward"), false)))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[(_uint)RENDERGROUP::NONLIGHT])
    {
        if (FAILED(pRenderObject->Render()))
            return E_FAIL;
    }

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_SSAO(_wstring& strMRTTag, _wstring& strFinalTag)
{
    if (true == m_RenderOption.isSkipSSAO)
        return S_OK;

    CRenderPass_SSAO::RENDERPASS_PARAMS Params {};
    Params.strFinalMRTTag = L"MRT_SSAO_Final";
    Params.strFinalSRVTag = strFinalTag;
    Params.strNormalSRVTag = L"Target_Normal";
    Params.strDepthSRVTag = L"Target_Depth";
    Params.pVIBuffer = m_pVIBufferCom;
    Params.pSSAOParams = &m_SSAOParams;
    Params.pWorldMatrix = &m_WorldMatrix;
    Params.pViewMatrix = &m_ViewMatrix;
    Params.pProjMatrix = &m_ProjMatrix;

    if (FAILED(m_pRenderPass_SSAO->Render(Params)))
        return E_FAIL;

    strMRTTag = L"MRT_SSAO_Final";
    strFinalTag = L"Target_SSAO_Final";

    return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
    /* Shade */
    if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_LightAcc")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_Diffuse", m_pLightingShaderCom, "g_DiffuseTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_Normal", m_pLightingShaderCom, "g_NormalTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_Depth", m_pLightingShaderCom, "g_DepthTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_MtrlProperty", m_pLightingShaderCom, "g_MtrlProperty")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_ShadowMask_Final", m_pLightingShaderCom, "g_ShadowMaskTexture")))
        return E_FAIL;

    m_pLightingShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pLightingShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pLightingShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    m_pLightingShaderCom->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORM::VIEW));
    m_pLightingShaderCom->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORM::PROJ));

    if (FAILED(m_pLightingShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    m_pGameInstance->Render_Lights(m_pLightingShaderCom, m_pVIBufferCom); // LightManager -> Render_Lights

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}


HRESULT CRenderer::Render_Deferred()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Final"), false)))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_Diffuse", m_pShaderCom, "g_DiffuseTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_Shade", m_pShaderCom, "g_ShadeTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_Ambient", m_pShaderCom, "g_AmbientTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_MtrlProperty", m_pShaderCom, "g_MtrlProperty")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_EmissionColor", m_pShaderCom, "g_EmissionColor")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_ShadowMask_Final", m_pShaderCom, "g_SourTexture")))
        return E_FAIL;

    if (FAILED(Bind_Matrices(m_pShaderCom)))
        return E_FAIL;

    m_pShaderCom->Begin((_uint)SHADER_PASS_DEFERRED::DEFFERED);

    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    m_pGameInstance->End_MRT(); // always return S_OK;

    return S_OK;
}

HRESULT CRenderer::Render_PostProcess_EdgeDetection()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_EdgeDetection"))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(TEXT("Target_Normal"), m_pShaderCom, "g_NormalTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(TEXT("Target_EffectFinal"), m_pShaderCom, "g_FinalTexture")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pShaderCom->Begin((_uint)SHADER_PASS_DEFERRED::EDGE_DETECTION);

    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Fog(_wstring& strFinalTag)
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Fog"))))
        return E_FAIL;

    // �ӽ�
    if (FAILED(m_pShaderCom->Bind_RawValue("g_FogParams", &m_FogParams, sizeof(FOG_PARAMS))))
        return E_FAIL;
    m_pShaderCom->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORM::VIEW));
    m_pShaderCom->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORM::PROJ));

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(strFinalTag, m_pShaderCom, "g_FinalTexture")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pShaderCom->Begin((_uint)SHADER_PASS_DEFERRED::FOG);

    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    strFinalTag = L"Target_Fog";

    return S_OK;
}

HRESULT CRenderer::Render_PostProcess_Distortion(_wstring& strFinalTag)
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Post_Distortion"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(TEXT("Target_Distortion"), m_pShaderCom, "g_DistortionTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(strFinalTag, m_pShaderCom, "g_FinalTexture")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pShaderCom->Begin((_uint)SHADER_PASS_DEFERRED::POSTPROCESS_DISTORTION);

    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    strFinalTag = L"Target_Post_Distortion";

    return S_OK;
}

HRESULT CRenderer::Render_PostProcess_Bloom(_wstring& strFinalTag)
{
    if (true == m_RenderOption.isSkipBloom)
        return S_OK;

    CRenderPass_Bloom::RENDERPASS_PARAMS Params {};
    Params.strFinalMRTTag = L"MRT_Bloom";
    Params.strFinalSRVTag = strFinalTag;
    Params.pVIBuffer = m_pVIBufferCom;
    Params.pBloomParams = &m_BloomParams;
    Params.pWorldMatrix = &m_WorldMatrix;
    Params.pViewMatrix = &m_ViewMatrix;
    Params.pProjMatrix = &m_ProjMatrix;

    if (FAILED(m_pRenderPass_Bloom->Render(Params)))
        return E_FAIL;

    strFinalTag = L"Target_Bloom";

    return S_OK;
}

HRESULT CRenderer::Render_PostProcess_ColorCorrection(_wstring& strFinalTag)
{
    if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_ColorCorrection")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(strFinalTag, m_pShaderCom, "g_FinalTexture")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorCorrectionParams", &m_ColorCorrectionParams, sizeof(m_ColorCorrectionParams))))
        return E_FAIL;

    if (FAILED(Bind_Matrices(m_pShaderCom)))
        return E_FAIL;

    m_pShaderCom->Begin((_uint)SHADER_PASS_DEFERRED::COLOR_CORRECTION);

    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    strFinalTag = L"Target_ColorCorrection";

    return S_OK;
}

HRESULT CRenderer::Render_PostProcess_RadialBlur()
{
    /* Convert Center posistion : Screen to TexCoord */
    RADIALBLUR_PARAMS ConvertedParams;
    memcpy(&ConvertedParams, &m_RadialBlurParams, sizeof(RADIALBLUR_PARAMS));
    ConvertedParams.vCenter.x = m_RadialBlurParams.vCenter.x / m_iOriginViewportWidth;
    ConvertedParams.vCenter.y = m_RadialBlurParams.vCenter.y / m_iOriginViewportHeight;

    CRenderPass_RadialBlur::RENDERPASS_PARAMS Params;
    Params.strFinalMRTTag = L"MRT_RadialBlur";
    Params.strFinalSRVTag = L"Target_ColorCorrection";
    Params.pVIBuffer = m_pVIBufferCom;
    Params.pWorldMatrix = &m_WorldMatrix;
    Params.pViewMatrix = &m_ViewMatrix;
    Params.pProjMatrix = &m_ProjMatrix;
    Params.pRadialBlurParams = &ConvertedParams;

    return m_pRenderPass_RadialBlur->Render(Params);
}

HRESULT CRenderer::Render_PostProcess_Final()
{
    // render to bakc buffer -> begin_MRT() ���ص��� ��¥�� ���� �н����� End_MRT �آZ��
    //if (FAILED(m_pGameInstance->Bind_RT_To_Shader(TEXT("Target_Final"), m_pShaderCom, "g_FinalTexture")))
    //    return E_FAIL;

    if (m_isRenderBlur)
    {
        if (FAILED(m_pGameInstance->Bind_RT_To_Shader(TEXT("Target_Blur"), m_pShaderCom, "g_FinalTexture")))
            return E_FAIL;
    }
    else
    {
        if (FAILED(m_pGameInstance->Bind_RT_To_Shader(TEXT("Target_RadialBlur"), m_pShaderCom, "g_FinalTexture")))
            return E_FAIL;
    }
   
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pShaderCom->Begin((_uint)SHADER_PASS_DEFERRED::POSTPROCESS_FINAL);

    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CRenderer::Render_Blend(_wstring& strMRTTag, _wstring& strFinalTag)
{
    m_RenderObjects[(_uint)RENDERGROUP::BLEND].sort([](const CGameObject* pSour, const CGameObject* pDest)->_bool
        {
            return pSour->Get_CamDistance() > pDest->Get_CamDistance();
        });

    if (FAILED(m_pGameInstance->Begin_MRT(strMRTTag, false)))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[(_uint)RENDERGROUP::BLEND])
    {
        if (FAILED(pRenderObject->Render()))
            return E_FAIL;
    }

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;
    return S_OK;
}

HRESULT CRenderer::Render_Blend_Effect()
{
    m_RenderObjects[(_uint)RENDERGROUP::BLEND].sort([](const CGameObject* pSour, const CGameObject* pDest)->_bool
        {
            return pSour->Get_CamDistance() > pDest->Get_CamDistance();
        });

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Effect"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[(_uint)RENDERGROUP::EFFECTBLEND])
    {
        if (FAILED(pRenderObject->Render()))
            return E_FAIL;
    }

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;
    return S_OK;
}

HRESULT CRenderer::Render_Blend_Weighted()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_WeightedBlendAcc"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[(_uint)RENDERGROUP::WEIGHTEDBLEND])
    {
        if (FAILED(pRenderObject->Render()))
            return E_FAIL;
    }

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Revealage"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[(_uint)RENDERGROUP::WEIGHTEDBLEND])
    {
        if (FAILED(pRenderObject->Render_Revealage()))
            return E_FAIL;
    }

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Distortion"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[(_uint)RENDERGROUP::DISTORTION])
    {
        if (FAILED(pRenderObject->Render_Distortion()))
            return E_FAIL;
    }

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Effect_Final(_wstring& strFinalTag)
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_EffectFinal"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(strFinalTag, m_pShaderCom, "g_FinalTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(TEXT("Target_Effect"), m_pShaderCom, "g_EffectTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(TEXT("Target_TransparencyAcc"), m_pShaderCom, "g_BlendAccTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(TEXT("Target_Revealage"), m_pShaderCom, "g_BlendRevealTexture")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pShaderCom->Begin((_uint)SHADER_PASS_DEFERRED::BLEND_EFFECT);

    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    strFinalTag = L"Target_EffectFinal";

    return S_OK;
}


HRESULT CRenderer::Render_Blur()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(TEXT("Target_ColorCorrection"), m_pShaderCom, "g_FinalTexture")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pShaderCom->Begin((_uint)SHADER_PASS_DEFERRED::UIBLUR);

    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_UI_Anim()
{
    m_RenderObjects[(_uint)RENDERGROUP::UI_ANIM].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
        {
            _vec3 vSourPos = static_cast<CTransform*>(pSour->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);
            _vec3 vDestPos = static_cast<CTransform*>(pDest->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);

            return vSourPos.z > vDestPos.z;
        });

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_UI"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[(_uint)RENDERGROUP::UI_ANIM])
    {
        if (FAILED(pRenderObject->Render()))
            return E_FAIL;
    }

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_UI()
{
    m_RenderObjects[(_uint)RENDERGROUP::UI].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
        {
            _vec3 vSourPos = static_cast<CTransform*>(pSour->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);
            _vec3 vDestPos = static_cast<CTransform*>(pDest->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);

            return vSourPos.z > vDestPos.z;
        });

    for (auto& pRenderObject : m_RenderObjects[(_uint)RENDERGROUP::UI])
    {
        if (FAILED(pRenderObject->Render()))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CRenderer::Ready_DepthStencilBuffer(_uint iWidth, _uint iHeight, ID3D11DepthStencilView** ppOut, _uint iArraySize)
{
    ID3D11Texture2D* pDepthStencilTexture = nullptr;

    D3D11_TEXTURE2D_DESC	TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    /* ���� ������ �ȼ��� ��������� �ȼ��� ������ �����ؾ߸� ���� �ؽ�Ʈ�� ����������. */
    /* �ȼ��� ���� �ٸ��� �ƿ� �������� ����. */
    TextureDesc.Width = iWidth;
    TextureDesc.Height = iHeight;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = iArraySize;
    TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, ppOut)))
        return E_FAIL;

    Safe_Release(pDepthStencilTexture);

    return S_OK;
}

HRESULT CRenderer::SetUp_Viewport(_uint iWidth, _uint iHeight)
{
    D3D11_VIEWPORT			ViewPortDesc;
    ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
    ViewPortDesc.TopLeftX = 0;
    ViewPortDesc.TopLeftY = 0;
    ViewPortDesc.Width = (_float)iWidth;
    ViewPortDesc.Height = (_float)iHeight;
    ViewPortDesc.MinDepth = 0.f;
    ViewPortDesc.MaxDepth = 1.f;

    m_pContext->RSSetViewports(1, &ViewPortDesc);

    return S_OK;
}

HRESULT CRenderer::Ready_Components()
{
    m_pShaderCom = CShader::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Deferred.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    m_pLightingShaderCom = CShader::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Lighting.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pLightingShaderCom)
        return E_FAIL;

    m_pVIBufferCom = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
    if (nullptr == m_pShaderCom)
        return E_FAIL;

#ifdef _DEBUG
    m_pDebugShaderCom = CShader::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Debug.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pDebugShaderCom)
        return E_FAIL;
#endif // _DEBUG

    return S_OK;
}

HRESULT CRenderer::Ready_RenderTargets()
{
    _uint iViewportWidth = m_iOriginViewportWidth;
    _uint iViewportHeight = m_iOriginViewportHeight;

    /* MRT : GameObject ( G-Buffer ) */
    /* Target_Diffuse */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Diffuse", iViewportWidth, iViewportHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    /* Target_Normal */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Normal", iViewportWidth, iViewportHeight, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    /* Target_Depth */
    // ���� ���� Ŭ�󿡼� �������ִ� far ���� ����ϴ°� �ƴ϶� �������� �ϵ��ڵ����� �׳� �ʱⰪ�� �ڰ� �ִ� �̸� �ذ��ϱ� ���� ������..
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Depth", iViewportWidth, iViewportHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1000.f, 0.f, 0.f))))
        return E_FAIL;
    /* Target_MtrlProperty */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_MtrlProperty", iViewportWidth, iViewportHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    /* Target_EmissionColor */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_EmissionColor", iViewportWidth, iViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    /* Target_ObjectID */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_ObjectID", iViewportWidth, iViewportHeight, DXGI_FORMAT_R32_UINT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* MRT : LightAcc */
    /* Target_Shade */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Shade", iViewportWidth, iViewportHeight, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    /* Target_Ambient */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Ambient", iViewportWidth, iViewportHeight, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_Effect = Effect Diffuse + Effect Emission */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Effect", iViewportWidth, iViewportHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_Distortion */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Distortion", iViewportWidth, iViewportHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_TransparencyAcc */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_TransparencyAcc", iViewportWidth, iViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_Revealage */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Revealage", iViewportWidth, iViewportHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_EffectFinal */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_EffectFinal", iViewportWidth, iViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* MRT : Shadow */
    const _uint iNumCasCade = 3;
    /* Target_Shadow */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Shadow", m_iShadowMapWidth, m_iShadowMapHeight, DXGI_FORMAT_R32_FLOAT, _float4(1.f, 0.f, 0.f, 0.f), iNumCasCade)))
        //Add_RenderTarget(L"Target_Shadow", m_iShadowMapWidth, m_iShadowMapHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1000.f, 1.f, 0.f, 0.f), iNumCasCade)))
        return E_FAIL;
    if (FAILED(Ready_DepthStencilBuffer(m_iShadowMapWidth, m_iShadowMapHeight, &m_pShadowDSV, iNumCasCade)))
        return E_FAIL;

    /* Target_ShadowMask_Final */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_ShadowMask_Final", iViewportWidth, iViewportHeight, DXGI_FORMAT_R8_UNORM, _float4(1.f, 0.f, 0.f, 0.f))))
        return E_FAIL;


    /* MRT : Final ( Deferred ) */
    /* Target_Fianl */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Final", iViewportWidth, iViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.63f, 0.83f, 0.98f, 1.0f))))
        return E_FAIL;

    /* MRT : PostProcess */

    /* Target_SSAO_Final */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_SSAO_Final", iViewportWidth, iViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_Fog */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Fog", iViewportWidth, iViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_Bloom */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Bloom", iViewportWidth, iViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_Post_Distortion */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Post_Distortion", iViewportWidth, iViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_ColorCorrection */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_ColorCorrection", iViewportWidth, iViewportHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Ready_RenderTarget */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_RadialBlur", iViewportWidth, iViewportHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_Blur */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Blur", iViewportWidth, iViewportHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_UI */
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_UI", iViewportWidth, iViewportHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Ready_MRTs()
{
    /* MRT_GameObjects */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_MtrlProperty"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_EmissionColor"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_ObjectID"))))
        return E_FAIL;

    /* MRT_LightAcc */
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_LightAcc", L"Target_Shade")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_LightAcc", L"Target_Ambient")))
        return E_FAIL;

    /* MRT_Effect */
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Effect", L"Target_Effect")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Effect", L"Target_Depth")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Distortion", L"Target_Distortion")))
        return E_FAIL;

    /* MRT_WeightedBlend */
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_WeightedBlendAcc", L"Target_TransparencyAcc")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Revealage", L"Target_Revealage")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_EffectFinal", L"Target_EffectFinal")))
        return E_FAIL;

    /* MRT_Shadow */
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Shadow", L"Target_Shadow")))
        return E_FAIL;

    /* MRT_ShadowMask_Final */
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_ShadowMask_Final", L"Target_ShadowMask_Final")))
        return E_FAIL;

    // ���۵� -> ����Ʈ -> �Ȱ� -> ��� ( ������ ã�� -> ���� -> ���ϱ� )

    /* MRT_Final_Forward */ // Non_Light ���� ����� 
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Final_Forward", L"Target_Final")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Final_Forward", L"Target_Depth")))
        return E_FAIL;

    /* MRT_Final */
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Final", L"Target_Final")))
        return E_FAIL;

    /* MRT_Post_Distortion */
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Post_Distortion", L"Target_Post_Distortion")))
        return E_FAIL;

    /* MRT_SSAO_Final */
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_SSAO_Final", L"Target_SSAO_Final")))
        return E_FAIL;

    /* MRT_Fog */
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Fog", L"Target_Fog")))
        return E_FAIL;

    /* MRT_Bloom */
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Bloom", L"Target_Bloom")))
        return E_FAIL;

    /* MRT_ColorCorrection */
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_ColorCorrection", L"Target_ColorCorrection")))
        return E_FAIL;

    /* Ready_MRT */
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_RadialBlur", L"Target_RadialBlur")))
        return E_FAIL;

    /* MRT_Blur*/
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Blur", L"Target_Blur")))
        return E_FAIL;

    /* MRT_UI */
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_UI", L"Target_UI")))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Ready_RenderPasses()
{
    m_pRenderPass_SSAO = CRenderPass_SSAO::Create(m_pDevice, m_pContext, _uint2(m_iOriginViewportWidth, m_iOriginViewportHeight));
    if (nullptr == m_pRenderPass_SSAO)
        return E_FAIL;

    m_pRenderPass_Bloom = CRenderPass_Bloom::Create(m_pDevice, m_pContext, _uint2(m_iOriginViewportWidth, m_iOriginViewportHeight));
    if (nullptr == m_pRenderPass_Bloom)
        return E_FAIL;

    m_pRenderPass_RadialBlur = CRenderPass_RadialBlur::Create(m_pDevice, m_pContext, _uint2(m_iOriginViewportWidth, m_iOriginViewportHeight));
    if (nullptr == m_pRenderPass_RadialBlur)
        return E_FAIL;

    m_pRenderPass_ShadowMask = CRenderPass_ShadowMask::Create(m_pDevice, m_pContext, _uint2(m_iOriginViewportWidth, m_iOriginViewportHeight), _uint2(m_iShadowMapWidth, m_iShadowMapHeight));
    if (nullptr == m_pRenderPass_ShadowMask)
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Bind_Matrices(CShader* pShaderCom) const
{
    if (FAILED(pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Ready_DebugRenders()
{
    _uint iViewportWidth = m_iOriginViewportWidth;
    _uint iViewportHeight = m_iOriginViewportHeight;

    m_pGameInstance->Ready_RT_Debug(L"Target_TransparencyAcc", 100.f, 100.f, 200.f, 200.f);
    m_pGameInstance->Ready_RT_Debug(L"Target_Revealage", 100.f, 300.f, 200.f, 200.f);
    m_pGameInstance->Ready_RT_Debug(L"Target_Effect", 100.f, 500.f, 200.f, 200.f);
    m_pGameInstance->Ready_RT_Debug(L"Target_Distortion", 100.f, 500.f, 200.f, 200.f);

    m_pGameInstance->Ready_RT_Debug(L"Target_Diffuse", 100.f, 100.f, 200.f, 200.f);
    m_pGameInstance->Ready_RT_Debug(L"Target_Normal", 100.f, 300.f, 200.f, 200.f);
    m_pGameInstance->Ready_RT_Debug(L"Target_Depth", 100.f, 500.f, 200.f, 200.f);
    m_pGameInstance->Ready_RT_Debug(L"Target_MtrlProperty", 300.f, 100.f, 200.f, 200.f);
    m_pGameInstance->Ready_RT_Debug(L"Target_EmissionColor", 300.f, 300.f, 200.f, 200.f);

    m_pGameInstance->Ready_RT_Debug(L"Target_Shade", iViewportWidth - 200.f, 150.f, 400.f, 300.f);
    m_pGameInstance->Ready_RT_Debug(L"Target_Ambient", iViewportWidth - 200.f, 450.f, 400.f, 300.f);

    m_pGameInstance->Ready_RT_Debug(L"Target_UI", 100.f, 100.f, 200.f, 200.f);


    m_DebugMRTTagsByIndex.push_back({ L"MRT_GameObjects" });
    m_DebugMRTTagsByIndex.push_back({ L"MRT_LightAcc" });
    m_DebugMRTTagsByIndex.push_back({ L"MRT_Distortion", L"MRT_WeightedBlendAcc", L"MRT_Revealage" });
    m_DebugMRTTagsByIndex.push_back({ L"MRT_UI" });

	if (FAILED(m_pRenderPass_SSAO->Ready_DebugRender(m_DebugMRTTagsByIndex, _uint2 { iViewportWidth, iViewportHeight })))
        return E_FAIL;
	if (FAILED(m_pRenderPass_Bloom->Ready_DebugRender(m_DebugMRTTagsByIndex, _uint2 { iViewportWidth, iViewportHeight })))
        return E_FAIL;
    /* shadow, shadow mask */
	if (FAILED(m_pRenderPass_ShadowMask->Ready_DebugRender(m_DebugMRTTagsByIndex, _uint2 { iViewportWidth, iViewportHeight })))
        return E_FAIL;

    m_pDebugRender = CDebugRender::Create(m_pDevice, m_pContext);
    if (nullptr == m_pDebugRender)
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Ready_DebugDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC DepthStencilDesc {};
    DepthStencilDesc.DepthEnable = TRUE;                        // ���� �׽�Ʈ Ȱ��ȭ
    DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;  // ���� ���ۿ� ���� ���
    DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;       // ����� �ȼ��� ���

    DepthStencilDesc.StencilEnable = FALSE;                       // ���ٽ� ��Ȱ��ȭ
    DepthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    DepthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    // ���ٽ� ����Ʈ ���̽� ���� ���� (������� �����Ƿ� �⺻��)
    DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // ���ٽ� �� ���̽� ���� ����
    DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;

    HRESULT hr = m_pDevice->CreateDepthStencilState(&DepthStencilDesc, &m_pDebugDSS);
    if (FAILED(hr))
    {
        MSG_BOX("Fail To Create Depth stencil stae");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CRenderer::Render_Debug()
{
    m_pContext->OMSetDepthStencilState(m_pDebugDSS, 0);
    m_pContext->GSSetShader(nullptr, nullptr, 0);

    m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    // debug component render and MyDebugRender
    for (auto& pDebugCom : m_DebugComponents[(_uint)DEBUGRENDERGROUP::NONBLEND])
        pDebugCom->Render();

    m_pDebugRender->Render_NonBlend();


    for (auto& pDebugCom : m_DebugComponents[(_uint)DEBUGRENDERGROUP::BLEND])
        pDebugCom->Render(); // ���� ���⼭ �������� ���� �ʾƼ� ���� ������Ʈ�� alpha blend�� ������� �ʴ´ٸ�, �Ʒ��� debugRender->Render_Blend()�� ���ĺ������� �������ȴ�.

    m_pDebugRender->Render_Blend();

    return S_OK;
}
#endif

void CRenderer::Clear()
{
    for (auto& ObjectList : m_RenderObjects)
    {
        for (auto& pRenderObejct : ObjectList)
            Safe_Release(pRenderObejct);

        ObjectList.clear();
    }

#ifdef _DEBUG
    for (auto& Group : m_DebugComponents)
    {
        for (auto& pRenderComponent : Group)
            Safe_Release(pRenderComponent);

        Group.clear();
    }

    m_pDebugRender->Clear();
#endif
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRenderer* pInstance = new CRenderer(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed To Created : CRenderer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenderer::Free()
{
    CBase::Free();

    Clear();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pLightingShaderCom);
    Safe_Release(m_pVIBufferCom);

#ifdef _DEBUG
    Safe_Release(m_pDebugRender);
    Safe_Release(m_pDebugDSS);
    Safe_Release(m_pDebugShaderCom);
#endif // _DEBUG

    Safe_Release(m_pRenderPass_SSAO);
    Safe_Release(m_pRenderPass_Bloom);
    Safe_Release(m_pRenderPass_RadialBlur);
    Safe_Release(m_pRenderPass_ShadowMask);

    Safe_Release(m_pShadowDSV);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
