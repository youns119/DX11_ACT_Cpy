#include "Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CTexture::CTexture(const CTexture& Prototype)
    : CComponent{ Prototype }
	, m_iNumSRVs{ Prototype.m_iNumSRVs }
	, m_SRVs{ Prototype.m_SRVs }
	, m_vResolution{ Prototype.m_vResolution }
{
	for (auto& pSRV : m_SRVs)
		Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
	m_iNumSRVs = iNumTextures;

	_tchar szEXT[MAX_PATH]{};
	_wsplitpath_s(pTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);


	for (size_t i = 0; i < m_iNumSRVs; ++i)
	{
		ID3D11ShaderResourceView* pSRV{ nullptr };

		_tchar szTextureFilePath[MAX_PATH] = TEXT("");

		wsprintf(szTextureFilePath, pTextureFilePath, i);

		HRESULT hr{};

		// By DirectXTK
		//if (0 == lstrcmp(szEXT, TEXT(".dds")))
		//{
		//	hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
		//}
		//else if (0 == lstrcmp(szEXT, TEXT(".tga")))
		//{
		//	hr = E_FAIL;
		//}
		//else
		//{
		//	hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
		//}

				
		//m_pContext->GenerateMips(pSRV); /* 메인 스레드가 아님! */


		// By DirectXTex
		if (0 == lstrcmp(szEXT, TEXT(".dds")))
		{
			ScratchImage Image{};
			TexMetadata MetaData{};
			hr = LoadFromDDSFile(szTextureFilePath, DDS_FLAGS_NONE, &MetaData, Image);
			if (FAILED(hr))
				return E_FAIL;

			m_vResolution.x = static_cast<_float>(MetaData.width);
			m_vResolution.y = static_cast<_float>(MetaData.height);

			hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
		}
		else if (0 == lstrcmp(szEXT, TEXT(".tga")))
		{
			hr = E_FAIL;
		}
		else
		{
			ScratchImage Image {};
			ScratchImage MipChain {};
			TexMetadata MetaData {};
			hr = LoadFromWICFile(szTextureFilePath, WIC_FLAGS_NONE, &MetaData, Image);
			//hr = LoadFromWICFile(szTextureFilePath, WIC_FLAGS_FORCE_RGB, &MetaData, Image);
			if (FAILED(hr))
				return E_FAIL;
			hr = GenerateMipMaps(Image.GetImages(), Image.GetImageCount(), Image.GetMetadata(), TEX_FILTER_DEFAULT, 0, MipChain);
			if (FAILED(hr))
				return E_FAIL;

			m_vResolution.x = static_cast<_float>(MetaData.width);
			m_vResolution.y = static_cast<_float>(MetaData.height);

			hr = CreateShaderResourceView(m_pDevice, MipChain.GetImages(), MipChain.GetImageCount(), MipChain.GetMetadata(), &pSRV);

		}

		if (FAILED(hr))
			return E_FAIL;


		m_SRVs.push_back(pSRV);
	}

    return S_OK;
}

HRESULT CTexture::Initialize_Prototype_With_InitialData(const void* pData, DXGI_FORMAT eFormat, _uint iFormatSize, _uint iWidth, _uint iHeight, _uint iArraySize)
{
	m_iNumSRVs = 1;

	D3D11_TEXTURE2D_DESC TextureDesc = {};
	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = iArraySize;
	TextureDesc.Format = eFormat;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	vector<D3D11_SUBRESOURCE_DATA> InitialDataArray;
	InitialDataArray.reserve(iArraySize);
	for (_uint i = 0; i < iArraySize; ++i)
	{
		D3D11_SUBRESOURCE_DATA InitialData {};
		InitialData.pSysMem = static_cast<const _char*>(pData) + i * iFormatSize * iWidth * iHeight;
		InitialData.SysMemPitch = iFormatSize * iWidth;

		InitialDataArray.push_back(InitialData);
	}

	ID3D11Texture2D* pTexture2D = nullptr;
	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, InitialDataArray.data(), &pTexture2D)))
		return E_FAIL;

	ID3D11ShaderResourceView* pSRV = nullptr;
	HRESULT hr = m_pDevice->CreateShaderResourceView(pTexture2D, nullptr, &pSRV);
	pTexture2D->Release();

	if (FAILED(hr))
		return E_FAIL;

	m_SRVs.push_back(pSRV);
	return S_OK;
}

HRESULT CTexture::Initialize_Prototype_Texture3D_With_InitialData(const void* pData, DXGI_FORMAT eFormat, _uint iFormatSize, _uint iWidth, _uint iHeight, _uint iDepth)
{
	m_iNumSRVs = 1;

	D3D11_TEXTURE3D_DESC TextureDesc = {};
	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.Depth = iDepth;
	TextureDesc.MipLevels = 1;
	TextureDesc.Format = eFormat;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitialData {};
	InitialData.pSysMem = pData;
	InitialData.SysMemPitch = iFormatSize * iWidth;
	InitialData.SysMemSlicePitch = iFormatSize * iWidth * iHeight;


	ID3D11Texture3D* pTexture3D = nullptr;
	if (FAILED(m_pDevice->CreateTexture3D(&TextureDesc, &InitialData, &pTexture3D)))
		return E_FAIL;

	ID3D11ShaderResourceView* pSRV = nullptr;
	HRESULT hr = m_pDevice->CreateShaderResourceView(pTexture3D, nullptr, &pSRV);
	pTexture3D->Release();

	if (FAILED(hr))
		return E_FAIL;

	m_SRVs.push_back(pSRV);
	return S_OK;


	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CTexture::Bind_Texture_To_Shader(CShader* pShader, const _char* pConstantName, _uint iIndex)
{
	if (iIndex >= m_iNumSRVs)
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_SRVs[iIndex]);
}

HRESULT CTexture::Bind_Textures_To_Shader(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_SRVs(pConstantName, m_SRVs.data(), (_uint)m_SRVs.size());
}

#ifdef _DEBUG
void CTexture::Set_Texture(const _tchar* pTextureFilePath, _uint iNumTextures)
{
	for (auto& pSRV : m_SRVs)
		Safe_Release(pSRV);
	m_SRVs.clear();

	Initialize_Prototype(pTextureFilePath, iNumTextures);
}
#endif

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed To Create : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CTexture* CTexture::Create_With_InitailData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const void* pData, DXGI_FORMAT eFormat, _uint iFormatSize, _uint iWidth, _uint iHeight, _uint iArraySize)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype_With_InitialData(pData, eFormat, iFormatSize, iWidth, iHeight, iArraySize)))
	{
		MSG_BOX("Failed To Create : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CTexture* CTexture::Create_Texture3D_With_InitailData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const void* pData, DXGI_FORMAT eFormat, _uint iFormatSize, _uint iWidth, _uint iHeight, _uint iDepth)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype_Texture3D_With_InitialData(pData, eFormat, iFormatSize, iWidth, iHeight, iDepth)))
	{
		MSG_BOX("Failed To Create : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	CComponent::Free();

	for (auto& pSRV : m_SRVs)
		Safe_Release(pSRV);
}
