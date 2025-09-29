#pragma once

#include "Component.h"

BEGIN(Engine)

class CShader;

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& Prototype);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize_Prototype_With_InitialData(const void* pData, DXGI_FORMAT eFormat, _uint iFormatSize, _uint iWidth, _uint iHeight, _uint iArraySize);
	virtual HRESULT Initialize_Prototype_Texture3D_With_InitialData(const void* pData, DXGI_FORMAT eFormat, _uint iFormatSize, _uint iWidth, _uint iHeight, _uint iDepth);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Bind_Texture_To_Shader(CShader* pShader, const _char* pConstantName, _uint iIndex = 0);
	HRESULT Bind_Textures_To_Shader(CShader* pShader, const _char* pConstantName);

	const ID3D11ShaderResourceView* Get_SRV(_int iIndex) const { return m_SRVs[iIndex]; }

	const _float2& Get_Resolution() const { return m_vResolution; }

#ifdef _DEBUG
public:
	void	Set_Texture(const _tchar* pTextureFilePath, _uint iNumTextures = 1);
#endif

private:
	_uint		m_iNumSRVs{ 0 };
	std::vector<ID3D11ShaderResourceView*>		m_SRVs;

	_float2		m_vResolution{};

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	static CTexture* Create_With_InitailData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const void* pData, DXGI_FORMAT eFormat, _uint iFormatSize, _uint iWidth, _uint iHeight, _uint iArraySize);
	static CTexture* Create_Texture3D_With_InitailData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const void* pData, DXGI_FORMAT eFormat, _uint iFormatSize, _uint iWidth, _uint iHeight, _uint iDepth);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

