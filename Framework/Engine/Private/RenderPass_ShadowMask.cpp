#include "RenderPass_ShadowMask.h"

#include "Renderer_Params.h"
#include "GameInstance.h"
#include "FlattenArray_3D.h"

CRenderPass_ShadowMask::CRenderPass_ShadowMask(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderPass_ShadowMask::Initialize(_uint2 vViewportSize, _uint2 vShadowMapSize)
{
	/* Target_ShadowMask */
	if (FAILED(m_pGameInstance->
		Add_RenderTarget(L"Target_ShadowMask", vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R8_UNORM, _float4(1.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* MRT_ShadowMask_Final */
	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_ShadowMask", L"Target_ShadowMask")))
		return E_FAIL;

	/* Ready_Components */
	m_pShaderCom_Blur = CShader::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Blur.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShaderCom_Blur)
		return E_FAIL;
	m_pShaderCom_ShadowMask = CShader::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Shadow.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShaderCom_ShadowMask)
		return E_FAIL;


	if (FAILED(Ready_RandomNoise()))
		return E_FAIL;

	/* Shader Params */
	m_ShaderParams.fNumFilters = m_NoiseTextureSize.z;
	m_ShaderParams.vNoiseTexelSize = _vec4(vViewportSize.x / m_NoiseTextureSize.x, vViewportSize.y / m_NoiseTextureSize.y, 0.f, 0.f);
	m_ShaderParams.vShadowTexelSize = _vec4(1.f / vShadowMapSize.x, 1.f / vShadowMapSize.y, 0.f, 0.f);


	/* SamplerStete */
	{
		D3D11_SAMPLER_DESC SamplerDesc {};
		SamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		//SamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		SamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.BorderColor[0] = 0.f;
		SamplerDesc.BorderColor[1] = 0.f;
		SamplerDesc.BorderColor[2] = 0.f;
		SamplerDesc.BorderColor[3] = 0.f;

		SamplerDesc.MipLODBias = 0.f;
		SamplerDesc.MaxAnisotropy = 1;
		SamplerDesc.MinLOD = 0;
		SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		if (FAILED(m_pDevice->CreateSamplerState(&SamplerDesc, &m_pSamplerState_LinearCmpBorder)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderPass_ShadowMask::Render(const RENDERPASS_PARAMS& Params)
{
	PROFILE_SCOPE_TAG;

	if (true == Params.isHardShadow)
	{
		if (FAILED(Render_HardShadow(Params)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(PCF(Params)))
			return E_FAIL;

		if (FAILED(Blur(Params)))
			return E_FAIL;
	}

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderPass_ShadowMask::Ready_DebugRender(vector<vector<_wstring>>& DebugMRTTagsByIndex, _uint2 vViewportSize)
{
	m_pGameInstance->Ready_RT_Debug(L"Target_Shadow", vViewportSize.x - 100.f, 100.f, 200.f, 200.f);

	m_pGameInstance->Ready_RT_Debug(L"Target_ShadowMask", 150.f, 150.f, 300.f, 300.f);
	m_pGameInstance->Ready_RT_Debug(L"Target_ShadowMask_Final", 150.f, 450.f, 300.f, 300.f);

	DebugMRTTagsByIndex.push_back({ L"MRT_Shadow", L"MRT_ShadowMask_Final", L"MRT_ShadowMask"});

	return S_OK;
}
#endif // _DEBUG

HRESULT CRenderPass_ShadowMask::Ready_RandomNoise()
{
	_uint iDepth = static_cast<_uint>(m_NoiseTextureSize.x);
	_uint iHeight = static_cast<_uint>(m_NoiseTextureSize.y);
	_uint iWidth = static_cast<_uint>(m_NoiseTextureSize.z);

	CFlattenArray_3D<_vec4> NoisedFilter(iWidth, iHeight, iDepth);

	for (_uint i = 0; i < iDepth; ++i)
	{
		for (_uint j = 0; j < iHeight; ++j)
		{
			_vec2 Filter[4][4] {};

			for (_uint k = 0; k < 4; ++k)
			{
				for (_uint l = 0; l < 4; ++l)
				{
					Filter[k][l].x = CUtils::Compute_Random_Normal() + k;
					Filter[k][l].y = CUtils::Compute_Random_Normal() + l;

					Filter[k][l] /= 4.f;
				}
			}

			for (_uint k = 0; k < 4; ++k)
			{
				for (_uint l = 0; l < 4; ++l)
				{
					_float fX = Filter[k][l].x;
					_float fY = Filter[k][l].y;

					_float fWrapX = sqrtf(fX) * cosf(fY * XM_2PI);
					_float fWrapY = sqrtf(fX) * sinf(fY * XM_2PI);

					Filter[k][l] = _vec2(fWrapX, fWrapY);
				}
			}
			for (_uint k = 0; k < iWidth; k += 2)
			{
				_uint iFilterXIndex = k >> 1;

				NoisedFilter.at(k    , j, i).x = Filter[iFilterXIndex][0].x;
				NoisedFilter.at(k    , j, i).y = Filter[iFilterXIndex][0].y;

				NoisedFilter.at(k    , j, i).z = Filter[iFilterXIndex][1].x;
				NoisedFilter.at(k    , j, i).w = Filter[iFilterXIndex][1].y;

				NoisedFilter.at(k + 1, j, i).x = Filter[iFilterXIndex][2].x;
				NoisedFilter.at(k + 1, j, i).y = Filter[iFilterXIndex][2].y;

				NoisedFilter.at(k + 1, j, i).z = Filter[iFilterXIndex][3].x;
				NoisedFilter.at(k + 1, j, i).w = Filter[iFilterXIndex][3].y;
			}
		}
	}

	m_pTextureCom_Noise = CTexture::Create_Texture3D_With_InitailData(m_pDevice, m_pContext, NoisedFilter.Data(), DXGI_FORMAT_R32G32B32A32_FLOAT, sizeof(_vec4), iWidth, iHeight, iDepth);
	if (nullptr == m_pTextureCom_Noise)
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderPass_ShadowMask::Bind_Matrices(CShader* pShader, const _float4x4* pWorldMatrix, const _float4x4* pViewMatrix, const _float4x4* pProjMatrix)
{
	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", pWorldMatrix)))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", pViewMatrix)))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", pProjMatrix)))
		return E_FAIL;

	/* Inverse */
	if (FAILED(m_pShaderCom_ShadowMask->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom_ShadowMask->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderPass_ShadowMask::PCF(const RENDERPASS_PARAMS& Params)
{
	//if (FAILED(m_pGameInstance->Begin_MRT(Params.strFinalMRTTag)))
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_ShadowMask")))
		return E_FAIL;

	/* Textures */
	if (FAILED(m_pGameInstance->Bind_RT_To_Shader(Params.strShadowMapSRVTag, m_pShaderCom_ShadowMask, "g_ShadowTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_To_Shader(Params.strDepthSRVTag, m_pShaderCom_ShadowMask, "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_To_Shader(Params.strNormalSRVTag, m_pShaderCom_ShadowMask, "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pTextureCom_Noise->Bind_Texture_To_Shader(m_pShaderCom_ShadowMask, "g_NoiseTexture")))
		return E_FAIL;

	/* Params */
	if (FAILED(m_pGameInstance->Bind_CascadeCircumSphere_To_Shader(m_pShaderCom_ShadowMask, "g_CircumSphere")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_ShadowMatrices_To_Shader(m_pShaderCom_ShadowMask, "g_LightViewMatrices", "g_LightProjMatrices")))
		return E_FAIL;

	m_ShaderParams.fRadius = Params.pParams->fRadius;
	if (FAILED(m_pShaderCom_ShadowMask->Bind_RawValue("g_ShadowMaskParamas", &m_ShaderParams, sizeof(SHADOWMASK_PARAMS))))
		return E_FAIL;

	/* Matrix */
	if (FAILED(Bind_Matrices(m_pShaderCom_ShadowMask, Params.pWorldMatrix, Params.pViewMatrix, Params.pProjMatrix)))
		return E_FAIL;

	/* Sampler */
	if (FAILED(m_pShaderCom_ShadowMask->Bind_Sampler("LinearCmpBorder", m_pSamplerState_LinearCmpBorder)))
		return E_FAIL;


	if (FAILED(m_pShaderCom_ShadowMask->Begin(static_cast<_uint>(SHADER_PASS::PCF))))
		return E_FAIL;

	if (FAILED(Params.pVIBuffer->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(Params.pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderPass_ShadowMask::Blur(const RENDERPASS_PARAMS& Params)
{
	if (FAILED(m_pGameInstance->Begin_MRT(Params.strFinalMRTTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_Depth", m_pShaderCom_Blur, "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_To_Shader(L"Target_ShadowMask", m_pShaderCom_Blur, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom_Blur->Bind_RawValue("g_BilateralBlurParams", Params.pBilateralBlueParams, sizeof(BILATERALBLUR_PARAMS))))
		return E_FAIL;

	if (FAILED(Bind_Matrices(m_pShaderCom_Blur, Params.pWorldMatrix, Params.pViewMatrix, Params.pProjMatrix)))
		return E_FAIL;

	m_pShaderCom_Blur->Begin((_uint)SHADER_PASS_BLUR::BILATERAL);

	Params.pVIBuffer->Bind_InputAssembler();
	Params.pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderPass_ShadowMask::Render_HardShadow(const RENDERPASS_PARAMS& Params)
{
	if (FAILED(m_pGameInstance->Begin_MRT(Params.strFinalMRTTag)))
		return E_FAIL;

	/* Textures */
	if (FAILED(m_pGameInstance->Bind_RT_To_Shader(Params.strShadowMapSRVTag, m_pShaderCom_ShadowMask, "g_ShadowTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_To_Shader(Params.strDepthSRVTag, m_pShaderCom_ShadowMask, "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_To_Shader(Params.strNormalSRVTag, m_pShaderCom_ShadowMask, "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pTextureCom_Noise->Bind_Texture_To_Shader(m_pShaderCom_ShadowMask, "g_NoiseTexture")))
		return E_FAIL;

	/* Params */
	if (FAILED(m_pGameInstance->Bind_CascadeCircumSphere_To_Shader(m_pShaderCom_ShadowMask, "g_CircumSphere")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_ShadowMatrices_To_Shader(m_pShaderCom_ShadowMask, "g_LightViewMatrices", "g_LightProjMatrices")))
		return E_FAIL;

	m_ShaderParams.fRadius = Params.pParams->fRadius;
	if (FAILED(m_pShaderCom_ShadowMask->Bind_RawValue("g_ShadowMaskParamas", &m_ShaderParams, sizeof(SHADOWMASK_PARAMS))))
		return E_FAIL;

	/* Matrix */
	if (FAILED(Bind_Matrices(m_pShaderCom_ShadowMask, Params.pWorldMatrix, Params.pViewMatrix, Params.pProjMatrix)))
		return E_FAIL;

	/* Sampler */
	if (FAILED(m_pShaderCom_ShadowMask->Bind_Sampler("LinearCmpBorder", m_pSamplerState_LinearCmpBorder)))
		return E_FAIL;


	if (FAILED(m_pShaderCom_ShadowMask->Begin(static_cast<_uint>(SHADER_PASS::HARD_SHADOW))))
		return E_FAIL;

	if (FAILED(Params.pVIBuffer->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(Params.pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

CRenderPass_ShadowMask* CRenderPass_ShadowMask::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint2 vViewportSize, _uint2 vShadowMapSize)
{
	CRenderPass_ShadowMask* pInstance = new CRenderPass_ShadowMask(pDevice, pContext);
	if (FAILED(pInstance->Initialize(vViewportSize, vShadowMapSize)))
	{
		MSG_BOX("Failed To Create : CRenderPass_ShadowMask");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderPass_ShadowMask::Free()
{
	super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pShaderCom_Blur);
	Safe_Release(m_pShaderCom_ShadowMask);
	Safe_Release(m_pTextureCom_Noise);

	Safe_Release(m_pSamplerState_LinearCmpBorder);
}
