#pragma once

#include "Base.h"

BEGIN(Engine)

class CCustomFont final : public CBase
{
	using super = CBase;

private:
	CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCustomFont() = default;

public:
	HRESULT Initialize(const _tchar* pFontFilePath);
	HRESULT Render(
		const _tchar* pText,
		const _float2& vPosition, 
		_fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f),
		_float fRotation = 0.f,
		const _float2& vOrigin = _float2(0.f, 0.f),
		const _float fScale = 1.f,
		SORTING eSorting = SORTING::NONE,
		SpriteEffects effects = SpriteEffects_None, 
		_float layerDepth = 0);
	_vector Measure_FontString(const _tchar* pText, _bool bIgnoreWhitespce = true);

private:
	ID3D11Device*			m_pDevice { nullptr };
	ID3D11DeviceContext*	m_pContext { nullptr };

	SpriteBatch*		m_pBatch { nullptr };
	SpriteFont*			m_pFont { nullptr };

	ID3D11BlendState*		m_pBlendState = { nullptr };
	D3D11_BLEND_DESC		m_BlendDesc = {};

	ID3D11DepthStencilState* m_pDepthState = { nullptr };
	D3D11_DEPTH_STENCIL_DESC m_DepthDesc = {};

public:
	static CCustomFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath);
	virtual void Free() override;
};

END
