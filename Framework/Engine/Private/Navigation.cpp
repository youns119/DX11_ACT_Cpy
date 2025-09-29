#include "Navigation.h"

#include "Cell.h"

#ifdef _DEBUG
#include "Shader.h"
#include "GameInstance.h"
#endif // _DEBUG
//
//_float4x4		CNavigation::s_WorldMatrix {};
//_float4x4		CNavigation::s_WorldMatrixInverse {};
#ifdef _DEBUG
const array<_float4, 4> CNavigation::vOptionColors =
{
	_float4{0.f, 1.f, 0.f, 0.4f},
	_float4{1.f, 1.f, 0.f, 0.5f},
	_float4{0.f, 0.f, 1.f, 0.5f},
	_float4{0.f, 1.f, 1.f, 0.5f},
};
#endif // _DEBUG


CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent(Prototype)
	, m_Cells{ Prototype.m_Cells }
#ifdef _DEBUG
	, m_pShaderCom{ Prototype.m_pShaderCom }
#endif
	, m_pWorldMatrix{ Prototype.m_pWorldMatrix }
	, m_pWorldMatrixInverse{ Prototype.m_pWorldMatrixInverse }
	, m_NumInObjects{ Prototype.m_NumInObjects }
	, m_Points{ Prototype.m_Points }
{
#ifdef _DEBUG
	Safe_AddRef(m_pShaderCom);
#endif

	for (auto& pCells : m_Cells)
		Safe_AddRef(pCells);


	Safe_AddRef(m_pWorldMatrix);
	Safe_AddRef(m_pWorldMatrixInverse);
	Safe_AddRef(m_NumInObjects);
	Safe_AddRef(m_Points);
}

HRESULT CNavigation::Initialize_Prototype()
{
	_uint iNumPoints{ 0 };
	m_Points = CShared<vector<_float3>>::Create();

	m_pWorldMatrix = CShared<_float4x4>::Create();
	m_pWorldMatrixInverse = CShared<_float4x4>::Create();

	XMStoreFloat4x4(m_pWorldMatrix->Ptr(), XMMatrixIdentity());
	XMStoreFloat4x4(m_pWorldMatrixInverse->Ptr(), XMMatrixIdentity());

	m_NumInObjects = CShared<vector<_uint>>::Create();
	m_NumInObjects->Value().resize(m_Cells.size(), 0);

#ifdef _DEBUG
	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/Hlsl/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShaderCom)
		return E_FAIL;
#endif

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation::Initialize_Prototype(const _wstring& strNavigationDataFile)
{
	DWORD dwByte = 0;
	_bool bResult = false;
	HANDLE hFile = CreateFile(strNavigationDataFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Can Not Open File");
		return E_FAIL;
	}

	_uint iNumPoints{ 0 };
	m_Points = CShared<vector<_float3>>::Create();

	bResult = ReadFile(hFile, &iNumPoints, sizeof(_uint), &dwByte, nullptr);
	m_Points->Value().resize(iNumPoints);
	bResult = ReadFile(hFile, m_Points->Value().data(), sizeof(_float3) * iNumPoints, &dwByte, nullptr);

	while (true)
	{
		//_float3 Points[CCell::POINT_END] {};

		//_bool bResult = ReadFile(hFile, Points, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);

		//if (0 == dwByte)
		//	break;

		//CCell* pCell = CCell::Create(m_pDevice, m_pContext, Points, (_int)m_Cells.size());
		//if (nullptr == pCell)
		//	return E_FAIL;

		//m_Cells.push_back(pCell);


		_bool bResult = false;

		_uint iOption = 0;
		std::array<_uint, CCell::POINT_END> Indices{};

		bResult = ReadFile(hFile, &iOption, sizeof(_uint), &dwByte, nullptr);
		bResult = ReadFile(hFile, Indices.data(), sizeof(_uint) * CCell::POINT_END, &dwByte, nullptr);

		if (0 == dwByte)
			break;
		//Sort_Points_ClockWise(Indices); // 아직 검증 안됨

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, Indices, (_int)m_Cells.size(), m_Points->Value(), iOption);
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	//XMStoreFloat4x4(&s_WorldMatrix, XMMatrixIdentity());
	//XMStoreFloat4x4(&s_WorldMatrixInverse, XMMatrixIdentity());

	//m_isPrototype = TRUE;

	//m_pWorldMatrix = new _float4x4();
	//m_pWorldMatrixInverse = new _float4x4();

	//XMStoreFloat4x4(m_pWorldMatrix, XMMatrixIdentity());
	//XMStoreFloat4x4(m_pWorldMatrixInverse, XMMatrixIdentity());

	//m_pSharedData = CSharedData::Create();
	//{
	//	XMStoreFloat4x4(&m_pSharedData->m_WorldMatrix, XMMatrixIdentity());
	//	XMStoreFloat4x4(&m_pSharedData->m_WorldMatrixInverse, XMMatrixIdentity());
	//	m_pSharedData->m_NumInObjects.resize(m_Cells.size(), 0);
	//}

	m_pWorldMatrix = CShared<_float4x4>::Create();
	m_pWorldMatrixInverse = CShared<_float4x4>::Create();

	XMStoreFloat4x4(m_pWorldMatrix->Ptr(), XMMatrixIdentity());
	XMStoreFloat4x4(m_pWorldMatrixInverse->Ptr(), XMMatrixIdentity());

	m_NumInObjects = CShared<vector<_uint>>::Create();
	m_NumInObjects->Value().resize(m_Cells.size(), 0);

#ifdef _DEBUG
	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/Hlsl/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShaderCom)
		return E_FAIL;
