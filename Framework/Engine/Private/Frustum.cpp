#include "Frustum.h"
#include "GameInstance.h"

CFrustum::CFrustum()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Initialize()
{
	m_OriginalPoints[0] = _float4(-1.f, 1.f, 0.f, 1.f);		// left top near
	m_OriginalPoints[1] = _float4(1.f, 1.f, 0.f, 1.f);		// right top near
	m_OriginalPoints[2] = _float4(1.f, -1.f, 0.f, 1.f);		// right bottom near
	m_OriginalPoints[3] = _float4(-1.f, -1.f, 0.f, 1.f);	// left bottom near

	m_OriginalPoints[4] = _float4(-1.f, 1.f, 1.f, 1.f);		// left top fat
	m_OriginalPoints[5] = _float4(1.f, 1.f, 1.f, 1.f);		// right top fat
	m_OriginalPoints[6] = _float4(1.f, -1.f, 1.f, 1.f);		// right bottom fat
	m_OriginalPoints[7] = _float4(-1.f, -1.f, 1.f, 1.f);	// left bottom fat

	return S_OK;
}

void CFrustum::Update()
{
	_matrix			ViewMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::VIEW);
	_matrix			ProjMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::PROJ);

	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat4(&m_WorldPoints[i], XMVector3TransformCoord(XMLoadFloat4(&m_OriginalPoints[i]), ProjMatrixInverse));
		XMStoreFloat4(&m_WorldPoints[i], XMVector3TransformCoord(XMLoadFloat4(&m_WorldPoints[i]), ViewMatrixInverse));
	}

	Make_Planes(m_WorldPoints, m_WorldPlanes);
}

void CFrustum::Transform_LocalSpace(_fmatrix WorldMatrix)
{
	_matrix	WorldMatrixInverse = XMMatrixInverse(nullptr, WorldMatrix);

	//_float4	vLocalPoints[8] {};
	array<_float4, 8> vLocalPoints {};

	for (size_t i = 0; i < 8; i++)
		XMStoreFloat4(&vLocalPoints[i], XMVector3TransformCoord(XMLoadFloat4(&m_WorldPoints[i]), WorldMatrixInverse));

	Make_Planes(vLocalPoints, m_LocalPlanes);
}

_bool CFrustum::Is_InWorldSpace(_fvector vWorldPoint, _float fRange) const
{
	/*a b c d
	x y z 1

		ax + by + cz + d = ? ;*/

	for (size_t i = 0; i < 6; i++)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_WorldPlanes[i]), vWorldPoint)))
			return false;
	}

	return true;
}

_bool CFrustum::Is_InLocalSpace(_fvector vLocalPoint, _float fRange) const
{

	for (size_t i = 0; i < 6; i++)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_LocalPlanes[i]), vLocalPoint)))
			return false;
	}

	return true;
}


void CFrustum::Make_Planes(const _float4* pPoints, _float4* pPlanes)
{
	/* +x */
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[6])));
	/* -x */
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(
		XMLoadFloat4(&pPoints[4]),
		XMLoadFloat4(&pPoints[0]),
		XMLoadFloat4(&pPoints[3])
	));

	/* +y */
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(
		XMLoadFloat4(&pPoints[4]),
		XMLoadFloat4(&pPoints[5]),
		XMLoadFloat4(&pPoints[1])
	));

	/* -y */
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(
		XMLoadFloat4(&pPoints[3]),
		XMLoadFloat4(&pPoints[2]),
		XMLoadFloat4(&pPoints[6])
	));

	/* +z */
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(
		XMLoadFloat4(&pPoints[5]),
		XMLoadFloat4(&pPoints[4]),
		XMLoadFloat4(&pPoints[7])
	));

	/* -z */
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(
		XMLoadFloat4(&pPoints[0]),
		XMLoadFloat4(&pPoints[1]),
		XMLoadFloat4(&pPoints[2])
	));

}

void CFrustum::Make_Planes(const array<_float4, 8>& Points, _Out_ array<_float4, 6>& Planes)
{
	XMStoreFloat4(&Planes[0], XMPlaneFromPoints(XMLoadFloat4(&Points[1]), XMLoadFloat4(&Points[5]), XMLoadFloat4(&Points[6])));
	/* -x */
	XMStoreFloat4(&Planes[1], XMPlaneFromPoints(
		XMLoadFloat4(&Points[4]),
		XMLoadFloat4(&Points[0]),
		XMLoadFloat4(&Points[3])
	));

	/* +y */
	XMStoreFloat4(&Planes[2], XMPlaneFromPoints(
		XMLoadFloat4(&Points[4]),
		XMLoadFloat4(&Points[5]),
		XMLoadFloat4(&Points[1])
	));

	/* -y */
	XMStoreFloat4(&Planes[3], XMPlaneFromPoints(
		XMLoadFloat4(&Points[3]),
		XMLoadFloat4(&Points[2]),
		XMLoadFloat4(&Points[6])
	));

	/* +z */
	XMStoreFloat4(&Planes[4], XMPlaneFromPoints(
		XMLoadFloat4(&Points[5]),
		XMLoadFloat4(&Points[4]),
		XMLoadFloat4(&Points[7])
	));

	/* -z */
	XMStoreFloat4(&Planes[5], XMPlaneFromPoints(
		XMLoadFloat4(&Points[0]),
		XMLoadFloat4(&Points[1]),
		XMLoadFloat4(&Points[2])
	));
}

CFrustum* CFrustum::Create()
{
	CFrustum* pInstance = new CFrustum();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CFrustum");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrustum::Free()
{
	super::Free();

	Safe_Release(m_pGameInstance);
}

