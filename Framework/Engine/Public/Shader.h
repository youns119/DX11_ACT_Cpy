#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& Prototype);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pVertexElements, _uint iNumElements);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Begin(_uint iPassIndex);
	HRESULT Bind_RawValue(const _char* pContantName, const void* pData, _uint iByteCount);
	HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix);
	HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4& Matrix);
	HRESULT Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices);
	HRESULT Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumSRVs);
	HRESULT Bind_Sampler(const _char* pConstantName, ID3D11SamplerState* pSamplerState);

//private:
public:
	ID3DX11Effect*		m_pEffect{ nullptr };

	std::vector<ID3D11InputLayout*>		m_InputLayouts;
	_uint								m_iNumPasses{ 0 }; /* 이 변수 필요하냐? 좀 더 두고본다 */

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pVertexElements, _uint iNumElements);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
