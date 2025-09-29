#pragma once

#include "Base.h"

BEGIN(Engine)

class CDebugRender final : public CBase
{
	using super = CBase;

	struct GRID_DESC
	{
		_vec3 vOrgin {};
		_vec3 vAxisX { 1.f, 0.f, 0.f };
		_vec3 vAxisY { 0.f, 0.f, 1.f };
		_float fUnitX { 1.f };
		_float fUnitY { 1.f };
	};

	struct CAPSULE_DESC
	{
		_vec3 vOrigin {};
		_vec3 vDirection {0.f, 1.f, 0.f};
		_float fRadius { 0.f };
		_float fHeight { 0.f };
	};

private:
	CDebugRender(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDebugRender() = default;

public:
	HRESULT Initialize();
	void Render();
	void Render_NonBlend();
	void Render_Blend();

	void Add_DebugRender_Sphere(DirectX::BoundingSphere Sphere, _float4 vColor);
	void Add_DebugRender_Triangle(_float3 Points[3], _float4 vColor);
	void Add_DebugRender_Quad(_vec3 Points[4], _vec4 vColor);
	void Add_DebugRender_Box(_mat WorldMatrix, _vec4 vColor);
	void Add_DebugRender_Ray(_vec3 vOrigin, _vec3 vDir, _bool isNormalize, _vec4 vColor);
	void Add_DebugRender_Grid(_vec3 vOrigin, _vec3 vAxisX, _vec3 vAxisY, _float fUnitX, _float fUnitY, _vec4 vColor);
	void Add_DebugRender_Capsule(_vec3 vPosition, _vec3 vDirection, _float fRadius, _float fHeight, _float4 vColor);
	void Add_DebugRender_Function(function<HRESULT()> RenderFunction) { m_RenderFunctionQueue.push_back(RenderFunction); }

	void Clear();

private:
	ID3D11Device*			m_pDevice { nullptr };
	ID3D11DeviceContext*	m_pContext { nullptr };

	ID3D11DepthStencilState* m_pDSS { nullptr };

	DirectX::PrimitiveBatch<VertexPositionColor> m_Batch;
	BasicEffect m_Effect;
	ID3D11InputLayout* m_pInputLayout { nullptr };

	vector< pair<DirectX::BoundingSphere, _float4> >		m_SphereRenderQueue;
	vector< pair<array<_float3, 3>, _float4> >				m_TriangleRenderQueue;
	vector< pair<array<_vec3, 4>, _vec4> >					m_QuadRenderQueue;
	vector< tuple<pair<_vec3, _vec3>, _bool, _vec4> >		m_RayQueue;
	vector< pair<GRID_DESC, _vec4> >						m_GridQueue;
	vector< pair<CAPSULE_DESC, _vec4> >						m_CapsuleQueue;
	vector< function<HRESULT()> >							m_RenderFunctionQueue;

private:
	HRESULT Ready_DepthStencilState();

	void Render_Sphere();
	void Render_Triangle();
	void Render_Qaud();
	void Render_Ray();
	void Render_Grid();
	void Render_Capsule();


public:
	static CDebugRender* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
