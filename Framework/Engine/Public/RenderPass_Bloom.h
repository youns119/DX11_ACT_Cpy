#pragma once

#include "Base.h"
#include "Renderer_Params.h"

BEGIN(Engine)

class CGameInstance;
class CShader;
class CVIBuffer_Rect;

class CRenderPass_Bloom final : public CBase
{
	using super = CBase;

public:
	struct RENDERPASS_PARAMS
	{
		_wstring strFinalMRTTag;
		_wstring strFinalSRVTag;

		CVIBuffer_Rect* pVIBuffer { nullptr };
		const _float4x4* pWorldMatrix { nullptr };
		const _float4x4* pViewMatrix { nullptr };
		const _float4x4* pProjMatrix { nullptr };

		const BLOOM_PARAMS* pBloomParams;
	};

private:
	CRenderPass_Bloom();
	virtual ~CRenderPass_Bloom() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint2 vViewportSize);
	HRESULT Render(const RENDERPASS_PARAMS& Params);

#ifdef _DEBUG
	HRESULT Ready_DebugRender(vector<vector<_wstring>>& DebugMRTTagsByIndex, _uint2 vViewportSize);
#endif

private:
	CGameInstance* m_pGameInstance { nullptr };

	CShader* m_pShaderCom_Bloom { nullptr };
	CShader* m_pShaderCom_Blur { nullptr };

	ID3D11DepthStencilView* m_pDSV_BloomSampling_0 { nullptr };
	ID3D11DepthStencilView* m_pDSV_BloomSampling_1 { nullptr };
	ID3D11DepthStencilView* m_pDSV_BloomSampling_2 { nullptr };

	_uint m_iBloomViewportWidthDenom_0 { 1 };
	_uint m_iBloomViewportWidthDenom_1 { 1 * 4 };
	_uint m_iBloomViewportWidthDenom_2 { 1 * 4 * 4 };
	_uint m_iBloomViewportHeightDenom_0 { 1 };
	_uint m_iBloomViewportHeightDenom_1 { 1 * 4 };
	_uint m_iBloomViewportHeightDenom_2 { 1 * 4 * 4 };

private:
	HRESULT Bind_Matrices(CShader* pShader, const _float4x4* pWorldMatrix, const _float4x4* pViewMatrix, const _float4x4* pProjMatrix);
	HRESULT Ready_DepthStencilBuffer(ID3D11Device* pDevice, _uint iWidth, _uint iHeight, ID3D11DepthStencilView** ppOut, _uint iArraySize = 1);

	HRESULT Blur(const RENDERPASS_PARAMS& Params, const _wstring& strBeginMRTTag, ID3D11DepthStencilView* pDSV, const _wstring& strSRVTag);
	HRESULT Blur_XY(const RENDERPASS_PARAMS& Params, const _wstring& strBeginMRTTag, ID3D11DepthStencilView* pDSV, const _wstring& strSRVTag);
	HRESULT Accumulate_Bloom(const RENDERPASS_PARAMS& Params, const _wstring& strBeginMRTTag, ID3D11DepthStencilView* pDSV, const _wstring& strSRVTag, const _wstring& strBloomCurveTag);
	HRESULT Apply_Bloom(const RENDERPASS_PARAMS& Params);
	HRESULT Sampling(const RENDERPASS_PARAMS& Params, const _wstring& strBeginMRTTag, ID3D11DepthStencilView* pDSV, const _wstring& strSRVTag);
	HRESULT Extract_Bright(const RENDERPASS_PARAMS& Params);


public:
	static CRenderPass_Bloom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint2 vViewportSize);
	virtual void Free() override;
};
END
