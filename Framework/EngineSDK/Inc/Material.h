#pragma once

#include "Base.h"

BEGIN(Engine)

class CMaterial final : public CBase
{
	using super = CBase;

private:
	CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMaterial() = default;

public:
	HRESULT Initialize(const aiMaterial* pAIMaterial, const _string& strModelFilePath);
	HRESULT Initialize_From_File(const HANDLE& hFile, const _wstring& strModelFilePath);
	HRESULT Bind_Material(class CShader* pShader, aiTextureType eType, const _char* pConstantName, _uint iTextureIndex);

	const vector<pair<_uint, _tchar[MAX_PATH]>>& Get_TextureFiles_By_Type() { return m_TextureFilesByType; }
	_uint Get_ShaderPass() const { return m_iShaderPass; }
	void Set_ShaderPass(_uint iShaderPass) { m_iShaderPass = iShaderPass; }
	const vector<ID3D11ShaderResourceView*>& Get_MtrlTextures(_uint iType) { return m_MtrlTextures[iType]; }

	HRESULT Save(const HANDLE& hFile);

	_bool Has_Texture(MATERIAL_TEXTURE eTexture) const;
	HRESULT Bind_BaseColor(CShader* pShader, const char* pConstantName) const;
	HRESULT Bind_EmissionColor(CShader* pShader, const char* pConstantName) const;
	const _float4& Get_BaseColor() const { return m_vBaseColor; }
	void Set_BaseColor(const _float4& vColor) { m_vBaseColor = vColor; }

private:
	ID3D11Device* m_pDevice { nullptr };
	ID3D11DeviceContext* m_pContext { nullptr };

	using vectorSRV = std::vector<ID3D11ShaderResourceView*>;
	std::array<vectorSRV, AI_TEXTURE_TYPE_MAX> m_MtrlTextures;

	using PATH_BY_INDEX = pair<_uint, _tchar[MAX_PATH]>;
	vector<PATH_BY_INDEX> m_TextureFilesByType;

	_float4 m_vBaseColor {1.f, 1.f, 1.f, 1.f};
	_float4 m_vEmissionColor {0.f, 0.f, 0.f, 1.f};

	_uint m_iShaderPass { 0 };

private:
	void Ready_DefaultTexture();
	ID3D11ShaderResourceView* Generate_DefaultTexture_With_Color(const _vec4& vColor);

public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMaterial* pAIMaterial, const _string& strModelFilePath);
	static CMaterial* Create_From_File(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const HANDLE& hFile, const _wstring& strModelFilePath);
	virtual void Free() override;
};

END