#endif

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation::Initialize_Prototype(const vector<vector<_uint>>& Indices, const vector<_float3>& Points)
{
	m_Points = CShared<vector<_float3>>::Create();
	m_Points->Value() = Points;

	for (auto& PolygonIndices : Indices)
	{
		std::array<_uint, CCell::POINT_END> TriangleIndices =
		{
			PolygonIndices[(_uint)CCell::POINT::A],
			PolygonIndices[(_uint)CCell::POINT::B],
			PolygonIndices[(_uint)CCell::POINT::C],
		}; // memcpy
		Sort_Points_ClockWise(TriangleIndices.data());

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, TriangleIndices, (_int)m_Cells.size(), m_Points->Value());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	m_pWorldMatrix = CShared<_float4x4>::Create();
	m_pWorldMatrixInverse = CShared<_float4x4>::Create();

	XMStoreFloat4x4(m_pWorldMatrix->Ptr(), XMMatrixIdentity());
	XMStoreFloat4x4(m_pWorldMatrixInverse->Ptr(), XMMatrixIdentity());

	m_NumInObjects = CShared<vector<_uint>>::Create();
	m_NumInObjects->Value().resize(m_Cells.size(), 0);

#ifdef _DEBUG
	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/Hlsl/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShaderCom)
		return E_FAIL;
#endif

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;


	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr != pArg) // 이 네비게이션을 타고 있는 놈이 아니라면 pArg 는 nullptr 이어야 할듯
	{
		DESC* pDesc = static_cast<DESC*>(pArg);

		m_iCurrentCellIndex = pDesc->iCurrentCellIndex;
		++m_NumInObjects->Value()[m_iCurrentCellIndex];

		Move_CurrentCellIndex(XMLoadFloat3(&pDesc->vStartPosition));
	}
	else
	{
		for (size_t i = 0; i < m_Cells.size(); ++i) // 이렇게 하면 네비를 타는 오브젝트들보다 무조건 맵이 먼저 만들어져야 함
			m_NumInObjects->Value()[i] = 0;
	}

	return S_OK;
}

HRESULT CNavigation::Save_Navigation(const _wstring& strFilePath)
{
	HRESULT hr{};
	DWORD dwByte{};
	HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Can Not Open File");
		return E_FAIL;
	}

	_uint iNumPoints = (_uint)m_Points->Value().size();
	WriteFile(hFile, &iNumPoints, sizeof(_uint), &dwByte, nullptr);

	vector<_float3> PointsLocalPos(iNumPoints);
	//for (_uint i = 0; i < iNumPoints; ++i)
	//{
	//	XMStoreFloat3(&PointsLocalPos[i], XMVector3TransformCoord(XMLoadFloat3(&m_Points->Value()[i]), XMLoadFloat4x4(m_pWorldMatrixInverse->Ptr()))); // 이제 이거 하면 안되는거 아니야?
	//}
	WriteFile(hFile, m_Points->Value().data(), sizeof(_float3) * iNumPoints, &dwByte, nullptr);
	//WriteFile(hFile, PointsLocalPos.data(), sizeof(_float3) * iNumPoints, &dwByte, nullptr);

	for (auto& pCell : m_Cells)
	{
		hr = pCell->Save_Cell(hFile);
	}

	CloseHandle(hFile);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation::Load_Navigation(const _wstring& strFilePath)
{
	if (!m_Cells.empty())
	{
		for (auto& pCells : m_Cells)
			Safe_Release(pCells);
		m_Cells.clear();
	}

	if (!m_Points->Value().empty())
		Safe_Release(m_Points);

	HRESULT hr{};
	DWORD dwByte{};
	HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Can Not Open File");
		return E_FAIL;
	}

	_uint iNumPoints{ 0 };
	m_Points = CShared<vector<_float3>>::Create();

	_bool bResult = ReadFile(hFile, &iNumPoints, sizeof(_uint), &dwByte, nullptr);
	m_Points->Value().resize(iNumPoints);
	bResult = ReadFile(hFile, m_Points->Value().data(), sizeof(_float3) * iNumPoints, &dwByte, nullptr);

	while (true)
	{
		bResult = false;

		_uint iOption = 0;
		std::array<_uint, CCell::POINT_END> Indices{};

		bResult = ReadFile(hFile, &iOption, sizeof(_uint), &dwByte, nullptr);
		bResult = ReadFile(hFile, Indices.data(), sizeof(_uint) * CCell::POINT_END, &dwByte, nullptr);

		if (0 == dwByte)
			break;
		//Sort_Points_ClockWise(Indices); // 아직 검증 안됨

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, Indices, (_int)m_Cells.size(), m_Points->Value(), iOption);
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	return S_OK;
}

