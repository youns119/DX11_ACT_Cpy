#pragma once

#include "Bounding.h"

template class CBounding<BoundingOrientedBox>;

class CBounding_OBB final : public CBounding<BoundingOrientedBox>
{
	using super = CBounding<BoundingOrientedBox>;

private:
	CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_OBB() = default;

public:
	virtual HRESULT Initialize(const CBounding<BoundingOrientedBox>::DESC& pArg);
	virtual void Update(_fmatrix WorldMatirx);
	virtual HRESULT Render(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* pBatch);

	_bool Intersect(CBounding<BoundingOrientedBox>* pTargetBounding);

private:
	DirectX::BoundingOrientedBox m_OriginalOBB {};
	DirectX::BoundingOrientedBox m_OBB {};

public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CBounding<BoundingOrientedBox>::DESC& Desc);
	virtual void Free() override;
};

