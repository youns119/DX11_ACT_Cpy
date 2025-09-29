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
	// ���� �����ͷ� ���� �����Ҵ� �� �ʿ䰡 �ֳ�? �׳� ���� �ɰŰ����� 
	DirectX::BoundingSphere m_OriginalSphere {};
	DirectX::BoundingSphere m_Sphere {};

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CBounding<BoundingSphere>::DESC& Desc);
	virtual void Free() override;
};

