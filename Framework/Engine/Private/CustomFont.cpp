#include "CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const _tchar* pFontFilePath)
{
	m_pFont = new SpriteFont(m_pDevice, pFontFilePath);
	m_pBatch = new SpriteBatch(m_pContext);

	m_DepthDesc.DepthEnable = TRUE;
	m_DepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	m_DepthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	m_DepthDesc.StencilEnable = FALSE;

	m_pDevice->CreateDepthStencilState(&m_DepthDesc, &m_pDepthState);
	m_pContext->OMSetDepthStencilState(m_pDepthState, 0);

	m_BlendDesc.RenderTarget[0].BlendEnable = TRUE;  // 블렌딩 활성화
	m_BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;  // 소스 색상의 알파 값 사용
	m_BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;  // 배경과 알파 블렌딩
	m_BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;  // 블렌딩 연산 방식
	m_BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	m_BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	m_BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	m_BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_BlendDesc.AlphaToCoverageEnable = FALSE;  // 폰트에는 불필요

	m_pDevice->CreateBlendState(&m_BlendDesc, &m_pBlendState);
	m_pContext->OMSetBlendState(m_pBlendState, nullptr, 0);

	return S_OK;
}

HRESULT CCustomFont::Render(const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, const _float fScale, SORTING eSorting, SpriteEffects effects, _float layerDepth)
{
	if (nullptr == m_pFont ||
		nullptr == m_pBatch)
		return E_FAIL;

	_vec2 vStringSize= m_pFont->MeasureString(pText);
	_float2 vSortedOrigin = vOrigin;

	m_pBatch->Begin(SpriteSortMode_Deferred, m_pBlendState, nullptr, m_pDepthState);

	switch (eSorting)
	{
	case SORTING::RIGHT:
		vSortedOrigin.x = vStringSize.x;
		vSortedOrigin.y = vStringSize.y * 0.5f;
		m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, fRotation, vSortedOrigin, fScale, effects, layerDepth);
		break;

	case SORTING::MIDDLE:
		XMStoreFloat2(&vSortedOrigin, _vec2(vStringSize * 0.5f)._vector());
		m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, fRotation, vSortedOrigin, fScale, effects, layerDepth);
		break;

	case SORTING::LEFT:
		vSortedOrigin = { 0.f, vStringSize.y * 0.5f };
		m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, fRotation, vSortedOrigin, fScale, effects, layerDepth);
		break;

	default:
		m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, fRotation, vOrigin, fScale, effects, layerDepth);
		break;
	}
	/* 뷰포트 상의 직교투영의 형태로 그려낸다. */

	m_pBatch->End();

	return S_OK;
}

_vector CCustomFont::Measure_FontString(const _tchar* pText, _bool bIgnoreWhitespce)
{
	return m_pFont->MeasureString(pText, bIgnoreWhitespce);
}

CCustomFont* CCustomFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath)
{
	CCustomFont* pInstance = new CCustomFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pFontFilePath)))
	{
		MSG_BOX("Failed to Created : CCustomFont");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustomFont::Free()
{
	super::Free();

	Safe_Delete(m_pBatch);
	Safe_Delete(m_pFont);

	Safe_Release(m_pDepthState);
	Safe_Release(m_pBlendState);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
