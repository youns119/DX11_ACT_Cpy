#include "RenderPass_SSAO.h"

#include "GameInstance.h"
#include "Renderer.h"
#include "FlattenArray_2D.h"
#include "FlattenArray_3D.h"

CRenderPass_SSAO::CRenderPass_SSAO()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderPass_SSAO::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint2 vViewportSize)
{
	_uint iViewportWidth = vViewportSize.x;
	_uint iViewportHeight = vViewportSize.y;

    /* Ready_RenderTargets */
	if (FAILED(m_pGameInstance->
		Add_RenderTarget(L"Target_SSAO", iViewportWidth, iViewportHeight, DXGI_FORMAT_R8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->
		Add_RenderTarget(L"Target_SSAO_Blur", iViewportWidth, iViewportHeight, DXGI_FORMAT_R8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


    /* Ready_MRTs */
	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_SSAO", L"Target_SSAO")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_SSAO_Blur", L"Target_SSAO_Blur")))
		return E_FAIL;

    
    /* Ready Components */
    m_pSSAOShaderCom = CShader::Create(pDevice, pContext, L"../../EngineSDK/Hlsl/Shader_SSAO.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pSSAOShaderCom)
        return E_FAIL;

    m_pBlurShaderCom = CShader::Create(pDevice, pContext, L"../../EngineSDK/Hlsl/Shader_Deferred.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pBlurShaderCom)
        return E_FAIL;


    _uint iCount = 0;
    for (auto& vDir : m_RandomRotatedTangent)
    {
        vDir.x = CUtils::Compute_Random(-1.f, 1.f);
        vDir.y = CUtils::Compute_Random(-1.f, 1.f);
        //vDir = vDir.Normalized();
        vDir = _vec3(vDir.x, vDir.y, 0.f).Normalized();
    }

    /* 24 */
    array<_vec4, m_iNumMaxSamples> Samples {};
    for (_uint i = 0; i < 3; ++i)
    {
        for (_uint j = 0; j < 8; ++j)
        {
            /* sprial */
            _float fTheta = (_float)j / (_float)(8) * XM_PIDIV2; // 0 ~ 63/64
            _float fPhi = (_float)j * XM_2PI / 1.618f + (_float)i;

            _vec3 vDir;
            vDir.x = sinf(fTheta) * cosf(fPhi);
            vDir.y = sinf(fTheta) * sinf(fPhi);
            vDir.z = cosf(fTheta);

            m_Kernel[i * 8 + j] = _vec4(vDir, (i * 8.f + j + 1.f) / static_cast<_float>(m_iNumMaxSamples));
        }
    }

    _uint iWidth = static_cast<_uint>(m_vNoiseTextureSize.x);
    _uint iHeight = static_cast<_uint>(m_vNoiseTextureSize.y);

    CFlattenArray_2D<_vec2> NoisedFilter(iWidth, iHeight);

    for (_uint i = 0; i < iWidth; ++i)
    {
        for (_uint j = 0; j < iHeight; ++j)
        {
            _vec2 vDir {};
            vDir.x = CUtils::Compute_Random_Normal();
            vDir.y = CUtils::Compute_Random_Normal();

            NoisedFilter.at(i, j) = vDir.Normalized();
		}
    }

    m_pTextureCom_RandomRotationNoise = CTexture::Create_With_InitailData(pDevice, pContext, NoisedFilter.Data(), DXGI_FORMAT_R32G32_FLOAT, sizeof(_vec2), iWidth, iHeight, 1);
    if (nullptr == m_pTextureCom_RandomRotationNoise)
        return E_FAIL;


    iWidth = static_cast<_uint>(m_vRandomSampleSize.x);
    iHeight = static_cast<_uint>(m_vRandomSampleSize.y);
    _uint iDepth = static_cast<_uint>(m_vRandomSampleSize.z);
    CFlattenArray_3D<_vec3> RandomSamples(iWidth, iHeight, iDepth);
    for (_uint i = 0; i < iDepth; ++i)
    {
        for (_uint j = 0; j < iHeight; ++j)
        {
            for (_uint k = 0; k < iWidth; ++k)
            {
                _uint iRadiusMag = k / 4; /* 0000 1111 2222 3333 */
                _uint iIter = k % 4; /* 0123 0123 0123 0123 */

				//_float fTheta = static_cast<_float>(iIter) / ((iWidth - 1) / 5) * XM_PIDIV2 + CUtils::Compute_Random(0.f, XM_PIDIV2 / 5.f);
                _float fTheta = CUtils::Compute_Random(0.f, XM_PIDIV2);
				_float fPhi = static_cast<_float>(iIter) / ((iWidth - 1) / 5) * XM_2PI + CUtils::Compute_Random(0.f, XM_2PI / 5.f);
                _float fRadius = static_cast<_float>(iRadiusMag) / 5.f + CUtils::Compute_Random(0.f, 1.f / 5.f);
				//_float fPhi = CUtils::Compute_Random(0.f, XM_2PI);
				//_float fRadius = static_cast<_float>(k) / (iWidth - 1);

                _vec3 vSample;
                vSample.x = fRadius * sinf(fTheta) * cosf(fPhi);
                vSample.y = fRadius * sinf(fTheta) * sinf(fPhi);
                vSample.z = fRadius * cosf(fTheta);

                RandomSamples.at(k, j, i) = vSample;
            }
        }
    }
    m_pTextureCom_RandomSample = CTexture::Create_Texture3D_With_InitailData(pDevice, pContext, RandomSamples.Data(), DXGI_FORMAT_R32G32B32_FLOAT, sizeof(_vec3), iWidth, iHeight, iDepth);
    if (nullptr == m_pTextureCom_RandomSample)
        return E_FAIL;

	return S_OK;
}

HRESULT CRenderPass_SSAO::Render(const RENDERPASS_PARAMS& Params)
{
    if (FAILED(Compute_AO(Params)))
        return E_FAIL;

    if (FAILED(Blur_AO(Params)))
        return E_FAIL;

    if (FAILED(Apply_AO(Params)))
        return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderPass_SSAO::Ready_DebugRender(vector<vector<_wstring>>& DebugMRTTagsByIndex, _uint2 vViewportSize)
{
	_uint iViewportWidth = vViewportSize.x;
	_uint iViewportHeight = vViewportSize.y;

	m_pGameInstance->Ready_RT_Debug(L"Target_SSAO", iViewportWidth * 0.5f, iViewportHeight * 0.5f, iViewportWidth * 1.f, iViewportHeight * 1.f);
	m_pGameInstance->Ready_RT_Debug(L"Target_SSAO_Blur", iViewportWidth * 0.5f, iViewportHeight * 0.5f, iViewportWidth * 1.f, iViewportHeight * 1.f);

	DebugMRTTagsByIndex.push_back({ L"MRT_SSAO" });
	DebugMRTTagsByIndex.push_back({ L"MRT_SSAO_Blur" });

	return S_OK;
}
#endif


HRESULT CRenderPass_SSAO::Compute_AO(const RENDERPASS_PARAMS& Params)
{
    if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_SSAO")))
        return E_FAIL;

    //if (FAILED(m_pSSAOShaderCom->Bind_RawValue("g_RandomVectors", m_RandomRotatedTangent.data(), sizeof(_vec4) * static_cast<_uint>(m_RandomRotatedTangent.size()))))
    //    return E_FAIL;
    //if (FAILED(m_pSSAOShaderCom->Bind_RawValue("g_vArraySize", &m_vArraySize , sizeof(_uint2))))
    //    return E_FAIL;

    if (FAILED(m_pSSAOShaderCom->Bind_RawValue("g_Kernals", m_Kernel.data(), sizeof(_vec4) * static_cast<_uint>(m_Kernel.size()))))
        return E_FAIL;

    if (FAILED(m_pSSAOShaderCom->Bind_RawValue("g_SSAOParams", Params.pSSAOParams, sizeof(SSAO_PARAMS))))
        return E_FAIL;

    if (FAILED(m_pTextureCom_RandomRotationNoise->Bind_Texture_To_Shader(m_pSSAOShaderCom, "g_RandomRotationNoiseTexture")))
        return E_FAIL;

    if (FAILED(m_pTextureCom_RandomSample->Bind_Texture_To_Shader(m_pSSAOShaderCom, "g_RandomSampleTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(Params.strDepthSRVTag, m_pSSAOShaderCom, "g_DepthTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(Params.strNormalSRVTag, m_pSSAOShaderCom, "g_NormalTexture")))
        return E_FAIL;

    if (FAILED(Bind_Matrices(m_pSSAOShaderCom, Params.pWorldMatrix, Params.pViewMatrix, Params.pProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pSSAOShaderCom->Bind_Matrix("g_CamViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
        return E_FAIL;
    if (FAILED(m_pSSAOShaderCom->Bind_Matrix("g_CamProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
        return E_FAIL;
    if (FAILED(m_pSSAOShaderCom->Bind_Matrix("g_CamViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORM::VIEW))))
        return E_FAIL;
    if (FAILED(m_pSSAOShaderCom->Bind_Matrix("g_CamProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORM::PROJ))))
        return E_FAIL;


    m_pSSAOShaderCom->Begin((_uint)SHADER_PASS_SSAO::COMPUTE_AO);

    Params.pVIBuffer->Bind_InputAssembler();
    Params.pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderPass_SSAO::Blur_AO(const RENDERPASS_PARAMS& Params)
{
    if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_SSAO_Blur")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_SSAO", m_pBlurShaderCom, "g_FinalTexture")))
        return E_FAIL;

    if (FAILED(Bind_Matrices(m_pBlurShaderCom, Params.pWorldMatrix, Params.pViewMatrix, Params.pProjMatrix)))
        return E_FAIL;

    m_pBlurShaderCom->Begin((_uint)SHADER_PASS_DEFERRED::BLUR);

    Params.pVIBuffer->Bind_InputAssembler();
    Params.pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderPass_SSAO::Apply_AO(const RENDERPASS_PARAMS& Params)
{

    if (FAILED(m_pGameInstance->Begin_MRT(Params.strFinalMRTTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(Params.strFinalSRVTag, m_pSSAOShaderCom, "g_FinalTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_SSAO_Blur", m_pSSAOShaderCom, "g_AOTexture")))
        return E_FAIL;

    if (FAILED(m_pSSAOShaderCom->Bind_Matrix("g_WorldMatrix", Params.pWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pSSAOShaderCom->Bind_Matrix("g_ViewMatrix", Params.pViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pSSAOShaderCom->Bind_Matrix("g_ProjMatrix", Params.pProjMatrix)))
        return E_FAIL;

    if (FAILED(Bind_Matrices(m_pSSAOShaderCom, Params.pWorldMatrix, Params.pViewMatrix, Params.pProjMatrix)))
        return E_FAIL;

    m_pSSAOShaderCom->Begin((_uint)SHADER_PASS_SSAO::APPLY_AO);

    Params.pVIBuffer->Bind_InputAssembler();
    Params.pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderPass_SSAO::Bind_Matrices(CShader* pShader, const _float4x4* pWorldMatrix, const _float4x4* pViewMatrix, const _float4x4* pProjMatrix)
{
    if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", pWorldMatrix)))
        return E_FAIL;
    if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", pViewMatrix)))
        return E_FAIL;
    if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", pProjMatrix)))
        return E_FAIL;

    return S_OK;
}

CRenderPass_SSAO* CRenderPass_SSAO::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint2 vViewportSize)
{
	CRenderPass_SSAO* pInstance = new CRenderPass_SSAO();
	if (FAILED(pInstance->Initialize(pDevice, pContext, vViewportSize)))
	{
		MSG_BOX("Failed To Create : CRenderPass_SSAO");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderPass_SSAO::Free()
{
	super::Free();

    Safe_Release(m_pSSAOShaderCom);
    Safe_Release(m_pBlurShaderCom);

    Safe_Release(m_pTextureCom_RandomRotationNoise);
    Safe_Release(m_pTextureCom_RandomSample);

    Safe_Release(m_pGameInstance);
}