_int CNavigation::Add_Point(_float3& vPoint)
{
	_int iIndex = { -1 };
	_bool bChange = Find_Point(vPoint, iIndex);

	if (bChange)
		return iIndex;
	else
		m_Points->Value().push_back(vPoint);


	return (_int)m_Points->Value().size() - 1;
}

_bool CNavigation::Find_Point(_float3& vPoint, _int& iChangeIndex)
{
	// 현재 피킹 위치랑 가까운 Point를 찾아서 그 포인트 값으로 반환 해주기
	if (!m_Points || m_Points->Value().empty())
		return false;

	_float2 ptScreenPos = m_pGameInstance->Calulate_ScreenMousePos();

	_float3 vRayDir = {};
	XMStoreFloat3(&vRayDir, m_pGameInstance->Calculate_ScreenToWorldRay(ptScreenPos));
	_vector vDir = XMLoadFloat3(&vRayDir);
	vDir = XMVector3Normalize(vDir);
	XMStoreFloat3(&vRayDir, vDir);

	_float4 vCamPos = m_pGameInstance->Get_CamPosition();
	_float3 vWorldPos = {};
	XMStoreFloat3(&vWorldPos, m_pGameInstance->Compute_WorldPos(ptScreenPos));


	_float fDist = {}; // 받아오는 인자
	_float3 vIntersectionPoint = {}; // 결과값


	//auto& Points = m_Points->Value();  // m_Value에 접근

	//Points[m_Cells[]]
	for (size_t iIndex = 0; iIndex < m_Cells.size(); iIndex++)
	{
		_bool isIntersect = TriangleTests::Intersects(
			XMLoadFloat4(&vCamPos),
			XMLoadFloat3(&vRayDir),
			m_Cells[iIndex]->Get_Point(CCell::POINT::A),
			m_Cells[iIndex]->Get_Point(CCell::POINT::B),
			m_Cells[iIndex]->Get_Point(CCell::POINT::C), // Tlqkf. 
			fDist);

		if (isIntersect)
		{
			_float3 vDistance[3] = {};
			XMStoreFloat3(&vIntersectionPoint, XMLoadFloat4(&vCamPos) + XMLoadFloat3(&vRayDir) * fDist);
			//결과

			// 내가 찍은 좌표 값과 삼각형 버텍스의 좌표를 절대값으로 만들어가지고 거리를 계산해가주고 가장 가까운 점을 반환할거샘
			XMStoreFloat3(&vDistance[0], XMVectorAbs(XMLoadFloat3(&vIntersectionPoint) - m_Cells[iIndex]->Get_Point(CCell::POINT::A)));
			XMStoreFloat3(&vDistance[1], XMVectorAbs(XMLoadFloat3(&vIntersectionPoint) - m_Cells[iIndex]->Get_Point(CCell::POINT::B)));
			XMStoreFloat3(&vDistance[2], XMVectorAbs(XMLoadFloat3(&vIntersectionPoint) - m_Cells[iIndex]->Get_Point(CCell::POINT::C)));

			// 각 벡터의 길이를 계산!!! X만 꺼내는 이유? XMVector3Length 함수는 벡터의 길이를 계산한 결과를 X에 반환. y z w는 0으로 초기화.
			_float fDistances[3] =
			{
				XMVectorGetX(XMVector3Length(XMLoadFloat3(&vDistance[0]))),
				XMVectorGetX(XMVector3Length(XMLoadFloat3(&vDistance[1]))),
				XMVectorGetX(XMVector3Length(XMLoadFloat3(&vDistance[2])))
			};

			_float fMinDist = fDistances[0]; // 0번째를 일단 넣어노코
			_int iMinIdx = 0; // [iIndex + 2] 뒤에 상수랑 인덱스가 동일하니끼 요거로 좀따 찾아서 포지션 반환해주꺼임

			for (_int i = 0; i < 3; i++) // 어차피 삼각형이니까 상수로 넣었어여.
			{
				if (fDistances[i] < fMinDist)
				{
					fMinDist = fDistances[i];
					iMinIdx = i; // 가장 가까운.
				}
			}
			XMStoreFloat3(&vIntersectionPoint, m_Cells[iIndex]->Get_Point(static_cast<CCell::POINT>(iMinIdx)));
			// vIntersectionPoint 이 이름말고 MinPoint할라고 햇는데 그냥 햇음 ㅋ헤헤 가장 가까운 거리의 포인터임! 켜켜!
			vPoint = vIntersectionPoint; // 가까운 점을 Out;
			iChangeIndex = m_Cells[iIndex]->Get_PointIndices()[iMinIdx]; // 코드가 개 미친 복잡해진다 진짜 ㅜ
			// 셀이 포인트들의 인덱스를 연속적으로 가지고 있으니까 
			// 셀의 인덱스와 해당 셀이 가지고 있는 iMinIdx(POINT::A,B,C)로 해당 POINT의인덱스를 알아내는거임 ㅜ 

			return true; // 가까운 점 있음
		}
	}
	return false;  // 가까운 점이 없음
}

