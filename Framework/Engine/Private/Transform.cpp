#include "Transform.h"
#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

// 이놈도 호출 될 일이 없어짐
//CTransform::CTransform(const CTransform& Prototype)
//	: CComponent{ Prototype }
//	, m_WorldMatrix{ Prototype.m_WorldMatrix }
//{
//}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	if (nullptr == pDesc)
		return S_OK;

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;
	m_WorldMatrix = pDesc->WorldMatrix;

	return S_OK;
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation, _uint iCellOptionMask)
{
	//vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	//if (nullptr != pNavigation && false == pNavigation->Is_Move(vPosition))
	//	return;

	//Set_State(STATE::POSITION, vPosition);

	_vector vPosition = Get_State(STATE::POSITION);
	_vector vLook = Get_State(STATE::LOOK);

	_vector vDeltaMove = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
		
	if (nullptr != pNavigation && false == pNavigation->Is_Move(vPosition + vDeltaMove))
	{
		_vector vSlidingVector = pNavigation->Compute_SlidingVector(vPosition, vDeltaMove);
		vDeltaMove = vSlidingVector;
		//return;
		if (false == pNavigation->Is_Move(vPosition + vDeltaMove))
			vDeltaMove = XMVectorZero();
	}

	Set_State(STATE::POSITION, vPosition + vDeltaMove);
}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation, _uint iCellOptionMask)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vLook = Get_State(STATE::LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation && false == pNavigation->Is_Move(vPosition))
		return;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation, _uint iCellOptionMask)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vRight = Get_State(STATE::RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation && false == pNavigation->Is_Move(vPosition))
		return;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation, _uint iCellOptionMask)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vRight = Get_State(STATE::RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation && false == pNavigation->Is_Move(vPosition))
		return;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Up(_float fTimeDelta, CNavigation* pNavigation, _uint iCellOptionMask)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vUp = Get_State(STATE::UP);

	vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation && false == pNavigation->Is_Move(vPosition))
		return;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Down(_float fTimeDelta, CNavigation* pNavigation, _uint iCellOptionMask)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vUp = Get_State(STATE::UP);

	vPosition -= XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation && false == pNavigation->Is_Move(vPosition))
		return;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Toward(_fvector vDirection, _float fTimeDelta, CNavigation* pNavigation, _uint iCellOptionMask)
{

	_vector vPosition = Get_State(STATE::POSITION);
	_vector vDeltaMove = XMVector3Normalize(vDirection) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation && false == pNavigation->Is_Move(vPosition + vDeltaMove))
	{
		_vector vSlidingVector = pNavigation->Compute_SlidingVector(vPosition, vDeltaMove);
		vDeltaMove = vSlidingVector;

		if (false == pNavigation->Is_Move(vPosition + vDeltaMove))
			vDeltaMove = XMVectorZero();
	}

	Set_State(STATE::POSITION, vPosition + vDeltaMove);
}

