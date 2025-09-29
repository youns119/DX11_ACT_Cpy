#include "Cell.h"

#ifdef _DEBUG
#include "VIBuffer_Cell.h"
#endif // _DEBUG


CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pContext{ pContext }
	, m_pDevice{ pDevice }
{
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

HRESULT CCell::Initialize(const array<_uint, POINT_END>& Indices, _int iIndex, const vector<_float3>& Points, _uint iOption)
{
	memcpy(m_PointIndices.data(), Indices.data(), sizeof(_uint) * POINT_END);

	m_iIndex = iIndex;
	m_iOption = iOption;

	Update_CacheData_From_PointIndices(Points);

#ifdef _DEBUG
	// usage_dynamic ���� ��������
	m_pVIBufferCom = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_Points.data()); // �������� ���°Ŷ� ������Ÿ�Ծ��� �ٷ� ����
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;
#endif

	return S_OK;
}

_bool CCell::Is_In(_fvector vPosition, _int& iNeighborIndex)
{
	for (_uint i = 0; i < LINE_END; ++i)
	{
		_vector vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_Points[i])); // SetY = 0 �ؾ����� �ʳ���? ���ص� �Ǵ°Ű��⵵�ϰ� ���ص� �ǳ� �������Ϳ��� ������ �����Ҷ� �� ���� ������ ���� �������� ��� �ֳĸ� �����ϸ� �ȴ�.
		_vector vLine = XMLoadFloat3(&m_Points[(i + 1) % LINE_END]) - XMLoadFloat3(&m_Points[i]);
		_vector vNormal = XMVector3Normalize(XMVectorSet(
			XMVectorGetZ(vLine) * -1.f,
			0.f,
			XMVectorGetX(vLine),
			0.f)); // ȸ����� ���� ����� �ľ��� �ۼ�

		if (0 < XMVectorGetX(XMVector3Dot(vDir, vNormal)))
		{
			iNeighborIndex = m_NeighborIndices[i];
			return false;
		}
	}

	return true;
}

//_int CCell::Get_Index(_fvector vPosition);
// ���� �˰��� Ǯ�� ������ �־���� �ε��� -1 �� ���� ó���ϴ°� �������� �ʴ´�. 
// �׸��� �� �� Is_In �Լ����� �ε��� �����ͼ� �װ� MovedIndex �� �޾Ƽ� ���ص� ���� ������? // �ش� ���� ��������ε� �̸��� ���� �ɸ���.

_bool CCell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{
	assert(!XMVector3Equal(vSourPoint, vDestPoint));
	// ���� ������ ������ ���ϸ� ������ Ƚ���� �پ����� �ڵ嵵 ���������

	// XMVectorEqual() vs XMVector3Equal() : ���� �˾Ƶα�

	//if (XMVector3Equal(vSourPoint, XMLoadFloat3(&m_Points[(_uint)POINT::A])))
	//{
	//	if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_Points[(_uint) POINT::B ])))
	//		return true;
	//	if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_Points[(_uint) POINT::C])))
	//		return true;
	//}
	//if (XMVector3Equal(vSourPoint, XMLoadFloat3(&m_Points[(_uint)POINT::B])))
	//{
	//	if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_Points[(_uint) POINT::C ])))
	//		return true;
	//	if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_Points[(_uint) POINT::A])))
	//		return true;
	//}
	//if (XMVector3Equal(vSourPoint, XMLoadFloat3(&m_Points[(_uint)POINT::C])))
	//{
	//	if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_Points[(_uint) POINT::B ])))
	//		return true;
	//	if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_Points[(_uint) POINT::A])))
	//		return true;
	//}

	 //�ƴϸ� ��¥�� ������ ����ε� �׳� �ݺ��� ������ �ǰ�
	for (_uint i = 0; i < POINT_END; ++i)
	{
		if (XMVector3Equal(vSourPoint, XMLoadFloat3(&m_Points[i])))
		{
			if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_Points[(i + 1) % POINT_END])))
				return true;
			if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_Points[(i + 2) % POINT_END])))
				return true;
		}
	}

	return false;
}