void CNavigation::Add_Cell(const vector<_int>& PointIndices)
{
	array<_uint, CCell::POINT_END> PointIndicesArray{};

	for (size_t i = 0; i < CCell::POINT_END; ++i)
	{
		PointIndicesArray[i] = PointIndices[i];
	}

	m_Cells.push_back(CCell::Create(m_pDevice, m_pContext, PointIndicesArray, (_uint)m_Cells.size(), m_Points->Value()));
	m_NumInObjects->Value().push_back(0);
	Sort_Points_ClockWise(PointIndicesArray.data());
	SetUp_Neighbors();
}

void CNavigation::Add_Cells(const vector<vector<_int>>& PointsIndices)
{
	for (auto& PointIndices : PointsIndices)
	{
		Add_Cell(PointIndices);
	}
}

void CNavigation::Erase_Point(_uint iPointIndex)
{
	vector<_float3>& Points = m_Points->Value();
	_bool isRemovable = false;

	// 이 포인트를 들고 있는 셀들은 전부 삭제한다.
	vector<_uint> DeleteCellIndices;
	for (size_t i = 0; i < m_Cells.size(); ++i)
	{
		if (m_Cells[i]->Check_Containing(iPointIndex))
		{
			DeleteCellIndices.push_back((_uint)i);
		}
	}
	// 벡터의 요소를 삭제할때 포문을 돌면서 삭제하면 인덱스 문제가 생길 수 있다..
	// 이 방법으로는 절대 안하려고 했는데, 이 방법은 DeleteCellIndices가 정렬되어 있기 때문에 사용 가능했다. 물론 뭐 sort 하면 되긴 하겟지만 
	for (size_t i = 0; i < DeleteCellIndices.size(); ++i)
	{
		_uint iDeleteIndex = DeleteCellIndices[i];
		Erase_Cell(iDeleteIndex - (_uint)i);
	}

	// 삭제될 포인트를 마지막과 바꾸고 각 셀들의 인덱스도 적용
	for (auto& pCell : m_Cells)
	{
		pCell->Change_PointIndex((_uint)Points.size() - 1, iPointIndex);
	}

	iter_swap(Points.begin() + iPointIndex, Points.end() - 1);
	Points.pop_back();
}

void CNavigation::Erase_Cell(_uint iCellIndex)
{
	auto iter = m_Cells.begin() + iCellIndex;
	Safe_Release(*iter);
	m_Cells.erase(iter);

	// sync index
	for (size_t i = 0; i < m_Cells.size(); ++i)
		m_Cells[i]->Set_Index((_uint)i);

	SetUp_Neighbors();
}

_vector CNavigation::Compute_Position_On_Plane(_fvector vWorldPos/* 네비를 타고 있는 녀석의 월드 포지션 */)
{
	if (-1 == m_iCurrentCellIndex)
		return {};

	// Local position
	_vector vPosition = XMVector3TransformCoord(vWorldPos, XMLoadFloat4x4(m_pWorldMatrixInverse->Ptr()));

	// Local position on plane
	vPosition = m_Cells[m_iCurrentCellIndex]->Compute_Position_On_Plane(vPosition);

	//  World position on plane
	return XMVector3TransformCoord(vPosition, XMLoadFloat4x4(m_pWorldMatrix->Ptr()));
}

