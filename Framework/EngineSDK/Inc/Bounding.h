#include "Base.h"
#include "DebugDraw.h"

BEGIN(Engine)

template<typename T>
class CBounding final : public CBase
{
	using super = CBase;

public:
	struct DESC
	{
		_float3 vCenter {};
		_float3 vExtents {};
		_float3 vRotation {};
		_float fRadius { 0.f };
	};

public:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
		: m_pDevice {pDevice}
		, m_pContext {pContext}
	{
		Safe_AddRef(m_pDevice);
		Safe_AddRef(m_pContext);
	}
	CBounding(const CBounding& Prototype)
		: m_pContext { Prototype.m_pContext }
		, m_pDevice { Prototype.m_pDevice }
	{
		Safe_AddRef(m_pDevice);
		Safe_AddRef(m_pContext);
	}
	virtual ~CBounding() = default;

public:
	HRESULT Initialize(const T& Prototype)
	{
		m_OriginBounding = Prototype;
		m_Bounding = m_OriginBounding;

		return S_OK;
	}

	void Update(_fmatrix WorldMatrix)
	{
		m_OriginBounding.Transform(m_Bounding, WorldMatrix);
	}
	void Update_IgnoreScale(_fmatrix WorldMatrix)
	{
		_matrix TransformMatrix = WorldMatrix;

		TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[0]);
		TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[1]);
		TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(WorldMatrix.r[2]);

		m_OriginBounding.Transform(m_Bounding, TransformMatrix);
	}

	HRESULT Render(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* pBatch, _fvector vColor)
	{
		pBatch->Begin();

		DX::Draw(pBatch, m_Bounding, vColor);

		pBatch->End();

		return S_OK;
	}

	const T& Get_Bounding() const { return m_Bounding; }

private:
	ID3D11Device* m_pDevice { nullptr };
	ID3D11DeviceContext* m_pContext { nullptr };

	T m_OriginBounding;
	T m_Bounding;

public:
	virtual void Free() override
	{
		super::Free();

		Safe_Release(m_pDevice);
		Safe_Release(m_pContext);
	}
};

END
