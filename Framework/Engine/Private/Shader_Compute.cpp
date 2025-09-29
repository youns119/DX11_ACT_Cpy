#include "Shader_Compute.h"
#include "GameInstance.h"
#include "VIBuffer_InstancingParticle.h"

CShader_Compute::CShader_Compute(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CShader_Compute::Initialize(const _tchar* pShaderFilePath, const _char* pFunctionName, const _char* pInitializeFunctionName, _uint iElementSize, _uint iNumElements, DWORD dwNumDataBytes, void* pInitData)
{
	m_iElementSize = iElementSize;
	m_iNumElements = iNumElements;
	if(FAILED(CreateComputeShader(pShaderFilePath, pFunctionName, false)))
		return E_FAIL;
	if(FAILED(CreateComputeShader(pShaderFilePath, pInitializeFunctionName, true)))
		return E_FAIL;
	if (FAILED(CreateBuffers(pInitData, dwNumDataBytes)))
		return E_FAIL;
	return S_OK;
}

HRESULT CShader_Compute::Compute(_uint iPassIndex, _uint iNumViews, void* pConstantData, DWORD dwNumDataBytes, ID3D11Buffer* pOriginalBuffer)
{
	if (nullptr == pOriginalBuffer || nullptr == m_pComputeShader)
		return E_FAIL;

	m_pContext->CSSetShader(m_pComputeShader, nullptr, 0);
	m_pContext->CSSetShaderResources(0, 1, &m_pSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 1, &m_pUAV, nullptr);
	if (m_pConstantBuffer && pConstantData)
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		m_pContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		memcpy(MappedResource.pData, pConstantData, dwNumDataBytes);
		m_pContext->Unmap(m_pConstantBuffer, 0);
	}
	m_pContext->CSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	m_pContext->Dispatch((m_iNumElements + 256 - 1) / 256, 1, 1);

	m_pContext->CSSetShader(nullptr, nullptr, 0);
	ID3D11UnorderedAccessView* ppUAViewnullptr[1] = { nullptr };
	m_pContext->CSSetUnorderedAccessViews(0, 1, ppUAViewnullptr, nullptr);
	ID3D11ShaderResourceView* ppSRVnullptr[1] = { nullptr };
	m_pContext->CSSetShaderResources(0, 1, ppSRVnullptr);
	ID3D11Buffer* ppCBnullptr[1] = { nullptr };
	m_pContext->CSSetConstantBuffers(0, 1, ppCBnullptr);

	
	m_iFrameIndex++;
	if(FAILED(CopyResult(pOriginalBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShader_Compute::Init_Data(_uint iPassIndex, _uint iNumViews, void* pConstantData, DWORD dwNumDataBytes, ID3D11Buffer* pOriginalBuffer)
{
	if (nullptr == m_pInitComputeShader || nullptr == pOriginalBuffer)
		return E_FAIL;

	m_pContext->CSSetShader(m_pInitComputeShader, nullptr, 0);
	m_pContext->CSSetShaderResources(0, 1, &m_pSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 1, &m_pUAV, nullptr);
	if (m_pConstantBuffer && pConstantData)
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		m_pContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		memcpy(MappedResource.pData, pConstantData, dwNumDataBytes);
		m_pContext->Unmap(m_pConstantBuffer, 0);
	}
	m_pContext->CSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	m_pContext->Dispatch((m_iNumElements + 256 - 1) / 256, 1, 1);

	m_pContext->CSSetShader(nullptr, nullptr, 0);
	ID3D11UnorderedAccessView* ppUAViewnullptr[1] = { nullptr };
	m_pContext->CSSetUnorderedAccessViews(0, 1, ppUAViewnullptr, nullptr);
	ID3D11ShaderResourceView* ppSRVnullptr[1] = { nullptr };
	m_pContext->CSSetShaderResources(0, 1, ppSRVnullptr);
	ID3D11Buffer* ppCBnullptr[1] = { nullptr };
	m_pContext->CSSetConstantBuffers(0, 1, ppCBnullptr);

	CopyResult(pOriginalBuffer);
	m_iFrameIndex = 0;
	return S_OK;
}

HRESULT CShader_Compute::Change_Function(const _tchar* pShaderFilePath, const _char* pFunctionName)
{
	Safe_Release(m_pComputeShader);

// 컴퓨트 쉐이더 생성
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
#ifdef USE_STRUCTURED_BUFFERS
		"USE_STRUCTURED_BUFFERS", "1",
#endif

#ifdef TEST_DOUBLE
		"TEST_DOUBLE", "1",
#endif
		nullptr, nullptr
	};
	// We generally prefer to use the higher CS shader profile when possible as CS 5.0 is better performance on 11-class hardware
	LPCSTR pProfile = (m_pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";

	ID3DBlob* pErrorBlob = nullptr;
	ID3DBlob* pBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(pShaderFilePath, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, pFunctionName, pProfile,
		dwShaderFlags, 0, &pBlob, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

		Safe_Release(pErrorBlob);
		Safe_Release(pBlob);

		return hr;
	}

	hr = m_pDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pComputeShader);

	Safe_Release(pErrorBlob);
	Safe_Release(pBlob);

#if defined(_DEBUG) || defined(PROFILE)
	if (SUCCEEDED(hr))
		m_pComputeShader->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(pFunctionName), pFunctionName);
#endif
	

	return hr;
}

HRESULT CShader_Compute::CreateComputeShader(const _tchar* pShaderFilePath, const _char* pFunctionName, _bool bIsInitializeCompute)
{
	// 컴퓨트 쉐이더 생성
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
#ifdef USE_STRUCTURED_BUFFERS
		"USE_STRUCTURED_BUFFERS", "1",
#endif

#ifdef TEST_DOUBLE
		"TEST_DOUBLE", "1",
#endif
		nullptr, nullptr
	};
	// We generally prefer to use the higher CS shader profile when possible as CS 5.0 is better performance on 11-class hardware
	LPCSTR pProfile = (m_pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";

	ID3DBlob* pErrorBlob = nullptr;
	ID3DBlob* pBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(pShaderFilePath, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, pFunctionName, pProfile,
		dwShaderFlags, 0, &pBlob, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

		Safe_Release(pErrorBlob);
		Safe_Release(pBlob);

		return hr;
	}

	if (true == bIsInitializeCompute)
	{
		hr = m_pDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pInitComputeShader);

		Safe_Release(pErrorBlob);
		Safe_Release(pBlob);

#if defined(_DEBUG) || defined(PROFILE)
		if (SUCCEEDED(hr))
			m_pInitComputeShader->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(pFunctionName), pFunctionName);
#endif
	}
	else
	{
		hr = m_pDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pComputeShader);

		Safe_Release(pErrorBlob);
		Safe_Release(pBlob);