_vector CCell::Compute_Position_On_Plane(_fvector vPosition)
{
	// ����� ������ ���

	// ������ ���� �����̿��Ϸ��޴µ� �̰� �� �� ��� ���� 
	_vector vPlane = XMLoadFloat4(&m_vPlane);

	_vector vTemp = vPosition;
	vTemp = XMVectorSetY(vTemp, 0.f);
	vTemp = XMVectorSetW(vTemp, 1.f);

	_float fHeight = XMVectorGetX(XMVector4Dot(vPlane, vTemp)) / -m_vPlane.y;

	return XMVectorSetY(vPosition, fHeight);
}

vector<_float3> CCell::Get_Points() const
{
	vector<_float3> Points;
	Points.resize(POINT_END);
	memcpy(Points.data(), m_Points.data(), sizeof(_float3) * POINT_END);
	return Points;
}

vector<_uint> CCell::Get_PointIndices() const
{
	vector<_uint> Indices;
	Indices.resize(POINT_END);
	memcpy(Indices.data(), m_PointIndices.data(), sizeof(_uint) * POINT_END);
	return Indices;
}

vector<_int> CCell::Get_NeighborIndices() const
{
	vector<_int> NeighborIndices;
	NeighborIndices.resize(LINE_END);
	memcpy(NeighborIndices.data(), m_NeighborIndices.data(), sizeof(_int) * LINE_END); // memcpy �����ؼ� ����ض�
	return NeighborIndices;
}

const array<_int, 3>& CCell::Get_NeightborIndices_Ref() const
{
	return m_NeighborIndices;
}

void CCell::Update_CacheData_From_PointIndices(const vector<_float3>& Points)
{
	_float3 PointA = Points[m_PointIndices[(_uint)POINT::A]];
	_float3 PointB = Points[m_PointIndices[(_uint)POINT::B]];
	_float3 PointC = Points[m_PointIndices[(_uint)POINT::C]];
	m_Points = { PointA, PointB, PointC };

	XMStoreFloat4(&m_vPlane,
		XMPlaneFromPoints(XMLoadFloat3(&PointA), XMLoadFloat3(&PointB), XMLoadFloat3(&PointC)));


	_vector vA = XMLoadFloat3(&PointA);
	_vector vB = XMLoadFloat3(&PointB);
	_vector vC = XMLoadFloat3(&PointC);

	_vector vAB = vB - vA;
	_vector tmep = XMVector3Length(vAB);
	_vector vMidPointAB = vA + XMVector3Normalize(vAB) * XMVector3Length(vAB) * 0.5f; // PointM

	_vector vCM = vMidPointAB - vC;
	_vector vCenter = vC + XMVector3Normalize(vCM) * XMVector3Length(vCM) * 2.f / 3.f;
	XMStoreFloat3(&m_vCenter, vCenter);
}

HRESULT CCell::Save_Cell(HANDLE hFile)
{
	DWORD dwByte{};
	WriteFile(hFile, &m_iOption, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, m_PointIndices.data(), sizeof(_uint) * (_uint)m_PointIndices.size(), &dwByte, nullptr);

	if (0 == dwByte)
		return E_FAIL;

	return S_OK;
}

_bool CCell::Check_Containing(_uint iIndex)
{
	for (auto iPointIndex : m_PointIndices)
	{
		if (iIndex == iPointIndex)
			return TRUE;
	}

	return FALSE;
}

void CCell::Change_PointIndex(_uint iSrc, _uint iDst)
{
	for (_uint i = 0; i < POINT_END; ++i)
	{
		if (iSrc == m_PointIndices[i])
		{
			m_PointIndices[i] = iDst;
			break;
		}
	}
}

_bool CCell::Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist)
{
	_vector pA = XMVectorSetW(XMLoadFloat3(&m_Points[(_uint)POINT::A]), 1.f);
	_vector pB = XMVectorSetW(XMLoadFloat3(&m_Points[(_uint)POINT::B]), 1.f);
	_vector pC = XMVectorSetW(XMLoadFloat3(&m_Points[(_uint)POINT::C]), 1.f);

	return TriangleTests::Intersects(vRayOrigin, vRayDirection, pA, pB, pC, fDist);
}