_bool CNavigation::Is_Move(_fvector vWorldPos)
{
	_vector vPosition = XMVector3TransformCoord(vWorldPos, XMLoadFloat4x4(m_pWorldMatrixInverse->Ptr()));

	_int iNeighborIndex = -1;

	if (m_Cells[m_iCurrentCellIndex]->Is_In(vPosition, iNeighborIndex))
		return true;

	//if (-1 == iNeighborIndex)
	//	return false;


	while (true)
	{
		/* 나간 곳에 이웃이 있는지 검사 */
		if (-1 == iNeighborIndex)
			return false;
		if (m_Cells[iNeighborIndex]->Is_In(vPosition, iNeighborIndex))
			break;
	}

	--m_NumInObjects->Value()[m_iCurrentCellIndex];
	m_iCurrentCellIndex = iNeighborIndex;
	++m_NumInObjects->Value()[m_iCurrentCellIndex];

	// Get_Index ()
	/*
		// _int movedIndex = CURRENTCELL->Get_Index();

		_int preIndex = m_iCurrentIndex;
		_int movedIndex = -1;

		while (true)
		{
			movedIndex = Cells[preIndex]->Get_index();

			if movedIndex == preIndex
				break;

			if -1 == movedIndex;
				return false;

			preIndex = movedIndex;
		}

		m_iCurrentIndex = movedINdex;
	*/
	return true;

	// Is_In 함수 변경하면 여기도 같이 변경해보자
}

vector<vector<_float3>> CNavigation::Get_Cells() const
{
	vector<vector<_float3>> CellData;
	CellData.reserve(m_Cells.size());

	for (auto& pCell : m_Cells)
		CellData.push_back(pCell->Get_Points());

	return CellData;
}

vector<vector<_uint>> CNavigation::Get_CellsPointIndices() const
{
	vector<vector<_uint>> CellPointIndices;
	CellPointIndices.reserve(m_Cells.size());

	for (auto& pCell : m_Cells)
		CellPointIndices.push_back(pCell->Get_PointIndices());

	return CellPointIndices;
}

_uint CNavigation::Get_CellOption(_uint iCellIndex) const
{
	return m_Cells[iCellIndex]->Get_Option();
}

void CNavigation::Set_CellOption(_uint iCellIndex, _uint iOption)
{
	m_Cells[iCellIndex]->Set_Option(iOption);
}

_uint CNavigation::Get_CurrentCellOption() const
{
	return m_Cells[m_iCurrentCellIndex]->Get_Option();
}

vector<_int> CNavigation::Get_NeighborCellIndices() const
{
	return Get_NeighborCellIndices(m_iCurrentCellIndex);
}

vector<_int> CNavigation::Get_NeighborCellIndices(_uint iCellIndex) const
{
	return m_Cells[m_iCurrentCellIndex]->Get_NeighborIndices();
}

void CNavigation::Set_Point(_uint iPointIndex, const _float3& Point)
{
	m_Points->Value()[iPointIndex] = Point;

	for (auto& pCell : m_Cells)
	{
		pCell->Update_CacheData_From_PointIndices(m_Points->Value());
#ifdef _DEBUG
		pCell->Update_Buffer();
#endif
	}
}

_bool CNavigation::Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist, _uint& iCellIndex, _uint iOptionMask)
{
	using CellInfo = pair<_float, _uint>;
	vector<CellInfo> HitCells;

	_vector vLocalRayOrigin = Convert_LocalPosition(vRayOrigin);
	_vector vLocalRayDirection = Convert_LocalVector(vRayDirection);

	vector<_uint> Cells = Get_CellIndices_By_Option(iOptionMask); // 이거보다 조금 빠르게 하는 방법 잇긴할거야

	for (_uint i : Cells)
	{
		if (m_Cells[i]->Ray_Cast(vLocalRayOrigin, vLocalRayDirection, fDist))
		{
			HitCells.push_back({ fDist, (_uint)i });
		}
	}

	if (HitCells.empty())
		return false;

	sort(HitCells.begin(), HitCells.end(), [](const CellInfo& Src, const CellInfo& Dst)->_bool
		{
			return Src.first < Dst.first;
		});

	fDist = HitCells.front().first;
	iCellIndex = HitCells.front().second;

	return true;
}

void CNavigation::Move_CurrentCellIndex(_int iMovedCellIndex)
{
	--m_NumInObjects->Value()[m_iCurrentCellIndex];
	m_iCurrentCellIndex = iMovedCellIndex;
	++m_NumInObjects->Value()[m_iCurrentCellIndex];
}

void CNavigation::Move_CurrentCellIndex(_fvector vWorldPosition)
{
	_vector vRayOrigin = vWorldPosition + XMVectorSet(0.f, 0.5f, 0.f, 0.f);
	_vector vRayDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	_float fDist = 0.f;
	_uint iCellIndex = 0;

	if (false == Ray_Cast(vRayOrigin, vRayDir, fDist, iCellIndex))
	{
		return;
	}

	Move_CurrentCellIndex(iCellIndex);
}

void CNavigation::UnRegister_ObjectIndex()
{
	if (0 > --m_NumInObjects->Value()[m_iCurrentCellIndex])
		MSG_BOX("0 > Num In Object");
}

