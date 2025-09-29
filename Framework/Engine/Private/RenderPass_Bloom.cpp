#include "RenderPass_Bloom.h"

#include "GameInstance.h"

CRenderPass_Bloom::CRenderPass_Bloom()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderPass_Bloom::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint2 vViewportSize)
{
    _uint iViewportWidth = vViewportSize.x;
    _uint iViewportHeight = vViewportSize.y;

    /* Target_Sampling */
    _uint iWidth = 0;
    _uint iHeight = 0;

    iWidth = iViewportWidth / m_iBloomViewportWidthDenom_0;
    iHeight = iViewportHeight / m_iBloomViewportHeightDenom_0;
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Sampling_4_BloomCurve", iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Sampling_4_Blur", iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Sampling_4_AccTemp", iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Sampling_4_Acc", iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(Ready_DepthStencilBuffer(pDevice, iWidth, iHeight, &m_pDSV_BloomSampling_0)))
        return E_FAIL;

    iWidth = iViewportWidth / m_iBloomViewportWidthDenom_1;
    iHeight = iViewportHeight / m_iBloomViewportHeightDenom_1;
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Sampling_4_6_BloomCurve", iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Sampling_4_6_Blur", iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Sampling_4_6_Acc", iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Sampling_4_6_AccTemp", iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(Ready_DepthStencilBuffer(pDevice, iWidth, iHeight, &m_pDSV_BloomSampling_1)))
        return E_FAIL;

    iWidth = iViewportWidth / m_iBloomViewportWidthDenom_2;
    iHeight = iViewportHeight / m_iBloomViewportHeightDenom_2;
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Sampling_4_6_6_BloomCurve", iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Sampling_4_6_6_BloomCurveTemp" /* blur X */, iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Sampling_4_6_6_Blur", iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->
        Add_RenderTarget(L"Target_Sampling_4_6_6_Acc", iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(Ready_DepthStencilBuffer(pDevice, iWidth, iHeight, &m_pDSV_BloomSampling_2)))
        return E_FAIL;


    /* MRT_Sampling_4... */
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Sampling_4_BloomCurve", L"Target_Sampling_4_BloomCurve")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Sampling_4_BlurTemp", L"Target_Sampling_4_AccTemp")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Sampling_4_Blur", L"Target_Sampling_4_Blur")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Sampling_4_Acc", L"Target_Sampling_4_Acc")))
        return E_FAIL;

    /* MRT_Sampling_4_6... */
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Sampling_4_6_BloomCurve", L"Target_Sampling_4_6_BloomCurve")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Sampling_4_6_BlurTemp", L"Target_Sampling_4_6_AccTemp")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Sampling_4_6_Blur", L"Target_Sampling_4_6_Blur")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Sampling_4_6_Acc", L"Target_Sampling_4_6_Acc")))
        return E_FAIL;

    /* MRT_Sampling_4_6_6... */
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Sampling_4_6_6_BloomCurve", L"Target_Sampling_4_6_6_BloomCurve")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Sampling_4_6_6_BlurTemp", L"Target_Sampling_4_6_6_BloomCurveTemp")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Sampling_4_6_6_Blur", L"Target_Sampling_4_6_6_Blur")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Sampling_4_6_6_Acc", L"Target_Sampling_4_6_6_Acc")))
        return E_FAIL;


    /* Ready_Components */
    m_pShaderCom_Bloom = CShader::Create(pDevice, pContext, L"../../EngineSDK/Hlsl/Shader_Deferred.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShaderCom_Bloom)
        return E_FAIL;
    m_pShaderCom_Blur = CShader::Create(pDevice, pContext, L"../../EngineSDK/Hlsl/Shader_Blur.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShaderCom_Blur)
        return E_FAIL;

	return S_OK;
}

HRESULT CRenderPass_Bloom::Render(const RENDERPASS_PARAMS& Params)
{
    // down sampling 4x4 and bloom curve -> 历厘
    if (FAILED(Extract_Bright(Params)))
        return E_FAIL;

    // bloom curve -> down sampling 4_6 -> 历厘
    if (FAILED(Sampling(Params, L"MRT_Sampling_4_6_BloomCurve", m_pDSV_BloomSampling_1, L"Target_Sampling_4_BloomCurve")))
        return E_FAIL;

    // down sampling 4_6 -> down sampling 4_6_6 -> 历厘
    if (FAILED(Sampling(Params, L"MRT_Sampling_4_6_6_BloomCurve", m_pDSV_BloomSampling_2, L"Target_Sampling_4_6_BloomCurve")))
        return E_FAIL;

    // blur 4_6_6
    if (FAILED(Blur(Params, L"MRT_Sampling_4_6_6_Blur", m_pDSV_BloomSampling_2, L"Target_Sampling_4_6_6_BloomCurve")))
    //if (FAILED(Blur_XY(Params, L"MRT_Sampling_4_6_6_Blur", m_pDSV_BloomSampling_2, L"Target_Sampling_4_6_6_BloomCurve")))
        return E_FAIL;

    // blur 4_6_6 -> up sampling + down sample 4_6 -> 历厘
    if (FAILED(Accumulate_Bloom(Params, L"MRT_Sampling_4_6_Acc", m_pDSV_BloomSampling_1, L"Target_Sampling_4_6_6_Blur", L"Target_Sampling_4_6_BloomCurve")))
        return E_FAIL;

    // blur 4_6
    if (FAILED(Blur(Params, L"MRT_Sampling_4_6_Blur", m_pDSV_BloomSampling_1, L"Target_Sampling_4_6_Acc")))
    //if (FAILED(Blur_XY(Params, L"MRT_Sampling_4_6_Blur", m_pDSV_BloomSampling_1, L"Target_Sampling_4_6_Acc")))
        return E_FAIL;
 
    // 4_6 -> upsampling + bloom curve -> 历厘
    if (FAILED(Accumulate_Bloom(Params, L"MRT_Sampling_4_Acc", m_pDSV_BloomSampling_0, L"Target_Sampling_4_6_Blur", L"Target_Sampling_4_BloomCurve")))
        return E_FAIL;
 
    // blur _4
    if (FAILED(Blur(Params, L"MRT_Sampling_4_Blur", m_pDSV_BloomSampling_0, L"Target_Sampling_4_Acc")))
    //if (FAILED(Blur_XY(Params, L"MRT_Sampling_4_Blur", m_pDSV_BloomSampling_0, L"Target_Sampling_4_Acc")))
        return E_FAIL;
 
    // _4 -> up sampling + original -> 历厘
    if (FAILED(Apply_Bloom(Params)))
        return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderPass_Bloom::Ready_DebugRender(vector<vector<_wstring>>& DebugMRTTagsByIndex, _uint2 vViewportSize)
{
    m_pGameInstance->Ready_RT_Debug(L"Target_Sampling_4_BloomCurve", 100.f, 100.f, 200.f, 200.f);
    m_pGameInstance->Ready_RT_Debug(L"Target_Sampling_4_Blur", 100.f, 300.f, 200.f, 200.f);
    m_pGameInstance->Ready_RT_Debug(L"Target_Sampling_4_Acc", 100.f, 500.f, 200.f, 200.f);

    m_pGameInstance->Ready_RT_Debug(L"Target_Sampling_4_6_BloomCurve", 300.f, 100.f, 200.f, 200.f);
    m_pGameInstance->Ready_RT_Debug(L"Target_Sampling_4_6_Blur", 300.f, 300.f, 200.f, 200.f);
    m_pGameInstance->Ready_RT_Debug(L"Target_Sampling_4_6_Acc", 300.f, 500.f, 200.f, 200.f);

    m_pGameInstance->Ready_RT_Debug(L"Target_Sampling_4_6_6_BloomCurve", 500.f, 100.f, 200.f, 200.f);
    m_pGameInstance->Ready_RT_Debug(L"Target_Sampling_4_6_6_Blur", 500.f, 300.f, 200.f, 200.f);
    m_pGameInstance->Ready_RT_Debug(L"Target_Sampling_4_6_6_Acc", 500.f, 500.f, 200.f, 200.f);

    
    DebugMRTTagsByIndex.push_back(
        {
            L"MRT_Sampling_4_BloomCurve", L"MRT_Sampling_4_Blur", L"MRT_Sampling_4_Acc",
            L"MRT_Sampling_4_6_BloomCurve", L"MRT_Sampling_4_6_Blur", L"MRT_Sampling_4_6_Acc",
            L"MRT_Sampling_4_6_6_BloomCurve", L"MRT_Sampling_4_6_6_Blur", L"MRT_Sampling_4_6_6_Acc",
        });

	return S_OK;
}
#endif

HRESULT CRenderPass_Bloom::Bind_Matrices(CShader* pShader, const _float4x4* pWorldMatrix, const _float4x4* pViewMatrix, const _float4x4* pProjMatrix)
{
    if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", pWorldMatrix)))
        return E_FAIL;
    if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", pViewMatrix)))
        return E_FAIL;
    if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", pProjMatrix)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderPass_Bloom::Ready_DepthStencilBuffer(ID3D11Device* pDevice, _uint iWidth, _uint iHeight, ID3D11DepthStencilView** ppOut, _uint iArraySize)
{
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

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

	if (FAILED(pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, ppOut)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

HRESULT CRenderPass_Bloom::Blur(const RENDERPASS_PARAMS& Params, const _wstring& strBeginMRTTag, ID3D11DepthStencilView* pDSV, const _wstring& strSRVTag)
{
    if (FAILED(m_pGameInstance->Begin_MRT(strBeginMRTTag, true, pDSV)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(strSRVTag, m_pShaderCom_Blur, "g_Texture")))
        return E_FAIL;

    if (FAILED(Bind_Matrices(m_pShaderCom_Blur, Params.pWorldMatrix, Params.pViewMatrix, Params.pProjMatrix)))
        return E_FAIL;

    m_pShaderCom_Blur->Begin((_uint)SHADER_PASS_BLUR::GAUSSIAN5x5);
    //m_pShaderCom_Blur->Begin((_uint)SHADER_PASS_BLUR::GAUSSIANBLUR9_X);

    Params.pVIBuffer->Bind_InputAssembler();
    Params.pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderPass_Bloom::Blur_XY(const RENDERPASS_PARAMS& Params, const _wstring& strBeginMRTTag, ID3D11DepthStencilView* pDSV, const _wstring& strSRVTag)
{
    if (FAILED(m_pGameInstance->Begin_MRT(strBeginMRTTag + L"Temp", true, pDSV)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(strSRVTag, m_pShaderCom_Blur, "g_Texture")))
        return E_FAIL;

    if (FAILED(Bind_Matrices(m_pShaderCom_Blur, Params.pWorldMatrix, Params.pViewMatrix, Params.pProjMatrix)))
        return E_FAIL;

    m_pShaderCom_Blur->Begin((_uint)SHADER_PASS_BLUR::GAUSSIAN5_X);

    Params.pVIBuffer->Bind_InputAssembler();
    Params.pVIBuffer->Render();


    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Begin_MRT(strBeginMRTTag, true, pDSV)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(strSRVTag + L"Temp", m_pShaderCom_Blur, "g_Texture")))
        return E_FAIL;

    if (FAILED(Bind_Matrices(m_pShaderCom_Blur, Params.pWorldMatrix, Params.pViewMatrix, Params.pProjMatrix)))
        return E_FAIL;

    m_pShaderCom_Blur->Begin((_uint)SHADER_PASS_BLUR::GAUSSIAN5_Y);

    Params.pVIBuffer->Bind_InputAssembler();
    Params.pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderPass_Bloom::Accumulate_Bloom(const RENDERPASS_PARAMS& Params, const _wstring& strBeginMRTTag, ID3D11DepthStencilView* pDSV, const _wstring& strSRVTag, const _wstring& strBloomCurveTag)
{
    if (FAILED(m_pGameInstance->Begin_MRT(strBeginMRTTag, true, pDSV)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(strSRVTag, m_pShaderCom_Bloom, "g_FinalTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(strBloomCurveTag, m_pShaderCom_Bloom, "g_SourTexture")))
        return E_FAIL;

    if (FAILED(Bind_Matrices(m_pShaderCom_Bloom, Params.pWorldMatrix, Params.pViewMatrix, Params.pProjMatrix)))
        return E_FAIL;

    m_pShaderCom_Bloom->Begin((_uint)SHADER_PASS_DEFERRED::BLOOM_ACC);

    Params.pVIBuffer->Bind_InputAssembler();
    Params.pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderPass_Bloom::Apply_Bloom(const RENDERPASS_PARAMS& Params)
{
    if (FAILED(m_pGameInstance->Begin_MRT(Params.strFinalMRTTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(Params.strFinalSRVTag, m_pShaderCom_Bloom, "g_FinalTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_Sampling_4_Blur", m_pShaderCom_Bloom, "g_SourTexture")))
        return E_FAIL;

    if (FAILED(Bind_Matrices(m_pShaderCom_Bloom, Params.pWorldMatrix, Params.pViewMatrix, Params.pProjMatrix)))
        return E_FAIL;

    m_pShaderCom_Bloom->Begin((_uint)SHADER_PASS_DEFERRED::BLOOM);

    Params.pVIBuffer->Bind_InputAssembler();
    Params.pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderPass_Bloom::Sampling(const RENDERPASS_PARAMS& Params, const _wstring& strBeginMRTTag, ID3D11DepthStencilView* pDSV, const _wstring& strSRVTag)
{
    if (FAILED(m_pGameInstance->Begin_MRT(strBeginMRTTag, true, pDSV)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(strSRVTag, m_pShaderCom_Bloom, "g_FinalTexture")))
        return E_FAIL;

    if (FAILED(Bind_Matrices(m_pShaderCom_Bloom, Params.pWorldMatrix, Params.pViewMatrix, Params.pProjMatrix)))
        return E_FAIL;

    m_pShaderCom_Bloom->Begin((_uint)SHADER_PASS_DEFERRED::SAMPLING);

    Params.pVIBuffer->Bind_InputAssembler();
    Params.pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderPass_Bloom::Extract_Bright(const RENDERPASS_PARAMS& Params)
{
    if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Sampling_4_BloomCurve", true, m_pDSV_BloomSampling_0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom_Bloom->Bind_RawValue("g_BloomParams", Params.pBloomParams, sizeof(BLOOM_PARAMS))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(Params.strFinalSRVTag, m_pShaderCom_Bloom, "g_FinalTexture")))
        return E_FAIL;

    if (FAILED(Bind_Matrices(m_pShaderCom_Bloom, Params.pWorldMatrix, Params.pViewMatrix, Params.pProjMatrix)))
        return E_FAIL;

    m_pShaderCom_Bloom->Begin((_uint)SHADER_PASS_DEFERRED::BLOOM_CURVE);

    Params.pVIBuffer->Bind_InputAssembler();
    Params.pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

CRenderPass_Bloom* CRenderPass_Bloom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint2 vViewportSize)
{
    CRenderPass_Bloom* pInstance = new CRenderPass_Bloom();
    if (FAILED(pInstance->Initialize(pDevice, pContext, vViewportSize)))
    {
        MSG_BOX("Failed To Create : CRenderPass_Bloom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenderPass_Bloom::Free()
{
    super::Free();

    Safe_Release(m_pShaderCom_Bloom);
    Safe_Release(m_pShaderCom_Blur);

    Safe_Release(m_pDSV_BloomSampling_0);
    Safe_Release(m_pDSV_BloomSampling_1);
    Safe_Release(m_pDSV_BloomSampling_2);
    
    Safe_Release(m_pGameInstance);
}
