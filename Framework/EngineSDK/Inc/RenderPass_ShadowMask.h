#pragma once

#include "Base.h"
#include "Renderer_Params.h"

BEGIN(Engine)

class CGameInstance;
class CShader;
class CVIBuffer_Rect;
class CTexture;

class CRenderPass_ShadowMask final : public CBase
{
	using super = CBase;

	enum class SHADER_PASS
	{
		HARD_SHADOW,
		PCF,
	};

public:
	struct RENDERPASS_PARAMS
	{
		_wstring strFinalMRTTag;
		_wstring strFinalSRVTag;

		_wstring strNormalSRVTag;
		_wstring strDepthSRVTag;
		_wstring strShadowMapSRVTag;

		CVIBuffer_Rect* pVIBuffer { nullptr };
		const _float4x4* pWorldMatrix { nullptr };
		const _float4x4* pViewMatrix { nullptr };
		const _float4x4* pProjMatrix { nullptr };

		const SHADOWMASK_PARAMS* pParams { nullptr };
		const BILATERALBLUR_PARAMS* pBilateralBlueParams { nullptr };

		_bool isHardShadow { false };
	};

private:
	CRenderPass_ShadowMask(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderPass_ShadowMask() = default;

public:
	HRESULT Initialize(_uint2 vViewportSize, _uint2 vShadowMapSize);
	HRESULT Render(const RENDERPASS_PARAMS& Params);

#ifdef _DEBUG
	HRESULT Ready_DebugRender(vector<vector<_wstring>>& DebugMRTTagsByIndex, _uint2 vViewportSize);
#endif

private:
	ID3D11Device* m_pDevice { nullptr };
	ID3D11DeviceContext* m_pContext { nullptr };
	CGameInstance* m_pGameInstance { nullptr };

	CShader* m_pShaderCom_Blur { nullptr };
	CShader* m_pShaderCom_ShadowMask { nullptr };
	CTexture* m_pTextureCom_Noise { nullptr };

	//_vec4 m_NoisedFilter[32][32][8]; 
	/* depth, height, width */
	const _vec3 m_NoiseTextureSize = { 32.f, 32.f, 8.f };

	SHADOWMASK_PARAMS m_ShaderParams {};

	ID3D11SamplerState* m_pSamplerState_LinearCmpBorder { nullptr };
	ID3D11SamplerState* m_pSamplerState_Linear { nullptr };

private:
	HRESULT Ready_RandomNoise();

	HRESULT Bind_Matrices(CShader* pShader, const _float4x4* pWorldMatrix, const _float4x4* pViewMatrix, const _float4x4* pProjMatrix);

	HRESULT PCF(const RENDERPASS_PARAMS& Params);
	HRESULT Blur(const RENDERPASS_PARAMS& Params);

	HRESULT Render_HardShadow(const RENDERPASS_PARAMS& Params);

public:
	static CRenderPass_ShadowMask* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint2 vViewportSize, _uint2 vShadowMapSize);
	virtual void Free() override;

};
END
