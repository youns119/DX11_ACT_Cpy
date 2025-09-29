#include "RenderTarget.h"

#include "Shader.h"
#ifdef _DEBUG
#include "VIBuffer_Rect.h"
#endif // _DEBUG


CRenderTarget::CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize, _uint iMipLevel)
{
	D3D11_TEXTURE2D_DESC TextureDesc {};
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = iMipLevel;
	TextureDesc.ArraySize = iArraySize;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적 */
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	m_vClearColor = vClearColor;

	m_iWidth = iWidth;
	m_iHeight = iHeight;

	//m_pContext->GenerateMips(m_pSRV);

	return S_OK;
}

void CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV, reinterpret_cast<_float*>(&m_vClearColor));
}

HRESULT CRenderTarget::Bind_RT_To_Shader(CShader* pShader, const _char* pConstantName) const
{
	return pShader->Bind_SRV(pConstantName, m_pSRV);
}

void CRenderTarget::Copy_Resource(ID3D11Texture2D* pTexture2D)
{
	m_pContext->CopyResource(pTexture2D, m_pTexture2D);
}

#ifdef _DEBUG
HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	_uint iNumViewport = 1;
	D3D11_VIEWPORT ViewportDesc {};

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	_float fWindowCoordX = fX - ViewportDesc.Width * 0.5f;
	_float fWindowCoordY = -fY + ViewportDesc.Height * 0.5f;

	_matrix WorldMatrix = XMMatrixScaling(fSizeX, fSizeY, 1.f);
	WorldMatrix.r[3] = XMVectorSet(fWindowCoordX, fWindowCoordY, 0.f, 1.f);

	XMStoreFloat4x4(&m_WorldMatrix_Debug, WorldMatrix);

	return S_OK;
}

HRESULT CRenderTarget::Render_Debug(CShader* pShader, CVIBuffer_Rect* pVIBuffer) const
{
	D3D11_TEXTURE2D_DESC TextureDesc;
	m_pTexture2D->GetDesc(&TextureDesc);

	return 1 < TextureDesc.ArraySize ?
		Render_Debug_Array(pShader, pVIBuffer, TextureDesc.ArraySize) :
		Render_Debug_Single(pShader, pVIBuffer);
}

HRESULT CRenderTarget::Render_Debug_Single(CShader* pShader, CVIBuffer_Rect* pVIBuffer) const
{
	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix_Debug)))
		return E_FAIL;

	if (FAILED(pShader->Bind_SRV("g_Texture", m_pSRV)))
		return E_FAIL;

	pShader->Begin(static_cast<_uint>(SHADER_PASS::SINGLE));

	if (FAILED(pVIBuffer->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTarget::Render_Debug_Array(CShader* pShader, CVIBuffer_Rect* pVIBuffer, _uint iArraySize) const
{
	if (FAILED(pShader->Bind_SRV("g_TextureArray", m_pSRV)))
		return E_FAIL;

	_mat ShiftedWorldMatrix = m_WorldMatrix_Debug;
	_float fHeight = ShiftedWorldMatrix.up.Length();

	for (_uint i = 0; i < iArraySize; ++i)
	{
		ShiftedWorldMatrix.position.y = m_WorldMatrix_Debug.m[3][1] - i * fHeight;

		if (FAILED(pShader->Bind_RawValue("g_iArrayIndex", &i, sizeof(_uint))))
			return E_FAIL;

		if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", ShiftedWorldMatrix._float4x4())))
			return E_FAIL;

		pShader->Begin(static_cast<_uint>(SHADER_PASS::ARRAY));

		if (FAILED(pVIBuffer->Bind_InputAssembler()))
			return E_FAIL;
		if (FAILED(pVIBuffer->Render()))
			return E_FAIL;
	}

	return S_OK;
}
#endif

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize, _uint iMipLevel)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iWidth, iHeight, ePixelFormat, vClearColor, iArraySize, iMipLevel)))
	{
		MSG_BOX("Failed to Create : CRenderTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	super::Free();

	//SaveDDSTextureToFile(m_pContext, m_pTexture2D, TEXT("../Bin/Diffuse.dds")); // 렌더타겟이 아직 3개 중에 어떤놈이 마지막으로 삭제되는지 알수 가 없기 때문에 원하는 놈 빼고 전부 지워야 한다.

	Safe_Release(m_pRTV);
	Safe_Release(m_pSRV);
	Safe_Release(m_pTexture2D);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
