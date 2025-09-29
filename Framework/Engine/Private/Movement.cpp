#include "Movement.h"

#include "Transform.h"

CMovement::CMovement(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CMovement::CMovement(const CMovement& Prototype)
	: CComponent(Prototype)
	, m_fRotationScale{ Prototype.m_fRotationScale }
	, m_fGravityScale{ Prototype.m_fGravityScale }
{
}

HRESULT CMovement::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMovement::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	m_fMoveAccel = pDesc->fMoveAccel;
	m_fMoveDecel = pDesc->fMoveDecel;
	m_fMaxSpeed = pDesc->fMaxSpeed;
	m_fMinSpeed = pDesc->fMinSpeed;
	m_fForceFriction = pDesc->fForceFriction;
	m_fRotationScale = pDesc->fRotationScale;

	m_fInAirControlRatio = pDesc->fInAirControlRatio;
	m_fMaxFallingSpeed = pDesc->fMaxFallingSpeed;
	m_fGravityScale = pDesc->fGravityScale;

	if (FAILED(super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMovement::Priority_Update(_float fTimeDelta)
{
	if (m_pUpdateComponent == nullptr || 
		fTimeDelta == 0.f)
		return;
}

void CMovement::Update(_float fTimeDelta)
{
	if (m_pUpdateComponent == nullptr || 
		!m_bActive || fTimeDelta == 0.f)
		return;

	// 중력 적용, 속도 계산
	if (!m_bFlyMode) Apply_Gravity(fTimeDelta);
	Calculate_Speed();

	// 수평 속도 적용
	m_pUpdateComponent->Set_Speed(m_fHorizontalSpeed);
	m_pUpdateComponent->Go_Toward(XMLoadFloat3(&m_vTotalHorizontalVelocity), fTimeDelta);

	// 수직 속도 적용
	m_pUpdateComponent->Set_Speed(m_fVerticalSpeed);
	m_pUpdateComponent->Go_Toward(XMLoadFloat3(&m_vTotalVerticalVelocity), fTimeDelta);

	if (m_bRotateToDirection) RotateToDirection(fTimeDelta);
	else if (m_bRotateToDirection_3D) RotateToDirection_3D(fTimeDelta);
}

void CMovement::Late_Update(_float fTimeDelta)
{
	if (m_pUpdateComponent == nullptr || 
		!m_bActive || fTimeDelta == 0.f)
		return;

	// 감속 적용
	// InputDir 초기화
	if (m_bFlyMode) Decrease_Velocity(fTimeDelta);
	else Decrease_HorizontalVelocity(fTimeDelta);

	if(_vec3(m_vForceVelocity).Length())
		Decrease_Force(fTimeDelta);

	XMStoreFloat3(&m_vInputDir, XMVectorZero());
}

void CMovement::Add_Velocity(_fvector vDir, _float fTimeDelta)
{
	// 입력된 방향에 가속도를 계산하여 더해주고
	// 최대 속도보다 빠르면 최대 속도로 조정해서 속도를 최종 계산
	_float fAccel = m_fMoveAccel;
	if (m_bInAir && !m_bFlyMode) fAccel *= m_fInAirControlRatio;

	_vector vVelocity = XMVector3Normalize(vDir) * fAccel * fTimeDelta;
	vVelocity = XMLoadFloat3(&m_vHorizontalVelocity) + XMLoadFloat3(&m_vVerticalVelocity) + vVelocity;

	if (XMVectorGetX(XMVector3Length(vVelocity)) >= m_fMaxSpeed)
		vVelocity = XMVector3Normalize(vVelocity) * m_fMaxSpeed;

	XMStoreFloat3(&m_vHorizontalVelocity, XMVectorSetY(vVelocity, 0.f));
	XMStoreFloat3(&m_vVerticalVelocity, XMVectorSetZ(XMVectorSetX(vVelocity, 0.f), 0.f));
}

void CMovement::Add_HorizontalVelocity(_fvector vDir, _float fTimeDelta)
{
	// 입력된 방향에 가속도를 계산하여 더해주고
	// 최대 속도보다 빠르면 최대 속도로 조정해서 수평 속도를 최종 계산
	_vector vHorizontalDir = XMVectorSetY(vDir, 0.f);

	_float fAccel = m_fMoveAccel;
	if (m_bInAir && !m_bFlyMode) fAccel *= m_fInAirControlRatio;

	_vector vHorizontalVelocity = XMVector3Normalize(vHorizontalDir) * fAccel * fTimeDelta;
	vHorizontalVelocity = XMLoadFloat3(&m_vHorizontalVelocity) + vHorizontalVelocity;

	if (XMVectorGetX(XMVector3Length(vHorizontalVelocity)) >= m_fMaxSpeed)
		vHorizontalVelocity = XMVector3Normalize(vHorizontalVelocity) * m_fMaxSpeed;

	XMStoreFloat3(&m_vHorizontalVelocity, vHorizontalVelocity);
}

void CMovement::Add_VerticalVelocity(_fvector vDir, _float fTimeDelta)
{
	// 입력된 방향에 가속도를 계산하여 더해주고
	// 최대 속도보다 빠르면 최대 속도로 조정해서 수직 속도를 최종 계산
	_vector vVerticalDir = XMVectorSetZ(XMVectorSetX(vDir, 0.f), 0.f);

	_float fAccel = m_fMoveAccel;
	if (m_bInAir && !m_bFlyMode) fAccel *= m_fInAirControlRatio;

	_vector vVerticalVelocity = XMVector3Normalize(vVerticalDir) * fAccel * fTimeDelta;
	vVerticalVelocity = XMLoadFloat3(&m_vVerticalVelocity) + vVerticalVelocity;

	if (XMVectorGetX(XMVector3Length(vVerticalVelocity)) >= m_fMaxSpeed)
		vVerticalVelocity = XMVector3Normalize(vVerticalVelocity) * m_fMaxSpeed;

	XMStoreFloat3(&m_vVerticalVelocity, vVerticalVelocity);
}

void CMovement::Add_Force(_fvector vDir, _float fForce)
{
	// 최대 속도에 상관없이 원하는 Force 만큼 무조건 속도를 더해준다
	_vector vVelocity = XMVector3Normalize(vDir) * fForce;
	vVelocity = _vec3(m_vForceVelocity)._vector() + vVelocity;

	XMStoreFloat3(&m_vForceVelocity, vVelocity);
}

void CMovement::TurnToDirection(_fvector _vDirection, _float fTimeDelta)
{
	// 입력된 방향으로 계속해서 회전한다
	_float fRotationScale = m_fRotationScale;
	if (m_bInAir && !m_bFlyMode) fRotationScale *= m_fInAirControlRatio;

	_vector vDirection = _vDirection - m_pUpdateComponent->Get_State(CTransform::STATE::POSITION);
	_vector vLook = m_pUpdateComponent->Get_State(CTransform::STATE::LOOK);

	vDirection = XMVectorSetY(vDirection, 0.f);
	vLook = XMVectorSetY(vLook, 0.f);

	_float fDegree = CUtils::GetBetweenAngle(vLook, vDirection);
	m_pUpdateComponent->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fDegree * 0.1f * fRotationScale * fTimeDelta);
}

void CMovement::Reset_Velocity()
{
	m_vHorizontalVelocity = _float3(0.f, 0.f, 0.f);
	m_vVerticalVelocity = _float3(0.f, 0.f, 0.f);
	m_vTotalHorizontalVelocity = _float3(0.f, 0.f, 0.f);
	m_vTotalVerticalVelocity = _float3(0.f, 0.f, 0.f);
	m_vForceVelocity = _float3(0.f, 0.f, 0.f);

	m_fHorizontalSpeed = 0.f;
	m_fVerticalSpeed = 0.f;

	m_vInputDir = _float3(0.f, 0.f, 0.f);
}

void CMovement::Reset_Horizontal()
{
	m_vHorizontalVelocity = _float3(0.f, 0.f, 0.f);
	m_vTotalHorizontalVelocity = _float3(0.f, 0.f, 0.f);
	m_vForceVelocity = _float3(0.f, m_vForceVelocity.y, 0.f);

	m_fHorizontalSpeed = 0.f;
}

void CMovement::Reset_Vertical()
{
	m_vVerticalVelocity = _float3(0.f, 0.f, 0.f);
	m_vTotalVerticalVelocity = _float3(0.f, 0.f, 0.f);
	m_vForceVelocity = _float3(m_vForceVelocity.x, 0.f, m_vForceVelocity.z);

	m_fVerticalSpeed = 0.f;
}

void CMovement::Decrease_HorizontalVelocity(_float fTimeDelta)
{
	// 현재 속도의 반대 방향으로 감속을 계산
	// 감속의 결과가 속도가 0이 아닌 완전히 반대 방향으로 나오면 0으로 조정해서 최종 계산
	if (_vec3(m_vHorizontalVelocity).Length() <= m_fMinSpeed)
		return;

	_float fDecel = m_fMoveDecel;
	if (m_bInAir && !m_bFlyMode) fDecel *= m_fInAirControlRatio;

	_vector vPreVelocity = XMLoadFloat3(&m_vHorizontalVelocity);
	_vector vHorizontalVelocity = XMVector3Normalize(XMLoadFloat3(&m_vHorizontalVelocity)) * -fDecel * fTimeDelta;

	vHorizontalVelocity = XMLoadFloat3(&m_vHorizontalVelocity) + vHorizontalVelocity;

	_float fVelX = XMVectorGetX(vHorizontalVelocity);
	_float fVelZ = XMVectorGetZ(vHorizontalVelocity);

	if (XMVectorGetX(vPreVelocity) * fVelX <= 0) fVelX = 0.f;
	if (XMVectorGetZ(vPreVelocity) * fVelZ <= 0) fVelZ = 0.f;

	vHorizontalVelocity = XMVectorSet(fVelX, 0.f, fVelZ, 0.f);
	XMStoreFloat3(&m_vHorizontalVelocity, vHorizontalVelocity);
}

void CMovement::Decrease_Velocity(_float fTimeDelta)
{
	// 현재 속도의 반대 방향으로 감속을 계산
	// 감속의 결과가 속도가 0이 아닌 완전히 반대 방향으로 나오면 0으로 조정해서 최종 계산
	if (_vec3(m_vHorizontalVelocity).Length() <= m_fMinSpeed &&
		_vec3(m_vVerticalVelocity).Length() <= m_fMinSpeed)
		return;

	_float fDecel = m_fMoveDecel;
	if (m_bInAir && !m_bFlyMode) fDecel *= m_fInAirControlRatio;

	_vector vPrevVelocity = XMLoadFloat3(&m_vVerticalVelocity) + XMLoadFloat3(&m_vHorizontalVelocity);
	_vector vVelocityDelta = XMVector3Normalize(vPrevVelocity) * -fDecel * fTimeDelta;

	_vector vPostVelocity = vPrevVelocity + vVelocityDelta;

	_float fVelX = XMVectorGetX(vPostVelocity);
	_float fVelY = XMVectorGetY(vPostVelocity);
	_float fVelZ = XMVectorGetZ(vPostVelocity);

	if (XMVectorGetX(vPrevVelocity) * fVelX <= 0) fVelX = 0.f;
	if (XMVectorGetY(vPrevVelocity) * fVelY <= 0) fVelY = 0.f;
	if (XMVectorGetZ(vPrevVelocity) * fVelZ <= 0) fVelZ = 0.f;

	vPostVelocity = XMVectorSet(fVelX, fVelY, fVelZ, 0.f);

	_vector vHorizontal = _vec3(_vec3(vPostVelocity).x, 0.f, _vec3(vPostVelocity).z)._vector();
	_vector vVertical = _vec3(0.f, _vec3(vPostVelocity).y, 0.f)._vector();

	XMStoreFloat3(&m_vHorizontalVelocity, vHorizontal);
	XMStoreFloat3(&m_vVerticalVelocity, vVertical);
}

void CMovement::Decrease_Force(_float fTimeDelta)
{
	_float fDecel = m_fForceFriction;
	_vector vPreVelocity{};
	_vector vForceVelocity{};

	if (m_bFlyMode)
		vPreVelocity = XMLoadFloat3(&m_vForceVelocity);
	else vPreVelocity = XMVectorSetY(_vec3(m_vForceVelocity)._vector(), 0.f);

	vForceVelocity = _vec3(vPreVelocity).Normalized()._vector() * -fDecel * fTimeDelta;
	vForceVelocity = vPreVelocity + vForceVelocity;

	_float fVelX = XMVectorGetX(vForceVelocity);
	_float fVelY{};
	_float fVelZ = XMVectorGetZ(vForceVelocity);
	if (m_bFlyMode)
		fVelY = XMVectorGetY(vForceVelocity);
	else fVelY = XMVectorGetY(XMLoadFloat3(&m_vForceVelocity));
	
	if (XMVectorGetX(vPreVelocity) * fVelX <= 0) fVelX = 0.f;
	if (XMVectorGetZ(vPreVelocity) * fVelZ <= 0) fVelZ = 0.f;
	if (m_bFlyMode)
		if (XMVectorGetY(vPreVelocity) * fVelY <= 0) fVelY = 0.f;

	_vector vPostVelocity = XMVectorSet(fVelX, fVelY, fVelZ, 0.f);

	XMStoreFloat3(&m_vForceVelocity, vPostVelocity);
}

void CMovement::Apply_Gravity(_float fTimeDelta)
{
	// 수직 속도에서 중력 방향으로 힘을 더해준다
	// 최대 낙하 속도를 넘지 않도록 조정해서 최종 계산
	_vector vGravity = XMVectorSet(0.f, -1.f, 0.f, 0.f) * GRAVITY * m_fGravityScale * fTimeDelta;
	_vector vVelocity = XMLoadFloat3(&m_vVerticalVelocity) + vGravity;
	_float fForceY = _vec3(m_vForceVelocity).y + _vec3(vGravity).y;

	// 낙하 속도 제한 (Force 포함)
	if (_vec3(vVelocity).Length() >= m_fMaxFallingSpeed &&
		XMVectorGetY(vVelocity) < 0.f)
	{
		_vec3 vNorm = _vec3(vVelocity).Normalized() * m_fMaxFallingSpeed;
		vVelocity = vNorm._vector();
	}
	if (_vec3(m_vForceVelocity).y * fForceY <= 0.f) fForceY = 0.f;

	XMStoreFloat3(&m_vVerticalVelocity, vVelocity);
	XMStoreFloat3(&m_vForceVelocity, XMVectorSetY(_vec3(m_vForceVelocity)._vector(), fForceY));
}

void CMovement::RotateToDirection(_float fTimeDelta)
{
	// 현재 속도의 방향으로 계속해서 회전시켜준다
	// RotationScale에 영향을 받음 (0.f ~ 1.f)
	if (_vec3(m_vTotalHorizontalVelocity).Length() == 0.f)
		return;

	_float fRotationScale = m_fRotationScale;
	if (m_bInAir && !m_bFlyMode) fRotationScale *= m_fInAirControlRatio;

	_float3 vVelocity = m_vTotalHorizontalVelocity;
	_vector vLook = m_pUpdateComponent->Get_State(CTransform::STATE::LOOK);

	vLook = XMVectorSetY(vLook, 0.f);

	_float fDegree = CUtils::GetBetweenAngle(vLook, XMLoadFloat3(&vVelocity));
	m_pUpdateComponent->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fDegree * 0.1f * fRotationScale * fTimeDelta);
}

void CMovement::RotateToDirection_3D(_float fTimeDelta)
{
	_vec3 vVelocity = _vec3(m_vTotalHorizontalVelocity) + _vec3(m_vTotalVerticalVelocity);
	if (vVelocity.Length() == 0.f)
		return;

	_vector vLook = XMVector3Normalize(vVelocity._vector());
	vLook = XMVectorSetW(vLook, 0.f);
	_vector	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	_matrix matRotation = { vRight, vUp, vLook, XMVectorSet(0.f, 0.f, 0.f, 1.f) };
	_vector vLookRotation = XMQuaternionRotationMatrix(matRotation);

	_matrix matTransform = m_pUpdateComponent->Get_WorldMatrix();
	_vector vRotation = CUtils::Get_MatrixQuaternion(matTransform);

	_vector vResult = XMQuaternionSlerp(vRotation, vLookRotation, fTimeDelta * m_fRotationScale);

	m_pUpdateComponent->Rotate(vResult);
}

void CMovement::Calculate_Speed()
{
	// 최종 수평, 수직 속도의 속력을 계산
	_vector vHorizontalForce = XMVectorSetY(_vec3(m_vForceVelocity)._vector(), 0.f);
	_vector vVerticalForce = XMVectorSetY(XMVectorZero(), _vec3(m_vForceVelocity).y);

	_vec3 vPrevTotalVelocity = _vec3(m_vTotalHorizontalVelocity) + _vec3(m_vTotalVerticalVelocity);

	m_vTotalHorizontalVelocity = (_vec3(m_vHorizontalVelocity) + _vec3(vHorizontalForce))._float3();
	m_vTotalVerticalVelocity = (_vec3(m_vVerticalVelocity) + _vec3(vVerticalForce))._float3();

	_vec3 vTotalVelocity = _vec3(m_vTotalHorizontalVelocity) + _vec3(m_vTotalVerticalVelocity);
	_float fTotalSpeed = vTotalVelocity.Length();
	if(fTotalSpeed <= m_fMinSpeed)
		vTotalVelocity = fTotalSpeed <= 0.f ? vPrevTotalVelocity.Normalized() * m_fMinSpeed : vTotalVelocity.Normalized() * m_fMinSpeed;

	XMStoreFloat3(&m_vTotalHorizontalVelocity, XMVectorSetY(vTotalVelocity._vector(), 0.f));
	XMStoreFloat3(&m_vTotalVerticalVelocity, XMVectorSetY(XMVectorZero(), vTotalVelocity.y));

	m_fHorizontalSpeed = _vec3(m_vTotalHorizontalVelocity).Length();

	m_fVerticalSpeed = _vec3(m_vTotalVerticalVelocity).Length();
	m_fVerticalSpeed = m_vTotalVerticalVelocity.y < 0 ? m_fVerticalSpeed * -1.f : m_fVerticalSpeed;
}

void CMovement::Set_UpdateComponent(CTransform* pTransform)
{
	// Movement 컴포넌트의 계산 결과를 넘겨주어 실제 매트릭스의 값을 변경해줄 Transform 컴포넌트를 설정한다
	m_pUpdateComponent = pTransform;
	Safe_AddRef(pTransform);
}

CMovement* CMovement::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMovement* pInstance = new CMovement(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CMovement");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CMovement::Clone(void* pArg)
{
	CMovement* pInstance = new CMovement(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CMovement");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMovement::Free()
{
	super::Free();

	Safe_Release(m_pUpdateComponent);
}