#if defined(_DEBUG) || defined(PROFILE)
		if (SUCCEEDED(hr))
			m_pComputeShader->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(pFunctionName), pFunctionName);
#endif
	}
	

	return hr;
}
HRESULT CShader_Compute::CreateBuffers(void* pInitData, DWORD dwNumDataBytes)
{
	D3D11_BUFFER_DESC Buffer_Desc = {};
	Buffer_Desc.ByteWidth = m_iElementSize * m_iNumElements;
	Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	Buffer_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Buffer_Desc.CPUAccessFlags = 0;
	Buffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	Buffer_Desc.StructureByteStride = m_iElementSize;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pInitData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	if (FAILED(m_pDevice->CreateBuffer(&Buffer_Desc, &InitData, &m_pSRVBuffer)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC Desc_SRV = {};
	Desc_SRV.Format = DXGI_FORMAT_UNKNOWN;
	Desc_SRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	Desc_SRV.Buffer.FirstElement = 0;
	Desc_SRV.Buffer.NumElements = m_iNumElements;
	if (FAILED(m_pDevice->CreateShaderResourceView(m_pSRVBuffer, &Desc_SRV, &m_pSRV)))
		return E_FAIL;

	Buffer_Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	if (FAILED(m_pDevice->CreateBuffer(&Buffer_Desc, &InitData, &m_pUAVBuffer)))
		return E_FAIL;

	D3D11_UNORDERED_ACCESS_VIEW_DESC Desc_UAV = {};
	Desc_UAV.Format = DXGI_FORMAT_UNKNOWN;
	Desc_UAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	Desc_UAV.Buffer.FirstElement = 0;
	Desc_UAV.Buffer.NumElements = m_iNumElements;
	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pUAVBuffer, &Desc_UAV, &m_pUAV)))
		return E_FAIL;

	// 스테이징 버퍼 생성
	for (size_t i = 0; i < NUM_BUFFERS; i++)
	{
		D3D11_BUFFER_DESC StagingDesc = {};
		StagingDesc.ByteWidth = m_iElementSize * m_iNumElements;
		StagingDesc.Usage = D3D11_USAGE_STAGING;
		StagingDesc.BindFlags = 0;
		StagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		StagingDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		StagingDesc.StructureByteStride = m_iElementSize;
		if (FAILED(m_pDevice->CreateBuffer(&StagingDesc, NULL, &m_pStagingBuffer[i])))
			return E_FAIL;
	}

	// 상수 버퍼 생성
	D3D11_BUFFER_DESC ConstantDesc{};
	ConstantDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstantDesc.ByteWidth = dwNumDataBytes; // 16바이트(혹은 그 배수)로 맞춰줘야 한답니다
	ConstantDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED(m_pDevice->CreateBuffer(&ConstantDesc, NULL, &m_pConstantBuffer)))
		return E_FAIL;

	return S_OK;
}
HRESULT CShader_Compute::CopyResult(ID3D11Buffer* pOriginalBuffer)
{
	{
		_uint iBufferIndex = m_iFrameIndex % NUM_BUFFERS;
		m_pContext->CopyResource(m_pStagingBuffer[iBufferIndex], m_pUAVBuffer);
	}
	if (m_iFrameIndex >= NUM_BUFFERS)
	{
		_uint iBufferIndex = (m_iFrameIndex - 1 + NUM_BUFFERS) % NUM_BUFFERS;
		if (nullptr == m_pStagingBuffer[iBufferIndex] || nullptr == pOriginalBuffer)
			return E_FAIL;

		// map, unmap m_pStagingBuffer, pOriginalBuffer
		D3D11_MAPPED_SUBRESOURCE StagingResource = {};
		m_pContext->Map(m_pStagingBuffer[iBufferIndex], 0, D3D11_MAP_READ, 0, &StagingResource);
		D3D11_MAPPED_SUBRESOURCE OriginalResource{};
		m_pContext->Map(pOriginalBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &OriginalResource);

		//복사 작업
		memcpy(OriginalResource.pData, StagingResource.pData, m_iElementSize * m_iNumElements);

		//VTXPARTICLE* pOriginalVertices = static_cast<VTXPARTICLE*>(OriginalResource.pData);
		//VTXPARTICLE* pStagingVertices = static_cast<VTXPARTICLE*>(StagingResource.pData);
		//for (size_t i = 0; i < m_iNumElements; i++)
		//{		
		//	pOriginalVertices[i] = pStagingVertices[i];
		//	_int a = 10;
		//}

		m_pContext->Unmap(pOriginalBuffer, 0);
		m_pContext->Unmap(m_pStagingBuffer[iBufferIndex], 0);
		return S_OK;
	}
	else
		return E_FAIL;

}

CShader_Compute* CShader_Compute::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const _char* pFunctionName, const _char* pInitializeFunctionName, _uint iElementSize, _uint iNumElements, DWORD dwNumDataBytes, void* pInitData)
{
	CShader_Compute* pInstance = new CShader_Compute(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pShaderFilePath, pFunctionName, pInitializeFunctionName, iElementSize, iNumElements, dwNumDataBytes, pInitData)))
	{
		MSG_BOX("Failed To Create : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader_Compute::Free()
{
	__super::Free();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	Safe_Release(m_pComputeShader);
	Safe_Release(m_pInitComputeShader);

	Safe_Release(m_pConstantBuffer);
	Safe_Release(m_pSRVBuffer);
	Safe_Release(m_pUAVBuffer);
	for (size_t i = 0; i < NUM_BUFFERS; i++)
		Safe_Release(m_pStagingBuffer[i]);

	Safe_Release(m_pSRV);
	Safe_Release(m_pUAV);
}