pair<_uint, _uint> CCell::Find_CommonEdge(CCell* pOther)
{
	vector<_uint> CommonEdge;
	for (_uint i = 0; i < POINT_END; ++i)
	{
		if (m_PointIndices[i] == pOther->m_PointIndices[(_uint)POINT::A])
			CommonEdge.push_back(m_PointIndices[i]);

		if (m_PointIndices[i] == pOther->m_PointIndices[(_uint)POINT::B])
			CommonEdge.push_back(m_PointIndices[i]);

		if (m_PointIndices[i] == pOther->m_PointIndices[(_uint)POINT::C])
			CommonEdge.push_back(m_PointIndices[i]);
	}

	if (CommonEdge.size() > 2)
		_CrtDbgBreak();

	return pair<_uint, _uint> {CommonEdge[0], CommonEdge[1]};
}

_float CCell::Compute_Distance(CCell* pOther) const
{
	return XMVectorGetX(XMVector3Length((XMLoadFloat3(&m_vCenter) - XMLoadFloat3(&pOther->m_vCenter))));
	// y�� ���� �� �� �ְԴ�������

	//_vector vSrc = XMLoadFloat3(&m_vCenter) - XMLoadFloat3(&pOther->m_vCenter);
	//vSrc = vSrc * XMVectorSet(1.f, 3.f, 1.f, 0.f);
	//return XMVectorGetX(XMVector3Length(vSrc));
}

_vector CCell::Compute_SlidingVector(_fvector vPosition, _fvector vDeltaMove)
{
	_vector vSlidingVector = XMVectorZero();

	_vector vNormal[LINE_END]{};
	for (_uint i = 0; i < LINE_END; ++i)
	{
		_vector vDir = XMVector3Normalize(vPosition + vDeltaMove - XMLoadFloat3(&m_Points[i])); // SetY = 0 �ؾ����� �ʳ���? ���ص� �Ǵ°Ű��⵵�ϰ� ���ص� �ǳ� �������Ϳ��� ������ �����Ҷ� �� ���� ������ ���� �������� ��� �ֳĸ� �����ϸ� �ȴ�.
		_vector vLine = XMLoadFloat3(&m_Points[(i + 1) % LINE_END]) - XMLoadFloat3(&m_Points[i]);

		vNormal[i] = XMVector3Normalize(XMVectorSet(
			XMVectorGetZ(vLine) * -1.f,
			0.f,
			XMVectorGetX(vLine),
			0.f));

		if (0 < XMVectorGetX(XMVector3Dot(vDir, vNormal[i])))
		{
			// �ܺ�
			//if (-1 == m_NeighborIndices[i])
			{
				_vector vProjectedDeltaMove = XMVector3Dot(vDeltaMove, vNormal[i]) * vNormal[i];
				vSlidingVector += vDeltaMove - vProjectedDeltaMove;
			}
		}
	}

	//_int iNeighborIndex {};
	//if (false == Is_In(vPosition + vSlidingVector, iNeighborIndex))
	//{
	//	// ���� �̵����Ѿ��ϴµ� �켱�� �� �����̵��� �غ���
	//	return XMVectorZero();
	//}

	return vSlidingVector;
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
	m_pVIBufferCom->Bind_InputAssembler();

	m_pVIBufferCom->Render();

	return S_OK;
}
void CCell::Update_Buffer()
{
	m_pVIBufferCom->Update(m_Points.data(), (_uint)m_Points.size());
}
#endif // _DEBUG

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const array<_uint, POINT_END>& Indices, _int iIndex, const vector<_float3>& Points, _uint iOption)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(Indices, iIndex, Points, iOption)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed To Create : CCell");
	}

	return pInstance;
}

void CCell::Free()
{
	super::Free();

#ifdef _DEBUG
	Safe_Release(m_pVIBufferCom);
#endif // _DEBUG

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

}
