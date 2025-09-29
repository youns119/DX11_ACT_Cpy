#pragma once
#include "Base.h"

BEGIN(Engine)
const int NUM_BUFFERS = 3;
class CShader_Compute final : public CBase
{
public:
	enum CBUFFER_TYPE {CBUFFER_PARTICLE, CBUFFER_MESH, TYPE_END};
private:
	CShader_Compute(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CShader_Compute() = default;

public:
	virtual HRESULT Initialize(const _tchar* pShaderFilePath, const _char* pFunctionName, const _char* pInitializeFunctionName, _uint iElementSize, _uint iNumElements, DWORD dwNumDataBytes, void* pInitData);
	HRESULT Compute(_uint iPassIndex, _uint iNumViews, void* pConstantData, DWORD dwNumDataBytes, ID3D11Buffer* pOriginalBuffer);
	HRESULT Init_Data(_uint iPassIndex, _uint iNumViews, void* pConstantData, DWORD dwNumDataBytes, ID3D11Buffer* pOriginalBuffer);
	HRESULT Change_Function(const _tchar* pShaderFilePath, const _char* pFunctionName);
private:
	HRESULT CreateComputeShader(const _tchar* pShaderFilePath, const _char* pFunctionName, _bool bIsInitializeCompute);
	HRESULT CreateBuffers(void* pInitData, DWORD dwNumDataBytes);
	HRESULT CopyResult(ID3D11Buffer* pOriginalBuffer);

private:
	ID3D11Device*						m_pDevice{ nullptr };
	ID3D11DeviceContext*				m_pContext{ nullptr };

	ID3D11ComputeShader*				m_pComputeShader = { nullptr };
	ID3D11ComputeShader*				m_pInitComputeShader = { nullptr };
	ID3D11Buffer*						m_pConstantBuffer = { nullptr };
	ID3D11Buffer*						m_pSRVBuffer = { nullptr };
	ID3D11Buffer*						m_pUAVBuffer = { nullptr };
	ID3D11Buffer*						m_pStagingBuffer[NUM_BUFFERS]; // GPU로부터 읽어오는 역할

	ID3D11ShaderResourceView*			m_pSRV = { nullptr };
	ID3D11UnorderedAccessView*			m_pUAV = { nullptr };

	_uint								m_iElementSize{};
	_uint								m_iNumElements{};

	_uint								m_iFrameIndex{};



public:
	static CShader_Compute* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const _char* pFunctionName, const _char* pInitializeFunctionName, _uint iElementSize, _uint iNumElements, DWORD dwNumDataBytes, void* pInitData);
	virtual void Free() override;
};

END