void CTransform::Move_By_Path(_float fTimeDelta, CNavigation* pNavigation, _fvector vGoalPosition, _float fDelay, _uint iOptionMask)
{
	m_fDelayTimeAcc += fTimeDelta;
	_vector vStartPosition = Get_State(STATE::POSITION);

	_vector vDirection = vGoalPosition - vStartPosition;
	if (XMVectorGetX(XMVector3Length(vDirection)) < 0.1f)
		return;

	//vector<pair<_float3, _float3>> PathEdge = pNavigation->Make_Path(vGoalPosition, iOptionMask);
	if (m_fDelayTimeAcc >= fDelay)
	{
		m_PathEdge = pNavigation->Make_Path(vGoalPosition, iOptionMask);
		m_fDelayTimeAcc = 0.f;
	}

	if (m_PathEdge.empty())
	{
		LookAt(vGoalPosition);
		Go_Toward(vDirection, fTimeDelta, pNavigation);
		return;
	}

	auto Is_CCW = ([](_fvector vBenchMark, _fvector vLeft, _fvector vRight)->_bool
		{
			return XMVectorGetY(XMVector3Cross(vLeft - vBenchMark, vRight - vBenchMark)) > 0.f;
		});

	auto Is_LeftRight = ([](_fvector vLeft, _fvector vRight)->_bool
		{
			return XMVectorGetY(XMVector3Cross(vLeft, vRight)) >= 0.f;
		});

	_vector vSrc = XMVectorSetW( XMLoadFloat3(&m_PathEdge[0].first), 1.f );
	_vector vDst = XMVectorSetW( XMLoadFloat3(&m_PathEdge[0].second), 1.f );
	_vector vTmp = vDst - vSrc;
	_vector vMiddle = vTmp * 0.5f + vSrc - vStartPosition; // dir

	_vector vPreLeft = XMVectorZero();
	_vector vPreRight = XMVectorZero();

	_vector vLeft = XMVectorZero();
	_vector vRight = XMVectorZero();

	if (Is_CCW(vStartPosition, vSrc, vDst))
	{
		vPreLeft = XMVector3Normalize(vSrc - vStartPosition);
		vPreRight = XMVector3Normalize(vDst - vStartPosition);
	}
	else
	{
		vPreLeft = XMVector3Normalize(vDst - vStartPosition);
		vPreRight = XMVector3Normalize(vSrc - vStartPosition);
	}


	for (size_t i = 1; i < m_PathEdge.size(); ++i)
	{
		vSrc = XMVectorSetW(XMLoadFloat3(&m_PathEdge[i].first), 1.f); // pos
		vDst = XMVectorSetW(XMLoadFloat3(&m_PathEdge[i].second), 1.f); // pos
		vTmp = vDst - vSrc; // dir
		vMiddle = vTmp * 0.5f + vSrc - vStartPosition; // dir

		if (Is_CCW(vStartPosition, vSrc, vDst))
		{
			vLeft = XMVector3Normalize(vSrc - vStartPosition); // dir
			vRight = XMVector3Normalize(vDst - vStartPosition); // dir
		}
		else
		{
			vLeft = XMVector3Normalize(vDst - vStartPosition);
			vRight = XMVector3Normalize(vSrc - vStartPosition);
		}


		if (Is_LeftRight(vPreLeft, vLeft)) // 더 좁아졌냐?
		{
			vPreLeft = vLeft;
		}

		if (Is_LeftRight(vRight, vPreRight)) // 더 좁아졌냐?
		{
			vPreRight = vRight;
		}

		if (Is_LeftRight(vRight, vPreLeft)) // 잘못됨 .. 이전 왼쪽보다 현재 오른쪽이 더 왼쪽에 있음
		{
			LookAt(vPreLeft + vStartPosition);
			//Go_Toward(vPreRight, fTimeDelta, pNavigation);
			Go_Straight(fTimeDelta, pNavigation);
			return;

			//vStartPosition = vPreLeft + vStartPosition;
		}

		if (Is_LeftRight(vPreRight, vLeft)) // 잘못됨 .. 
		{
			LookAt(vPreRight + vStartPosition);
			//Go_Toward(vPreRight, fTimeDelta, pNavigation);
			Go_Straight(fTimeDelta, pNavigation);
			return;

			//vStartPosition = vPreRight + vStartPosition;
		}

	}
	LookAt(vGoalPosition);
	Go_Straight(fTimeDelta, pNavigation);

}

void CTransform::Locate(const _float3& vLocation)
{
	//Set_State(STATE::POSITION, v(&vLocation));
	// 여긴 좀 더 고민해봐야겠다.
	memcpy(m_WorldMatrix.m[(_uint)STATE::POSITION], &vLocation, sizeof(_float3));
}

