#include "Font_Manager.h"

#include "CustomFont.h"
#include "GameInstance.h"

CFont_Manager::CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CFont_Manager::Initialize()
{
    _uint iTextureWidth = 40;
    _uint iTextureHeight = 40;

    if (FAILED(Ready_FontTexture(iTextureWidth, iTextureHeight)))
        return E_FAIL;

    if (FAILED(Ready_DepthStencilBuffer(iTextureWidth, iTextureHeight, &m_pFontDSV)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

HRESULT CFont_Manager::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
    if (nullptr != Find_Font(strFontTag))
        return E_FAIL;

    CCustomFont* pFont = CCustomFont::Create(m_pDevice, m_pContext, pFontFilePath);
    if (nullptr == pFont)
        return E_FAIL;

    m_Fonts.emplace(strFontTag, pFont);

    return S_OK;
}

HRESULT CFont_Manager::Render(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, const _float fScale, SORTING eSorting, SpriteEffects effects, _float layerDepth)
{
    CCustomFont* pFont = Find_Font(strFontTag);
    if (nullptr == pFont)
        return E_FAIL;

    return pFont->Render(pText, vPosition, vColor, fRotation, vOrigin, fScale, eSorting, effects, layerDepth);
}

HRESULT CFont_Manager::Render_In_World(const _wstring& strFontTag, const _tchar* pText, const _float4x4& WorldMatrix, const _float2& vPosition, _fvector vColor, const _float2& vOrigin, const _float fScale, SORTING eSorting)
{
    // chagne render target,

    // render font

    // reset render target
    
    // render Rect

    //HRESULT CRenderTarget_Manager::Begin_MRT(const _wstring & strMRTTag, ID3D11DepthStencilView * pDSV)
	array<ID3D11RenderTargetView*, 8> OldRenderTargets {};
	OldRenderTargets.fill(nullptr);
	ID3D11DepthStencilView* pOldDSV { nullptr };

	m_pContext->OMGetRenderTargets((_uint)OldRenderTargets.size(), OldRenderTargets.data(), &pOldDSV);

    _float4 vClearColor { 0.f, 0.f, 0.f, 1.f };
    m_pContext->ClearRenderTargetView(m_pFontRTV, reinterpret_cast<_float*>(&vClearColor));
	m_pContext->ClearDepthStencilView(m_pFontDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	m_pContext->OMSetRenderTargets(1, &m_pFontRTV, m_pFontDSV);

    Render(strFontTag, pText, vPosition, vColor, 0.f, vOrigin, fScale, eSorting);

    ////HRESULT CRenderTarget_Manager::End_MRT()
	m_pContext->OMSetRenderTargets((_uint)OldRenderTargets.size(), OldRenderTargets.data(), pOldDSV);


	Safe_Release(pOldDSV);
    for(auto& RTV : OldRenderTargets)
		Safe_Release(RTV);

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_SRV("g_Texture", m_pFontSRV)))
		return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CFont_Manager::Render_In_Screen(const _wstring& strFontTag, const _tchar* pText, const _float4x4& WorldMatrix, const _float4x4& ViewMatrix, const _float4x4& ProjMatrix, const _float2& vPosition, _fvector vColor, const _float2& vOrigin, const _float fScale, SORTING eSorting)
{
    // chagne render target,

    // render font

    // reset render target

    // render Rect

    //HRESULT CRenderTarget_Manager::Begin_MRT(const _wstring & strMRTTag, ID3D11DepthStencilView * pDSV)
    array<ID3D11RenderTargetView*, 8> OldRenderTargets{};
    OldRenderTargets.fill(nullptr);
    ID3D11DepthStencilView* pOldDSV{ nullptr };

    m_pContext->OMGetRenderTargets((_uint)OldRenderTargets.size(), OldRenderTargets.data(), &pOldDSV);

    _float4 vClearColor{ 0.f, 0.f, 0.f, 1.f };
    m_pContext->ClearRenderTargetView(m_pFontRTV, reinterpret_cast<_float*>(&vClearColor));
    m_pContext->ClearDepthStencilView(m_pFontDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    m_pContext->OMSetRenderTargets(1, &m_pFontRTV, m_pFontDSV);

    Render(strFontTag, pText, vPosition, vColor, 0.f, vOrigin, fScale, eSorting);

    ////HRESULT CRenderTarget_Manager::End_MRT()
    m_pContext->OMSetRenderTargets((_uint)OldRenderTargets.size(), OldRenderTargets.data(), pOldDSV);

    Safe_Release(pOldDSV);
    for (auto& RTV : OldRenderTargets)
        Safe_Release(RTV);

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_SRV("g_Texture", m_pFontSRV)))
        return E_FAIL;

    m_pShaderCom->Begin(0);
    
    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    return S_OK;
}

_vector CFont_Manager::Measure_FontString(const _wstring& strFontTag, const _tchar* pText, _bool bIgnoreWhitespce)
{
    CCustomFont* pFont = Find_Font(strFontTag);
    if (nullptr != pFont)
        return pFont->Measure_FontString(pText, bIgnoreWhitespce);
    else
        return XMVectorZero();
}

CCustomFont* CFont_Manager::Find_Font(const _wstring& strFrontTag)
{
    auto iter = m_Fonts.find(strFrontTag);
    if (iter == m_Fonts.end())
        return nullptr;

    return iter->second;
}

HRESULT CFont_Manager::Ready_Components()
{
    m_pShaderCom = CShader::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Font.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    m_pVIBufferCom = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    return S_OK;

}

HRESULT CFont_Manager::Ready_DepthStencilBuffer(_uint iWidth, _uint iHeight, ID3D11DepthStencilView** ppOut)
{
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

    D3D11_TEXTURE2D_DESC	TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    TextureDesc.Width = iWidth;
    TextureDesc.Height = iHeight;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, ppOut)))
        return E_FAIL;

    Safe_Release(pDepthStencilTexture);

    return S_OK;

}

HRESULT CFont_Manager::Ready_FontTexture(_uint iWidth, _uint iHeight)
{
    D3D11_TEXTURE2D_DESC TextureDesc {};
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    TextureDesc.Width = iWidth;
    TextureDesc.Height = iHeight;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_DEFAULT; /* Á¤Àû */
    TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pFontSRV)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pFontRTV)))
        return E_FAIL;

    return S_OK;
}

CFont_Manager* CFont_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFont_Manager* pInstance = new CFont_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CFont_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFont_Manager::Free()
{
    super::Free();

    for (auto& Pair : m_Fonts)
        Safe_Release(Pair.second);

    m_Fonts.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
    
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);

    Safe_Release(m_pFontDSV);
    Safe_Release(m_pFontSRV);
    Safe_Release(m_pFontRTV);
    Safe_Release(m_pTexture2D);
}
