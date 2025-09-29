#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	using super = CBase;

private:
	CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight_Manager() = default;

public:
	//const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	const LIGHT_DESC& Get_LightDesc(const _wstring& strLightTag) const;
	void Set_LightDesc(const _wstring& strLightTag, const LIGHT_DESC& LightDesc);
	_float4 Set_LightRotateY(_float4 vDirection, _float fAngle);

	HRESULT Initialize();
	HRESULT Add_Light(const _wstring& LightTag, const LIGHT_DESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	void Remove_Light(const _wstring& strLightTag);

	void Toggle_Light(const _wstring& strLightTag);
	void Turn_On(const _wstring& strLightTag);
	void Turn_Off(const _wstring& strLightTag);

	void Clear_Lights();

	void Save_Light(HANDLE hFile, LIGHT_TYPE eType);

private:
	ID3D11Device* m_pDevice { nullptr };
	ID3D11DeviceContext* m_pContext { nullptr };

	//std::list<class CLight*> m_Lights;
	map<const _wstring, class CLight*>  m_Lights;

public:
	static CLight_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
