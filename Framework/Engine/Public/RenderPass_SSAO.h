#pragma once

#include "Base.h"
#include "Renderer_Params.h"

BEGIN(Engine)

class CGameInstance;
class CShader;
class CVIBuffer_Rect;

class CRenderPass_SSAO final : public CBase
{
	using super = CBase;

public:
	struct RENDERPASS_PARAMS
	{
		_wstring strFinalMRTTag;
		_wstring strFinalSRVTag;
		_wstring strNormalSRVTag;
		_wstring strDepthSRVTag;

		CVIBuffer_Rect*	pVIBuffer { nullptr };
		const _float4x4* pWorldMatrix { nullptr };
		const _float4x4* pViewMatrix { nullptr };
		const _float4x4* pProjMatrix { nullptr };

		const SSAO_PARAMS* pSSAOParams;
	};

private:
	CRenderPass_SSAO();
	virtual ~CRenderPass_SSAO() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint2 vViewportSize);
	HRESULT Render(const RENDERPASS_PARAMS& Params);

#ifdef _DEBUG
	HRESULT Ready_DebugRender(vector<vector<_wstring>>& DebugMRTTagsByIndex, _uint2 vViewportSize);
#endif

private:
	CGameInstance*			m_pGameInstance {nullptr};

	CShader* m_pSSAOShaderCom { nullptr };
	CShader* m_pBlurShaderCom { nullptr };


	_vec2 m_vNoiseTextureSize { 128.f, 128.f };
	CTexture* m_pTextureCom_RandomRotationNoise { nullptr };

	static const _uint m_iNumMaxSamples { 24 };
	array<_vec4, m_iNumMaxSamples> m_Kernel {};

	const _uint2 m_vArraySize { 8, 8 };
	array<_vec4, 64> m_RandomRotatedTangent {};
	
	_vec3 m_vRandomSampleSize { 16.f, 64.f, 64.f};
	CTexture* m_pTextureCom_RandomSample { nullptr };

private:
	HRESULT Compute_AO(const RENDERPASS_PARAMS& Params);
	HRESULT Blur_AO(const RENDERPASS_PARAMS& Params);
	HRESULT Apply_AO(const RENDERPASS_PARAMS& Params);

	HRESULT Bind_Matrices(CShader* pShader, const _float4x4* pWorldMatrix, const _float4x4* pViewMatrix, const _float4x4* pProjMatrix);

public:
	static CRenderPass_SSAO* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint2 vViewportSize);
	virtual void Free() override;
};
END
