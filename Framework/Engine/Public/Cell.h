#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
	using super = CBase;

public:
	enum class POINT { A, B, C, MAX };
	enum class LINE { AB, BC, CA, MAX };

	enum
	{
		POINT_END = POINT::MAX,
		LINE_END = LINE::MAX
	};

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	HRESULT Initialize(const array<_uint, POINT_END>& Indices, _int iIndex, const vector<_float3>& Points, _uint iOption);

	_vector Get_Point(POINT ePoint)
	{
		return XMLoadFloat3(&m_Points[(_uint)ePoint]);
	}

	void Set_Neighbor(LINE eLine, CCell* pNeighbor)
	{
		m_NeighborIndices[(_uint)eLine] = pNeighbor->m_iIndex;
	}

	_bool Is_In(_fvector vPosition, _int& iNeiborIndex);
	_bool Compare_Points(_fvector vSourPoint, _fvector vDestPoint);
	_vector Compute_Position_On_Plane(_fvector vPosition);

	_int Get_Index()const { return m_iIndex; };
	void Set_Index(_uint iIndex) { m_iIndex = iIndex; }

	_uint Get_Option() const { return m_iOption; }
	void Set_Option(_uint iOption) { m_iOption = iOption; }

	vector<_float3> Get_Points() const;
	vector<_uint> Get_PointIndices() const;
	vector<_int> Get_NeighborIndices() const;
	const array<_int, 3>& Get_NeightborIndices_Ref() const;

	void Update_CacheData_From_PointIndices(const vector<_float3>& Points);

	HRESULT Save_Cell(HANDLE hFile);

	_bool Check_Containing(_uint iIndex);
	void Change_PointIndex(_uint iSrc, _uint iDst);

	_bool Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist);

	pair<_uint, _uint> Find_CommonEdge(CCell* pOther);

	void Set_Parent(_uint iParentIndex) { m_iParentIndex = iParentIndex; }
	_uint Get_ParentIndex() const { return m_iParentIndex; }
	_float Compute_Distance(CCell* pOther) const;
	_vector Get_CenterPosition() const { return XMLoadFloat3(&m_vCenter); }

	_vector Compute_SlidingVector(_fvector vPosition, _fvector vDeltaMove);

#ifdef _DEBUG
public:
	HRESULT Render();
	void Update_Buffer();
#endif

private:
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pContext{ nullptr };

	//std::array<_float3, POINT_END>	m_Points {};
	std::array<_uint, POINT_END>	m_PointIndices{};
	std::array<_int, LINE_END>		m_NeighborIndices{ -1, -1, -1 };
	std::array<_float3, POINT_END>	m_Points{};

	_int				m_iIndex{ 0 };

	_float4		m_vPlane{};

	_uint m_iOption{ 0 }; // bit masking
	_float3 m_vCenter{};

	_uint m_iParentIndex{ 0 };

#ifdef _DEBUG
private:
	class CVIBuffer_Cell* m_pVIBufferCom{ nullptr };
#endif


public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		const array<_uint, POINT_END>& Indices, _int iIndex, const vector<_float3>& Points, _uint iOption = 0);
	virtual void Free() override;
};

END
