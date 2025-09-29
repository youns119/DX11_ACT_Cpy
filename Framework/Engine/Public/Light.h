#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLight final : public CBase
{
	using super = CBase;

	struct LIGHT_PARAMS
	{
		_vec3 vDir {};
		_vec3 vPos {};
		_vec3 vColor {};
		_vec3 vAttenuation {};
		_vec2 vRange {};
		_float fIntensity { 1.f };
	};

private:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight() = default;

public:
	//const LIGHT_DESC* Get_LightDesc() const { return &m_LightDesc; }
	const LIGHT_DESC& Get_LightDesc() const { return m_LightDesc; }
	void Set_LightDesc(const LIGHT_DESC& LightDesc) { m_LightDesc = LightDesc; }// 달리 방법이 없을것같다. 가져오고 다시 세팅하고..

	HRESULT Initialize(const LIGHT_DESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer) const;

	void Toggle();
	void Turn_On();
	void Turn_Off();

private:
	ID3D11Device* m_pDevice { nullptr };
	ID3D11DeviceContext* m_pContext { nullptr };
	LIGHT_DESC m_LightDesc {};

	_bool m_isActive { true };

public:
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc);
	virtual void Free() override;
};

END