vector<pair<_float3, _float3>> CNavigation::Make_Path(_fvector vGoalPosition, _uint iOptionMask)
{
	Clear_AStar();

	_vector vRayOrigin = vGoalPosition + XMVectorSet(0.f, 0.1f, 0.f, 0.f);
	_vector vRayDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	_float fDist = 0.f;
	_uint iGoalCellIndex = 0;

	if (false == Ray_Cast(vRayOrigin, vRayDir, fDist, iGoalCellIndex, iOptionMask))
	{
		return {};
	}

	if (m_iCurrentCellIndex == iGoalCellIndex)
		return {};

	if (true == Is_Accessible(m_iCurrentCellIndex, iGoalCellIndex, iOptionMask))
	{
		return Make_BestRoute(m_iCurrentCellIndex, iGoalCellIndex);
	}

	return {};
}

_vector CNavigation::Get_CenterPosition(_uint iCellIndex) // 무게 중심 위치
{
	return XMVector3TransformCoord(m_Cells[iCellIndex]->Get_CenterPosition(), XMLoadFloat4x4(m_pWorldMatrix->Ptr()));
}

_vector CNavigation::Compute_SlidingVector(_fvector vPosition, _fvector vDeltaMove)
{
	return m_Cells[m_iCurrentCellIndex]->Compute_SlidingVector(vPosition, vDeltaMove);
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &s_WorldMatrix)))
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;


	vector<CCell*> DeferredCell;
	DeferredCell.reserve(m_Cells.size());

	_float4 vColor{};
	_float4x4 WorldMatrix;

	WorldMatrix = m_pWorldMatrix->Value();
	WorldMatrix.m[3][1] += 0.01f; //살짝 떨어져잇기

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	// 옵션 별로 Get_Cells_By_Option(Option Mask);

	for (_uint i = 0; i < 4; ++i)
	{
		vector<_uint> OptionCells = Get_CellIndices_By_Option(1 << i); // 
		vColor = vOptionColors[i];
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
			return E_FAIL;

		m_pShaderCom->Begin(static_cast<_uint>(RENDER_PASS::DEFAULT));
		for (_uint ID : OptionCells)
		{
			auto pCell = m_Cells[ID];
			if (FAILED(pCell->Render()))
				return E_FAIL;
		}
	}


	// 단순 외곽선
	//WorldMatrix = m_pWorldMatrix->Value();
	WorldMatrix.m[3][1] += 0.01f;

	vColor = _float4(0.f, 1.f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	m_pShaderCom->Begin(static_cast<_uint>(RENDER_PASS::WIREFRAME));
	for (auto& pCell : m_Cells)
	{
		if (0 < m_NumInObjects->Value()[pCell->Get_Index()])
		{
			DeferredCell.push_back(pCell);
			continue;
		}

		if (FAILED(pCell->Render()))
			return E_FAIL;
	}



	// 해당 셀에 객체가 있을 경우
	vColor = _float4(1.f, 0.f, 1.f, 0.4f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	//WorldMatrix = m_pWorldMatrix->Value();
	WorldMatrix.m[3][1] += 0.01f;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;


	m_pShaderCom->Begin((_uint)m_ePass);
	for (auto& pCell : DeferredCell)
	{
		if (FAILED(pCell->Render()))
			return E_FAIL;
	}

	//DeferredCell.clear();

   // render cell index
	static _uint s_iUIIndex = 0;
	if (m_pGameInstance->Key_Down(KEY::SPACE))
		s_iUIIndex = (s_iUIIndex + 1) % 3;

	_float fCosFOV = cos(XMConvertToRadians(90.f * 0.5f));
	if (1 == s_iUIIndex)
	{
		for (auto& pCell : m_Cells)
		{
			_vector vWorldPosition = XMVector3TransformCoord(pCell->Get_CenterPosition(), XMLoadFloat4x4(m_pWorldMatrix->Ptr()));
			_vector vCamPosition = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());
			_vector vCamLook = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::VIEW).r[2];
			if (fCosFOV > XMVectorGetX(XMVector3Dot(XMVector3Normalize(vWorldPosition - vCamPosition), XMVector3Normalize(vCamLook))))
				continue; // 정확히는 카메라 fov 의 cos 값을 비교해야함

			_vector vViewPos = XMVector3TransformCoord(vWorldPosition, m_pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORM::VIEW));
			_vector vProjPos = XMVector3TransformCoord(vViewPos, m_pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORM::PROJ));
			_vector vNDCPos = vProjPos / XMVectorGetW(vProjPos);

			// -1 ~ 1 -> 0 ~ winSizeX
			_float2 vScreenPos{};
			_uint2 vViewPortSize = m_pGameInstance->Get_ViewportSize();
			vScreenPos.x = (XMVectorGetX(vNDCPos) * 0.5f + 0.5f) * vViewPortSize.x;
			vScreenPos.y = (XMVectorGetY(vNDCPos) * -0.5f + 0.5f) * vViewPortSize.y;


			_wstring strCellIndex = L"C." + to_wstring(pCell->Get_Index());
			m_pGameInstance->Add_DebugRender_Function([vScreenPos, strCellIndex]()->HRESULT
				{
					return CGameInstance::GetInstance()->Render_Font(L"Font_LiberationSans_20", strCellIndex.c_str(), vScreenPos, XMVectorSet(0.2f, 0.2f, 0.2f, 1.f), 0.f, {}, 1.f);
				});
		}
	}
	else if (2 == s_iUIIndex)
	{
		for (size_t i = 0; i < m_Points->Value().size(); ++i)
		{
			_vector vWorldPosition = XMVector3TransformCoord(XMLoadFloat3(&m_Points->Value()[i]), XMLoadFloat4x4(m_pWorldMatrix->Ptr()));

			_vector vCamPosition = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());
			_vector vCamLook = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::VIEW).r[2];
			if (fCosFOV > XMVectorGetX(XMVector3Dot(XMVector3Normalize(vWorldPosition - vCamPosition), XMVector3Normalize(vCamLook))))
				continue; // 정확히는 카메라 fov 의 cos 값을 비교해야함

			_vector vViewPos = XMVector3TransformCoord(vWorldPosition, m_pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORM::VIEW));
			_vector vProjPos = XMVector3TransformCoord(vViewPos, m_pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORM::PROJ));
			_vector vNDCPos = vProjPos / XMVectorGetW(vProjPos);


			// -1 ~ 1 -> 0 ~ winSizeX
			_float2 vScreenPos{};
			_uint2 vViewPortSize = m_pGameInstance->Get_ViewportSize();
			vScreenPos.x = (XMVectorGetX(vNDCPos) * 0.5f + 0.5f) * vViewPortSize.x;
			vScreenPos.y = (XMVectorGetY(vNDCPos) * -0.5f + 0.5f) * vViewPortSize.y;


			_wstring strPointIndex = L"P." + to_wstring(i);
			m_pGameInstance->Add_DebugRender_Function([vScreenPos, strPointIndex]()->HRESULT
				{
					return CGameInstance::GetInstance()->Render_Font(L"Font_LiberationSans_20", strPointIndex.c_str(), vScreenPos, XMVectorSet(0.2f, 0.2f, 0.2f, 1.f), 0.f, {}, 1.f);
				});

		}
	}

	return S_OK;
}
#endif // _DEBUG


HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue; // break;


			if (pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT::A), pSourCell->Get_Point(CCell::POINT::B)))
				pSourCell->Set_Neighbor(CCell::LINE::AB, pDestCell);

			if (pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT::B), pSourCell->Get_Point(CCell::POINT::C)))
				pSourCell->Set_Neighbor(CCell::LINE::BC, pDestCell);

			if (pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT::C), pSourCell->Get_Point(CCell::POINT::A)))
				pSourCell->Set_Neighbor(CCell::LINE::CA, pDestCell);
		}
	}

	return S_OK;
}

void CNavigation::Sort_Points_ClockWise(_uint* TriangleIndices)
{
	_vector pA = XMLoadFloat3(&m_Points->Value()[TriangleIndices[(_uint)CCell::POINT::A]]);
	_vector pB = XMLoadFloat3(&m_Points->Value()[TriangleIndices[(_uint)CCell::POINT::B]]);
	_vector pC = XMLoadFloat3(&m_Points->Value()[TriangleIndices[(_uint)CCell::POINT::C]]);

	_vector AB = pB - pA;
	_vector AC = pC - pA;

	_vector Cross = XMVector3Cross(AB, AC);

	if (0 > XMVectorGetY(Cross))
		swap(TriangleIndices[(_uint)CCell::POINT::B], TriangleIndices[(_uint)CCell::POINT::C]);
}

vector<_uint> CNavigation::Get_CellIndices_By_Option(_uint iOptionMask)
{
	vector<_uint> CellIndices;
	CellIndices.reserve(m_Cells.size());

	for (_uint i = 0; i < (_uint)m_Cells.size(); ++i)
	{
		if (iOptionMask & (1 << m_Cells[i]->Get_Option()))
		{
			CellIndices.push_back(i);
		}
	}

	return CellIndices;
}