void CTransform::LookAt(_fvector vAt)
{
	_float3 vScale = Get_Scale();

	_vector vLook = vAt - Get_State(STATE::POSITION);
	vLook = XMVectorSetW(vLook, 0.f);
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::UpAt(_fvector vAt)
{
	_float3 vScale = Get_Scale();

	_vector vUp = vAt - Get_State(STATE::POSITION);
	vUp = XMVectorSetW(vUp, 0.f);
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vUp);
	_vector vLook = XMVector3Cross(vUp, vRight);

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::Turn(const _float3& vAxis, _float fTimeDelta)
{
	Turn(XMLoadFloat3(&vAxis), fTimeDelta);
}

void CTransform::Rotate(const _float3& vAxis, _float fRadians)
{
	Rotate(XMLoadFloat3(&vAxis), fRadians);
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	if (XMVector3Equal(vAxis, XMVectorZero()))
		return;

	_vector vRight = Get_State(STATE::RIGHT);
	_vector vUp = Get_State(STATE::UP);
	_vector vLook = Get_State(STATE::LOOK);

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotate(_fvector vAxis, _float fRadians)
{
	_float3 vScaled = Get_Scale();

	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fRadians);

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotate_By_State(STATE eState, _float fRadians)
{
	_vector vRight = Get_State(STATE::RIGHT);
	_vector vUp = Get_State(STATE::UP);
	_vector vLook = Get_State(STATE::LOOK);

	_vector vAxis = Get_State(eState);

	if (0 != XMVectorGetX(XMVectorEqual(vAxis, XMVectorZero())))
		return;

	_matrix RotationMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(vAxis, fRadians));
	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn_Pivot(_fvector vPivot, _gvector vAxis, _float fTimeDelta)
{
	_matrix matPivot = XMMatrixInverse(nullptr, XMMatrixTranslationFromVector(vPivot));
	//_matrix matPivot = XMMatrixTranslationFromVector(vPivot);
	_matrix matRotation = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	_matrix matFinal = matPivot * matRotation;

	XMStoreFloat4x4(&m_WorldMatrix, matFinal * XMLoadFloat4x4(&m_WorldMatrix));
}

void CTransform::Rotate(_float fX, _float fY, _float fZ)
{
	fX = XMConvertToRadians(fX);
	fY = XMConvertToRadians(fY);
	fZ = XMConvertToRadians(fZ);

	_float3 vScaled = Get_Scale();

	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_vector vQuaternion = XMQuaternionRotationRollPitchYaw(fX, fY, fZ);

	_matrix RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotate(_fvector vRotation)
{
	_float3 vScaled = Get_Scale();

	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix RotationMatrix = XMMatrixRotationQuaternion(vRotation);

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Set_Scale(const _float3& vScale)
{
	// 원래라면 음수도 들어가야 하지만 우리는 길이를 재기 때문에 항상 양수로 변경된다. 그래서 음수는 아예 막아둠
	if (0 < vScale.x)
		Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * vScale.x);
	if (0 < vScale.y)
		Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP)) * vScale.y);
	if (0 < vScale.z)
		Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK)) * vScale.z);
}

//_float3 CTransform::Compute_Scaled()
_float3 CTransform::Get_Scale()
{
	return _float3(
		XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),
		XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
		XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK)))
	);
}

_float3 CTransform::Get_Rotation()
{
	_float fPitch{}, fYaw{}, fRoll{};
	_float fZUp = m_WorldMatrix.m[2][1];

	fPitch = asin(-m_WorldMatrix.m[2][1]);

	if (abs(m_WorldMatrix.m[2][1]) < 0.9999f)
	{
		fYaw = atan2(m_WorldMatrix.m[2][0], m_WorldMatrix.m[2][2]);
		fRoll = atan2(m_WorldMatrix.m[0][1], m_WorldMatrix.m[1][1]);
	}
	else
	{
		fYaw = 0.0f;
		fRoll = std::atan2(-m_WorldMatrix.m[1][0], m_WorldMatrix.m[0][0]);
	}

	_float3 vRotation = _float3(fPitch, fYaw, fRoll);
	_float3 vRotationDegree = vRotation;
	vRotationDegree.x = XMConvertToDegrees(vRotation.x);
	vRotationDegree.y = XMConvertToDegrees(vRotation.y);
	vRotationDegree.z = XMConvertToDegrees(vRotation.z);

	return vRotationDegree;
}

