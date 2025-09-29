#pragma once

#include "Bounding.h"

template class CBounding<BoundingBox>;

class CBounding_AABB final : public CBounding<BoundingBox>
{
	using super = CBounding<BoundingBox>;

private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_AABB() = default;

public:
	virtual HRESULT Initialize(const CBounding<BoundingBox>::DESC& pArg);
	virtual void Update(_fmatrix WorldMatirx);
	virtual HRESULT Render(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* pBatch);

	_bool Intersect(CBounding<BoundingBox>* pTargetBounding);

private:
	// 굳이 포인터로 만들어서 동적할당 할 필요가 있나? 그냥 만들어도 될거같은데 
	DirectX::BoundingBox m_OriginalAABB {};
	DirectX::BoundingBox m_AABB {};

	
public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CBounding<BoundingBox>::DESC& Desc);
	virtual void Free() override;
};