_bool CNavigation::Is_Accessible(_uint iStartIndex, _uint iGoalIndex, _uint iOptionMask)
{
	if (!m_OpenList.empty())
	{
		m_isOpenList[m_OpenList.front()] = false;
		m_OpenList.pop_front();
	}

	m_CloseList.push_back(iStartIndex);
	m_isCloseList[iStartIndex] = true;

	//vector<_int> NeighborIndices = m_Cells[iStartIndex]->Get_NeighborIndices();
	const array<_int, 3>& NeighborIndices = m_Cells[iStartIndex]->Get_NeightborIndices_Ref();
	for (const _int iNeighborIndex : NeighborIndices)
	{
		if (iNeighborIndex == iGoalIndex)
		{
			m_Cells[iNeighborIndex]->Set_Parent(iStartIndex);
			return true;
		}

		if (-1 == iNeighborIndex)
			continue;

		if (!(iOptionMask & 1 << m_Cells[iNeighborIndex]->Get_Option()))
			continue;

		if (false == m_isOpenList[iNeighborIndex] && false == m_isCloseList[iNeighborIndex])
		{
			m_Cells[iNeighborIndex]->Set_Parent(iStartIndex);
			m_OpenList.push_back(iNeighborIndex);
			m_isOpenList[iNeighborIndex] = true;
		}
	}

	if (m_OpenList.empty())
		return false;

	m_OpenList.sort([this, iStartIndex, iGoalIndex](_uint iSrc, _uint iDst)->_bool
		{
			_float fSrcPCost = m_Cells[iStartIndex]->Compute_Distance(m_Cells[iSrc]);
			_float fDstPCost = m_Cells[iStartIndex]->Compute_Distance(m_Cells[iDst]);


			_float fSrcGCost = m_Cells[iGoalIndex]->Compute_Distance(m_Cells[iSrc]);
			_float fDstGCost = m_Cells[iGoalIndex]->Compute_Distance(m_Cells[iDst]);


			return fSrcPCost + fSrcGCost < fDstPCost + fDstGCost;
		});


	return Is_Accessible(m_OpenList.front(), iGoalIndex, iOptionMask);
}

vector<pair<_float3, _float3>> CNavigation::Make_BestRoute(_uint iStartIndex, _uint iGoalIndex)
{
	// 적어도 다른 셀 인 경우에 호출됨 
	vector<pair<_float3, _float3>> CommonEdges;

	vector<_uint> BestRouteIndices;

	BestRouteIndices.push_back(iGoalIndex);


	_uint iPreIndex = iGoalIndex;
	_uint iRouteIndex = m_Cells[iGoalIndex]->Get_ParentIndex();

	pair<_uint, _uint> CommonEdgeIndex = m_Cells[iPreIndex]->Find_CommonEdge(m_Cells[iRouteIndex]);
	// 수상함 왜지? 항상 두번씩 들어가는ㅅ지 확인해보기
	CommonEdges.push_back({ m_Points->Value()[CommonEdgeIndex.first], m_Points->Value()[CommonEdgeIndex.second] });

	while (iPreIndex != iStartIndex)
	{
		CommonEdgeIndex = m_Cells[iPreIndex]->Find_CommonEdge(m_Cells[iRouteIndex]);

		_vector vCellCenter = m_Cells[iPreIndex]->Get_CenterPosition(); // 패딩 할떄 누구 쎈터 가져올거임?
		_vector EdgeA = XMLoadFloat3(&m_Points->Value()[CommonEdgeIndex.first]);
		_vector EdgeB = XMLoadFloat3(&m_Points->Value()[CommonEdgeIndex.second]);
		EdgeA = XMVector3Normalize(vCellCenter - EdgeA) * 0.2f + EdgeA;
		EdgeB = XMVector3Normalize(vCellCenter - EdgeB) * 0.2f + EdgeB;
		pair<_float3, _float3> EdgeWithPadding;
		XMStoreFloat3(&EdgeWithPadding.first, EdgeA);
		XMStoreFloat3(&EdgeWithPadding.second, EdgeB);

		//CommonEdges.push_back({ m_Points->Value()[CommonEdgeIndex.first], m_Points->Value()[CommonEdgeIndex.second] });
		CommonEdges.push_back(EdgeWithPadding);

		BestRouteIndices.push_back(iRouteIndex);
		iPreIndex = iRouteIndex;
		iRouteIndex = m_Cells[iRouteIndex]->Get_ParentIndex();
	}

	//reverse(BestRouteIndices.begin(), BestRouteIndices.end());
	reverse(CommonEdges.begin(), CommonEdges.end());
	return CommonEdges;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strNavigationDataFile)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strNavigationDataFile)))
	{
		MSG_BOX("Failed To Create : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Clear_AStar()
{
	m_OpenList.clear();
	m_CloseList.clear();

	m_isOpenList.assign(m_Cells.size(), false);
	m_isCloseList.assign(m_Cells.size(), false);
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const vector<vector<_uint>> Indices, const vector<_float3>& Points)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Indices, Points)))
	{
		MSG_BOX("Failed To Create : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CNavigation::Free()
{
	super::Free();

	for (auto& pCells : m_Cells)
		Safe_Release(pCells);
	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShaderCom);
#endif // _DEBUG

	//if (m_isPrototype)
	//{
	//	Safe_Delete(m_pWorldMatrix);
	//	Safe_Delete(m_pWorldMatrixInverse);
	//}

	//Safe_Release(m_pSharedData);

	Safe_Release(m_pWorldMatrix);
	Safe_Release(m_pWorldMatrixInverse);
	Safe_Release(m_NumInObjects);
	Safe_Release(m_Points);
}