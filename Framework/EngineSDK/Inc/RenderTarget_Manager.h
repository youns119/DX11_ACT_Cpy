#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget;
class CShader;
class CVIBuffer_Rect;

class CRenderTarget_Manager final : public CBase
{
	using super = CBase;
private:
	CRenderTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderTarget(const _wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize, _uint iMipLevel);
	HRESULT Bind_RT_To_Shader(const _wstring& strRenderTargetTag, CShader* pShader, const _char* pConstantName);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strRenderTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, _bool isClear, ID3D11DepthStencilView* pDSV);
	HRESULT End_MRT();
	void Copy_RT_Resource(const _wstring& strRenderTargetTag, ID3D11Texture2D* pTexture2D);

#ifdef _DEBUG
	HRESULT Ready_RT_Debug(const _wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_MRT_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer);
#endif

private:
	ID3D11Device* m_pDevice { nullptr };
	ID3D11DeviceContext* m_pContext { nullptr };

	ID3D11RenderTargetView* m_pBackBuffer { nullptr };
	ID3D11DepthStencilView* m_pOriginDSV { nullptr };

	map<const _wstring, CRenderTarget*>				m_RenderTargets;
	map<const _wstring, vector<CRenderTarget*>>		m_MRTs;
	map<const _wstring, _uint2>						m_MRTResolutions;

	_uint m_iViewportWidth { 0 };
	_uint m_iViewportHeight { 0 };

private:
	CRenderTarget* Find_RenderTarget(const _wstring& strRenderTagetTag);
	vector<CRenderTarget*>& Find_MRT(const _wstring& strMRTTag);
	void SetUp_Viewport(_uint iWidth, _uint iHeight);


public:
	static CRenderTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
