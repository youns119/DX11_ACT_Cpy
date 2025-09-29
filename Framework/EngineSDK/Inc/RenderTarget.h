#pragma once

#include "Base.h"

/* 렌더타겟을 의미하는 컴객체를 들고 있을거다. */

BEGIN(Engine)

class CShader;
class CVIBuffer_Rect;

class CRenderTarget final : public CBase
{
#ifdef _DEBUG
	enum class SHADER_PASS { SINGLE, ARRAY, MAX };
#endif

	using super = CBase;

private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const { return m_pRTV; }
	_uint2 Get_Resolution() const { return _uint2(m_iWidth, m_iHeight); }

	HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize, _uint iMipLevel);
	void Clear();
	HRESULT Bind_RT_To_Shader(CShader* pShader, const _char* pConstantName) const;
	void Copy_Resource(ID3D11Texture2D* pTexture2D);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(CShader* pShader, CVIBuffer_Rect* pVIBuffer) const;
#endif // _DEBUG


private:
	ID3D11Device*				m_pDevice { nullptr };
	ID3D11DeviceContext*		m_pContext { nullptr };

	ID3D11Texture2D*				m_pTexture2D { nullptr }; // 얘는 멤버변수로 저장하는 의미가 있나? 만들때만 잠깐 쓰고 release 하면 안돼?
	ID3D11RenderTargetView*			m_pRTV { nullptr };
	ID3D11ShaderResourceView*		m_pSRV { nullptr };

	_float4 m_vClearColor {};
	_uint m_iWidth { 0 };
	_uint m_iHeight { 0 };

#ifdef _DEBUG
private:
	_float4x4 m_WorldMatrix_Debug {};
#endif

#ifdef _DEBUG
private:
	HRESULT Render_Debug_Single(CShader* pShader, CVIBuffer_Rect* pVIBuffer) const;
	HRESULT Render_Debug_Array(CShader* pShader, CVIBuffer_Rect* pVIBuffer, _uint iArraySize) const;
#endif

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize, _uint iMipLevel);
	virtual void Free() override;
};

END
