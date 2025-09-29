#pragma once

#include "Base.h"
#include "Renderer_Params.h"

BEGIN(Engine)

class CGameInstance;
class CShader;
class CVIBuffer_Rect;

class CRenderPass_RadialBlur final : public CBase
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

		const RADIALBLUR_PARAMS* pRadialBlurParams {};
	};

private:
	CRenderPass_RadialBlur();
	virtual ~CRenderPass_RadialBlur() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint2 vViewportSize);
	HRESULT Render(const RENDERPASS_PARAMS& Params);

#ifdef _DEBUG
	//HRESULT Ready_DebugRender(vector<vector<_wstring>>& DebugMRTTagsByIndex, _uint2 vViewportSize);
#endif

private:
	CGameInstance* m_pGameInstance { nullptr };

	CShader* m_pShaderCom_Blur { nullptr };

private:
	HRESULT Bind_Matrices(CShader* pShader, const _float4x4* pWorldMatrix, const _float4x4* pViewMatrix, const _float4x4* pProjMatrix);

public:
	static CRenderPass_RadialBlur* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint2 vViewportSize);
	virtual void Free() override;

};
END