void CTransform::Jump(_float JumpForce) // free fall 을 위한 초기 값 대입
{
	m_fVerticalVelocity = JumpForce;
}

_bool CTransform::FreeFall(_float fTimeDelta, CNavigation* pNavigation, _uint iCellOptionMask) // 위 아래로만 제어, 관성에 대한 움직임은 클라이언트가 알아서 하기
{
	// 클라이언트에서 네비게이션에 의해서 움직임이 제어되어있음, 셀 안에 있음이 보장된 상태

	m_fVerticalVelocity += s_fGravityAcc * fTimeDelta;
	const _vector vCurrentPosition = Get_State(STATE::POSITION);
	const _vector vCurrentPositionOnPlane = pNavigation->Compute_Position_On_Plane(vCurrentPosition);

	_vector vRayOrigin = vCurrentPosition;
	// is_move 는 위 아래 검사를 안한다. 오르막 경사가 있는 삼각형에서 앞으로 갔더니 삼각형의 아래에 위치하는 경우가 생길 수 있다.
	if (XMVectorGetY(vCurrentPosition) < XMVectorGetY(vCurrentPositionOnPlane))
		vRayOrigin = vCurrentPositionOnPlane + XMVectorSet(0.f, 0.01f, 0.f, 0.f); // 평면에 포함된 점에서 레이 캐스팅이 안되기때문에 

	_float fDist = 0.f;
	_uint iClosestCellIndex = 0;
	const _vector vVerticalDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	
	if (false == pNavigation->Ray_Cast(vRayOrigin, vVerticalDir, fDist, iClosestCellIndex, iCellOptionMask))
	{
		// 다음 위치 아래에 셀이 없는 경우
		// 보통 이전 위치로 돌아가야 하는데..  이전 위치를 알 수가 없다.
		// 근데 떨어질 곳 연직 아래 방향에 셀이 없는 경우는 아마 게임 만들면서 거의 없을것이다.
		// 그래도 우선 계속 떨어지게 만들어보자
		Set_State(STATE::POSITION, vCurrentPosition + XMVectorSet(0.f, m_fVerticalVelocity, 0.f, 0.f) * fTimeDelta);

		return true;
	}

	pNavigation->Move_CurrentCellIndex(iClosestCellIndex);
	_vector vClosestCellInVerticalDirection = vRayOrigin + fDist * vVerticalDir;

	_vector vMoveByGravity = vCurrentPosition + XMVectorSet(0.f, m_fVerticalVelocity, 0.f, 0.f) * fTimeDelta;
	if (XMVectorGetY(vMoveByGravity) < XMVectorGetY(vClosestCellInVerticalDirection))
	{
		vMoveByGravity = vClosestCellInVerticalDirection;
		Set_State(STATE::POSITION, vMoveByGravity);
		m_fVerticalVelocity = 0.f;
		return false;
	}

	Set_State(STATE::POSITION, vMoveByGravity);
	return true;
}

// shader 와 결합도가 높아지지만... Get worldMatrix 를 사용하지 않고 하는 방법또 있을까?
// Transform Component의 책임이 월드 행렬을 셰이더에 전달하는거 까지 인가? ? ? 
HRESULT CTransform::Bind_WorldMatrix_To_Shader(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
	//return pShader->Bind_Matrix(pConstantName, m_WorldMatrix);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

// 특별한 생성 루틴이 되어서 사용 할 일이 없어짐
CCloneable* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	CComponent::Free();
}
