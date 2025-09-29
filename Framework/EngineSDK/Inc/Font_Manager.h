#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameInstance;
class CCustomFont;
class CVIBuffer_Rect;
class CShader;

class CFont_Manager final : public CBase
{
	using super = CBase;

private:
	CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Render(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f),
		_float fRotation = 0.f, const _float2& vOrigin = _float2(0.f, 0.f), const _float fScale = 1.f, SORTING eSorting = SORTING::NONE, SpriteEffects effects = SpriteEffects_None, _float layerDepth = 0);
	HRESULT Render_In_World(const _wstring& strFontTag, const _tchar* pText, const _float4x4& WorldMatrix, const _float2& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f),
		const _float2& vOrigin = _float2(0.f, 0.f), const _float fScale = 1.f, SORTING eSorting = SORTING::LEFT);
	HRESULT Render_In_Screen(const _wstring& strFontTag, const _tchar* pText, const _float4x4& WorldMatrix, const _float4x4& ViewMatrix, const _float4x4& ProjMatrix, const _float2& vPosition, _fvector vColor, const _float2& vOrigin, const _float fScale, SORTING eSorting);
	_vector Measure_FontString(const _wstring& strFontTag, const _tchar* pText, _bool bIgnoreWhitespce = true);

private:
	ID3D11Device*			m_pDevice { nullptr };
	ID3D11DeviceContext*	m_pContext { nullptr };

	map<const _wstring, CCustomFont*>		m_Fonts;

	CGameInstance* m_pGameInstance { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom { nullptr };
	CShader* m_pShaderCom { nullptr };

	ID3D11DepthStencilView* m_pFontDSV { nullptr };
	ID3D11RenderTargetView* m_pFontRTV { nullptr };
	ID3D11ShaderResourceView* m_pFontSRV { nullptr };
	ID3D11Texture2D* m_pTexture2D { nullptr };

private:
	CCustomFont* Find_Font(const _wstring& strFrontTag);

	HRESULT Ready_Components();
	HRESULT Ready_DepthStencilBuffer(_uint iWidth, _uint iHeight, ID3D11DepthStencilView** ppOut);
	HRESULT Ready_FontTexture(_uint iWidth, _uint iHeight);


public:
	static CFont_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
