#pragma once

#include "Base.h"

#ifdef _DEBUG
//#define SHADOW_DEBUG
#endif // _DEBUG


BEGIN(Engine)

class CGameInstance;
class CShadow final : public CBase
{
	using super = CBase;
private:
	CShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CShadow() = default;

public:
	HRESULT Initialize();
	void Update();
	HRESULT SetUp_ShadowLight(_fvector vEye, _fvector vAt, _float fLightAngle, _float fAspect, _float fNear, _float fFar);
	HRESULT SetUp_ShadowLight(_vec3 vLightDir, _float fNear = 0.1f, _float fFar = 1000.f);
	HRESULT Bind_Matrices(class CShader* pShader, const _char* pViewConstantName, const _char* pProjConstantName) const;
	HRESULT Bind_CascadeCircumSphere_To_Shader(class CShader* pShader, const _char* pCircueSphereConstantName) const;


private:
	ID3D11Device*			m_pDevice { nullptr };
	ID3D11DeviceContext*	m_pContext { nullptr };

	CGameInstance* m_pGameInstance { nullptr };

	_float4x4		m_LightViewMatrices[3] {};
	_float4x4		m_LightProjMatrices[3] {};

	_vec3			m_vLightDir {};
	_float			m_fNear { 0.1f };
	_float			m_fFar { 1000.f };

	static const _uint	s_iNumCascade { 3 };

	array<_vec4, s_iNumCascade> m_CircumscribeSpheres {};

#ifdef _DEBUG
	array<_vec3, 8> m_ShadowDebugFrustum {};
#endif // _DEBUG


private:
	//void Cal_Cuboid(const array<_vec4, 8>& FrustumPoints);
	void Compute_CircumscribedSphere(_Out_ _vec3& fCenter, _Out_ _float& fRadius, const array<_vec3, 8>& FrustumPoints, _vec4 vColor);

#ifdef _DEBUG
	array<_vec3, 8> Create_TestFrustum() const;
	void Save_ShadowDebugFrustum();
#endif // _DEBUG

public:
	static CShadow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END
