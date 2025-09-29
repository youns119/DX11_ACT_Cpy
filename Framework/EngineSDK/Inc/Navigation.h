#pragma once

#include "Component.h"
#include "Shared.h"

BEGIN(Engine)

class CCell;

// 점프를 할떄든 뭐든 m_iCurrentCellIndex 를 옮길떄 -1 이되는 경우가 생길수도 있음 사실 뭐 중요한가? 싶긴하지만 그래도 혹시 오류가 나면 이것도 의심해보기
class ENGINE_DLL CNavigation final : public CComponent
{
	using super = CComponent;

public:
	struct DESC
	{
		_int iCurrentCellIndex{ -1 };
		_float3 vStartPosition{}; // 이거 왜 주석이였ㄷ지?
	};

	enum class RENDER_PASS
	{
		DEFAULT,
		WIREFRAME,

		MAX
	};

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Prototype(const _wstring& strNavigationDataFile);
	virtual HRESULT Initialize_Prototype(const vector<vector<_uint>>& Indices, const vector<_float3>& Points);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Save_Navigation(const _wstring& strFilePath);
	HRESULT Load_Navigation(const _wstring& strFilePath);

	_int Add_Point(_float3& vPoint);
	_bool Find_Point(_float3& vPoint, _int& iChangeIndex);

	_uint Get_CellSize() { return (_uint)m_Cells.size(); }
	void Add_Cell(const vector<_int>& PointIndices);
	void Add_Cells(const vector<vector<_int>>& PointsIndices);
	void Erase_Point(_uint iPointIndex);
	void Erase_Cell(_uint iCellIndex);

	void Set_WorldMatrix(_fmatrix WorldMatrix)
	{
		XMStoreFloat4x4(m_pWorldMatrix->Ptr(), WorldMatrix);
		XMStoreFloat4x4(m_pWorldMatrixInverse->Ptr(), XMMatrixInverse(nullptr, WorldMatrix));
	}

	_vector Compute_Position_On_Plane(_fvector vWorldPos);
	_bool Is_Move(_fvector vWorldPos);

	//void Check_Cell(_uint iCellIndex, _bool isIn) { m_pSharedData->m_bSomethingIn[iCellIndex] = isIn; }

	vector<vector<_float3>> Get_Cells() const;
	vector<vector<_uint>> Get_CellsPointIndices() const;
	const vector<_float3>& Get_Points() const { return m_Points->Value(); }

	_uint Get_CellOption(_uint iCellIndex) const;
	void Set_CellOption(_uint iCellIndex, _uint iOption);
	_uint Get_CurrentCellOption() const;

	vector<_int> Get_NeighborCellIndices() const;
	vector<_int> Get_NeighborCellIndices(_uint iCellIndex) const;

	void Set_Point(_uint iPointIndex, const _float3& Point);

	_vector Convert_WorldPosition(_fvector vWorldPosition) { return XMVector3TransformCoord(vWorldPosition, XMLoadFloat4x4(m_pWorldMatrix->Ptr())); }
	_vector Convert_LocalPosition(_fvector vWorldPosition) { return XMVector3TransformCoord(vWorldPosition, XMLoadFloat4x4(m_pWorldMatrixInverse->Ptr())); }
	_vector Convert_LocalVector(_fvector vWorldVector) { return XMVector3TransformNormal(vWorldVector, XMLoadFloat4x4(m_pWorldMatrixInverse->Ptr())); }
	_bool Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist, _uint& iCellIndex, _uint iOptionMask = 0xffffffff);


	void Move_CurrentCellIndex(_int iMovedCellIndex); // 디버깅용. 현재 셀 위에 몇개의 오브젝트가 있는지 
	void Move_CurrentCellIndex(_fvector vWorldPosition);//현재 포지션에서 레이캐스팅 + 발 밑 셀없으면 이동못하는거? 잇으면 해
	void UnRegister_ObjectIndex();

	//Astar
	vector<pair<_float3, _float3>> Make_Path(_fvector vGoalPosition, _uint iOptionMask = 0xffffffff);
	_vector Get_CenterPosition(_uint iCellIndex); // 예시로 카메라 가 셀의 무게중심 위에 위치하고 촬영 (플레이어 죽었을때)

	//슬라이딩 벡터
	_vector Compute_SlidingVector(_fvector vPosition, _fvector vDeltaMove);

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
	void Set_RenderPass(RENDER_PASS ePass) { m_ePass = ePass; }
#endif // _DEBUG


private:
	/* 이 네비를 이용하는 객체가 현재 어떤 셀 안에 있는지?*/
	_int						m_iCurrentCellIndex{ -1 };
	vector<CCell*>				m_Cells;
	CShared<vector<_float3>>* m_Points{ nullptr };

	// static 으로 한다면 네비게이션 메쉬.. 모든 네비게이션은 똑같아짐..
	//static _float4x4		s_WorldMatrix;
	//static _float4x4		s_WorldMatrixInverse;

#ifdef _DEBUG
	class CShader* m_pShaderCom{ nullptr };
	RENDER_PASS m_ePass{ RENDER_PASS::DEFAULT };
	static const array<_float4, 4> vOptionColors;
#endif // _DEBUG

	CShared<_float4x4>* m_pWorldMatrix{ nullptr };
	CShared<_float4x4>* m_pWorldMatrixInverse{ nullptr };

	CShared<vector<_uint>>* m_NumInObjects{ nullptr }; // 디버깅용. 셀위에 오브젝트 몇개

	// AStar
	list<_uint> m_OpenList;
	list<_uint> m_CloseList;
	vector<_bool> m_isOpenList;
	vector<_bool> m_isCloseList;

private:
	// AStar
	_bool Is_Accessible(_uint iStartIndex, _uint iGoalIndex, _uint iOptionMask);
	vector<pair<_float3, _float3>> Make_BestRoute(_uint iStartIndex, _uint iGoalIndex);
	void Clear_AStar();

	HRESULT SetUp_Neighbors();
	void Sort_Points_ClockWise(_uint* TriangleIndices);
	vector<_uint> Get_CellIndices_By_Option(_uint iOptionMask); // 이건멍미


public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strNavigationDataFile);
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		const vector<vector<_uint>> Indices, const vector<_float3>& Points);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
