#include "SpringArm.h"

#include "GameInstance.h"

CSpringArm::CSpringArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CSpringArm::CSpringArm(const CSpringArm& Prototype)
	: super(Prototype)
{
}

HRESULT CSpringArm::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpringArm::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
		return E_FAIL;

	m_vOffset = pDesc->vOffset;
	m_vTargetOffset = pDesc->vTargetOffset;

	m_fArmLength = pDesc->fArmLength;
	m_fLagSpeed = pDesc->fLagSpeed;
	m_fLagRotation = pDesc->fLagRotation;

	m_fCollisionOffset = pDesc->fCollisionOffset;

	m_fPrevArmLength = m_fArmLength;
	m_vPrevPosition = _mat(XMLoadFloat4x4(m_pParentWorldMatrix)).position._float4();
	m_vPrevRotation = _vec4(CUtils::Get_MatrixQuaternion(XMLoadFloat4x4(m_pParentWorldMatrix)))._float4();

	m_pTransformCom->Rotate(_vec4(m_vPrevRotation)._vector());

	return S_OK;
}

void CSpringArm::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CSpringArm::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CSpringArm::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);
}

void CSpringArm::Final_Update(_float fTimeDelta)
{
	Calculate_Offset();

	if (m_bPhysicsCollision)
		PhysicsCollision();

	Calculate_ArmLength();
	Calculate_Combined();

	super::Final_Update(fTimeDelta);
}

HRESULT CSpringArm::Render()
{
	if (FAILED(super::Render()))
		return E_FAIL;

	return S_OK;
}

void CSpringArm::Calculate_Offset()
{
	_vec3 vOffset = _vec3(m_vOffset);
	_vec3 vTargetOffset = _vec3(m_vTargetOffset);

	_vec3 vRight = m_pTransformCom->Get_State(CTransform::STATE::RIGHT);
	_vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE::UP);
	_vec3 vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK);

	vOffset = vRight * vOffset.x + vUp * vOffset.y + vLook * vOffset.z;
	vTargetOffset = vRight * vTargetOffset.x + vUp * vTargetOffset.y + vLook * vTargetOffset.z;

	m_vStartOffset = vTargetOffset._float3();
	m_vDirOffset = vOffset.Normalized()._float3();
}

void CSpringArm::Calculate_Combined()
{
	_matrix matParent = XMMatrixIdentity();

	if (m_bPhysicsCollision && m_fCollisionDist > 0.f)
		m_vDirOffset = (_vec3(m_vDirOffset) * m_fCollisionDist)._float3();
	else if (m_fPrevArmLength > 0.f)
		m_vDirOffset = (_vec3(m_vDirOffset) * m_fPrevArmLength)._float3();

	if (!m_bInheritRotation)
	{
		_vector vScale = CUtils::Get_MatrixScale(XMLoadFloat4x4(m_pParentWorldMatrix));
		_vector vRotation = Calculate_RotationLag();
		_vector vPosition = Calculate_PositionLag();
		vPosition = vPosition + _vec3(m_vStartOffset)._vector() + _vec3(m_vDirOffset)._vector();

		matParent = XMMatrixAffineTransformation(vScale, XMVectorZero(), vRotation, vPosition);
	}

	XMStoreFloat4x4(&m_CombinedWorldMatrix, matParent);
}

void CSpringArm::Calculate_ArmLength()
{
	if (fabs(m_fArmLength - m_fPrevArmLength) >= m_fEpsilon)
		m_fPrevArmLength = CUtils::Lerp_Float(m_fPrevArmLength, m_fArmLength, m_fLagSpeed);
	else m_fPrevArmLength = m_fArmLength;

	if (m_fPrevArmLength < 0.f) m_fPrevArmLength = 0.f;
}

_vector CSpringArm::Calculate_PositionLag()
{
	_vector vCurrPosition = _mat(XMLoadFloat4x4(m_pParentWorldMatrix)).position._vector();

	if (!m_bActiveLag)
	{
		XMStoreFloat4(&m_vPrevPosition, vCurrPosition);
		return vCurrPosition;
	}

	_vector vLerpPosition = XMVectorLerp(XMLoadFloat4(&m_vPrevPosition), vCurrPosition, m_fLagSpeed);

	if (XMVectorGetX(XMVector3Length(vCurrPosition - XMLoadFloat4(&m_vPrevPosition))) < m_fEpsilon)
		vLerpPosition = vCurrPosition;

	XMStoreFloat4(&m_vPrevPosition, vLerpPosition);

	return vLerpPosition;
}

_vector CSpringArm::Calculate_RotationLag()
{
	_vector vCurrRotation = CUtils::Get_MatrixQuaternion(m_pTransformCom->Get_WorldMatrix());

	if (!m_bActiveLag)
	{
		XMStoreFloat4(&m_vPrevRotation, vCurrRotation);
		return vCurrRotation;
	}

	_float fDot = _vec4(XMVector4Dot(XMLoadFloat4(&m_vPrevRotation), vCurrRotation)).x;
	if (fDot < 0.f)
		vCurrRotation = XMVectorNegate(vCurrRotation);

	_vector vLerpRotation = XMQuaternionSlerp(XMLoadFloat4(&m_vPrevRotation), vCurrRotation, m_fLagRotation);

	if (_vec3(XMQuaternionLength(vCurrRotation - _vec4(m_vPrevRotation)._vector())).x < m_fEpsilon)
		vLerpRotation = vCurrRotation;

	XMStoreFloat4(&m_vPrevRotation, vLerpRotation);

	return vLerpRotation;
}

void CSpringArm::PhysicsCollision()
{
	_vector vPosition = _mat(XMLoadFloat4x4(m_pParentWorldMatrix)).position._vector() + XMLoadFloat3(&m_vStartOffset);
	_vector vLook = _mat(m_pTransformCom->Get_WorldMatrix()).look._vector() * -1.f;

	if (m_pGameInstance->Ray_Cast_To_Physics(vPosition, vLook, m_fPrevArmLength, m_fCollisionDist, 1))
		m_fCollisionDist -= m_fCollisionOffset;
	else m_fCollisionDist = 0.f;
}

CSpringArm* CSpringArm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpringArm* pInstance = new CSpringArm(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CSpringArm");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CSpringArm::Clone(void* pArg)
{
	CSpringArm* pInstance = new CSpringArm(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CSpringArm");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpringArm::Free()
{
	super::Free();
}