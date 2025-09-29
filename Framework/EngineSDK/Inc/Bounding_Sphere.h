#pragma once

#include "Bounding.h"

template class CBounding<BoundingSphere>;

class CBounding_Sphere final : public CBounding<BoundingSphere>
{
	using super = CBounding<BoundingSphere>;

private:
	CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_Sphere() = default;

public:
	virtual HRESULT Initialize(const CBounding<BoundingSphere>::DESC& pArg);
	virtual void Update(_fmatrix WorldMatrix);
	virtual HRESULT Render(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* pBatch);

	_bool Intersect(CBounding<BoundingSphere>* pTargetBounding);

private:
	// 굳이 포인터로 만들어서 동적할당 할 필요가 있나? 그냥 만들어도 될거같은데 
	DirectX::BoundingSphere m_OriginalSphere {};
	DirectX::BoundingSphere m_Sphere {};

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CBounding<BoundingSphere>::DESC& Desc);
	virtual void Free() override;
};

