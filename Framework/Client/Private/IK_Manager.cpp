#include "pch.h"
#include "IK_Manager.h"
#include "GameInstance.h"

#include "IK_Segment.h"
#include "Transform.h"
#include "Bone.h"

#include "Arrow.h"

CIK_Manager::CIK_Manager()
{
}

CIK_Manager::CIK_Manager(const CIK_Manager& Prototype)
{
}

HRESULT CIK_Manager::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CIK_Manager::Initialize(void* _pDesc)
{
	IK_DESC* pDesc = static_cast<IK_DESC*>(_pDesc);
	if (nullptr != pDesc) {
		m_pOwner = pDesc->pOwner;
		m_pRootBoneTransform = pDesc->pRootBoneTransform;
		m_fConstraintAngle = pDesc->fConstraintAngle;
		m_fConstraintMaxDistance = pDesc->fConstraintMaxDistance;
		m_fConstraintMinDistance = pDesc->fConstraintMinDistance;
		m_SegmentGroups.resize(pDesc->iSegmentGroupSize);		
		m_fLerpSpeed = pDesc->fLerpSpeed;
		m_fMaxHeight = pDesc->fMaxHeight;
	}
	else return E_FAIL;

	return S_OK;
}

void CIK_Manager::Update(_float fTimeDelta)
{
	if (!bSegmenPlaced) {
		Reset_Position();
		bSegmenPlaced = true;
	}

	if (m_bOperate)
		Adjust_Segment(fTimeDelta);

//#ifdef _DEBUG
//	ImGui::Begin("Tset");
//	ImGui::DragFloat("Constraint MaxDistance", &m_fConstraintMaxDistance);
//	ImGui::DragFloat("Constraint MinDistance", &m_fConstraintMinDistance);
//	ImGui::DragFloat("Constraint Angle", &m_fConstraintAngle);
//	//ImGui::DragFloat("Default Distance", &m_fDefaultDistance);
//
//	ImGui::DragFloat("Step Height", &m_fMaxHeight);
//	ImGui::DragFloat("Lerp Speed", &m_fLerpSpeed);
//
//	ImGui::End();
//
//
//
//	ImGui::Begin("State Debugger");
//	ImGui::NewLine();
//	_float x = m_SegmentGroups[0].Segments[1]->Get_TargetPosition()->x;
//	_float y = m_SegmentGroups[0].Segments[1]->Get_TargetPosition()->y;
//	_float z = m_SegmentGroups[0].Segments[1]->Get_TargetPosition()->z;
//
//	ImGui::Text("Target Pos: %.2f, %.2f, %.2f ",x,y,z);
//	//ImGui::Text("State Group: %s", m_Enums[m_iCurGroup]);
//
//	ImGui::End();
//#endif // _DEBUG
}

void CIK_Manager::Reset_Position()
{
	for (_int i = 0; i < m_SegmentGroups.size(); i++)
	{
		_vector vTargetPosition{};
		m_SegmentGroups[i].Segments[1]->Set_LerpStartPosition(XMLoadFloat4(m_SegmentGroups[i].Segments[1]->Get_TargetPosition()));
		m_SegmentGroups[i].Segments[1]->Set_TargetPosition(Compute_TargetPosition(m_SegmentGroups[i].Segments));		
		vTargetPosition = XMLoadFloat4(m_SegmentGroups[i].Segments[1]->Get_TargetPosition());

		Reposition_SegmentGroup(m_SegmentGroups[i].Segments, vTargetPosition);
	}
}

void CIK_Manager::Adjust_Segment(_float fTimeDelta)
{
	_vector vTargetPosition{};
	_bool bDoReposition = false;

	for (_int i = 0; i < m_SegmentGroups.size(); i++)
	{				
		bDoReposition = Compare_Distance(m_SegmentGroups[i].Segments[1]);
		if (!bDoReposition)
			bDoReposition = Compare_Angle(m_SegmentGroups[i].Segments);

		if (bDoReposition) {			
			m_SegmentGroups[i].Segments[1]->Set_LerpStartPosition(XMLoadFloat4(m_SegmentGroups[i].Segments[1]->Get_CurPosition()));
			m_SegmentGroups[i].Segments[1]->Set_TargetPosition(Compute_TargetPosition(m_SegmentGroups[i].Segments));
			m_SegmentGroups[i].bIsMoving = true;
		}

		vTargetPosition = XMLoadFloat4(m_SegmentGroups[i].Segments[1]->Get_TargetPosition());

		if (m_SegmentGroups[i].bIsMoving) {
			m_SegmentGroups[i].fLerpFactor += fTimeDelta * m_fLerpSpeed;
			if (m_SegmentGroups[i].fLerpFactor > 1.f) {
				m_SegmentGroups[i].Segments[1]->Set_LerpStartPosition(vTargetPosition);
				m_SegmentGroups[i].fLerpFactor = 0.f;
				m_SegmentGroups[i].bIsMoving = false;

				m_SegmentGroups[i].Segments[1]->Set_TargetPosition(Compute_TargetPosition(m_SegmentGroups[i].Segments));
				vTargetPosition = XMLoadFloat4(m_SegmentGroups[i].Segments[1]->Get_TargetPosition());			
			}
			else {
				_vector StartPos = XMLoadFloat4(m_SegmentGroups[i].Segments[1]->Get_LerpStartPosition());
				vTargetPosition = Compute_InterporatedTargetPosition(StartPos, vTargetPosition, m_SegmentGroups[i].fLerpFactor);
			}
		}

		Reposition_SegmentGroup(m_SegmentGroups[i].Segments, vTargetPosition);
	}
}

void CIK_Manager::Reposition_SegmentGroup(vector<CIK_Segment*> _pSegmentGroup, _vector _vTargetPosition)
{
	_vector vCurrentPosition{};
	_vector vCurTargetPosition = _vTargetPosition;

	_matrix seg1 = XMLoadFloat4x4(_pSegmentGroup[0]->Get_WorldSpace());
	_matrix seg2 = XMLoadFloat4x4(_pSegmentGroup[1]->Get_WorldSpace());

	_vector vRootSegmentPosition = seg1.r[(_uint)CTransform::STATE::POSITION];
	_vector vNextSegmentPosition = seg2.r[(_uint)CTransform::STATE::POSITION];
	_float  fBoneLength = XMVectorGetX(XMVector4Length(vNextSegmentPosition - vRootSegmentPosition));

	vector<_vector> Positions{};
	for (_int i = (_pSegmentGroup.size() - 1); i >= 0; i--)
	{
		vCurrentPosition = XMLoadFloat4x4(_pSegmentGroup[i]->Get_WorldSpace()).r[(_uint)CTransform::STATE::POSITION];
		_vector LookDirection = XMVector3Normalize(vCurTargetPosition - vCurrentPosition);
		vCurrentPosition = vCurTargetPosition;
		LookDirection = -LookDirection;

		Positions.push_back(vCurrentPosition);
		vCurTargetPosition = XMVectorAdd(vCurrentPosition, XMVectorScale(LookDirection, fBoneLength));
	}

	vCurTargetPosition = vRootSegmentPosition;

	for (_int i = 0; i < _pSegmentGroup.size(); i++)
	{
		vCurrentPosition = Positions[(_pSegmentGroup.size() - 1) - i];
		_vector vLookDirection = XMVector3Normalize(vCurTargetPosition - vCurrentPosition);
		vCurrentPosition = vCurTargetPosition;
		vLookDirection = -vLookDirection;

		vCurTargetPosition = XMVectorAdd(vCurrentPosition, XMVectorScale(vLookDirection, fBoneLength));
		_pSegmentGroup[i]->Set_CurPosition(vCurrentPosition);		
		_pSegmentGroup[i]->Set_TargetPosition(_vTargetPosition);
		_pSegmentGroup[i]->Set_ToePosition(vCurTargetPosition);
		_pSegmentGroup[i]->Reposition();
	}
}

_vector CIK_Manager::Compute_TargetPosition(vector<CIK_Segment*> _pSegmentGroup)
{
	_vector vRootForward = XMVector3Normalize(m_pRootBoneTransform->Get_State(CTransform::STATE::LOOK));
	_vector vRootPosition = m_pRootBoneTransform->Get_State(CTransform::STATE::POSITION);
	_vector vUp = XMVector3Normalize(m_pRootBoneTransform->Get_State(CTransform::STATE::UP));

	_float fAngleDeg = _pSegmentGroup[0]->Get_BaseAngle();
	_float fRad = XMConvertToRadians(fAngleDeg);
	_matrix matRotY = XMMatrixRotationAxis(vUp, fRad);

	_vector vTargetDir = XMVector3TransformNormal(vRootForward, matRotY);
	vTargetDir = XMVector3Normalize(vTargetDir);

	_float fDis = _pSegmentGroup[0]->Get_DefaultDistance();
	_vector vTargetPosition = vRootPosition + XMVectorScale(vTargetDir, fDis);		
	XMVectorSetY(vTargetPosition, XMVectorGetY(vRootPosition));

	_float fDistance;
	if (Detect_Ground(vTargetPosition, fDistance))
		vTargetPosition = XMVectorSetY(vTargetPosition, XMVectorGetY(vRootPosition) + (3.f - fDistance));
	else
		vTargetPosition = XMVectorSetY(vTargetPosition, 0.f);

	return vTargetPosition;
}

_vector CIK_Manager::Compute_InterporatedTargetPosition(_fvector _StartPos, _fvector _TargetPos, _float _fFactor)
{
	_vector vTargetPosition = _StartPos * (1 - _fFactor) + _TargetPos * _fFactor;
	_float height = sin(_fFactor * 3.141592) *  m_fMaxHeight;
	height += XMVectorGetY(_TargetPos);
	vTargetPosition = XMVectorSetY(vTargetPosition, height);
	return vTargetPosition;
}

_bool CIK_Manager::Detect_Ground(_fvector vRayOrigin, _float& _fDistance)
{	
	_vec3 vOrigin = _vec3(XMVectorGetX(vRayOrigin), XMVectorGetY(vRayOrigin) + 3.f, XMVectorGetZ(vRayOrigin));
	_vec3 vDirection = _vec3(0.f, -1.f, 0.f);
	_float fMaxDistance = 40.f;

	_bool bIsHit = CGameInstance::GetInstance()->Ray_Cast_To_Physics(vOrigin, vDirection, fMaxDistance, _fDistance, PxQueryFlag::eSTATIC);	
	return bIsHit;
}

_bool CIK_Manager::Compare_Distance(CIK_Segment* _pJointSegment)
{
	_matrix pSegWorldMat = XMLoadFloat4x4(_pJointSegment->Get_WorldSpace());
	_matrix pRootSegWorldMat = XMLoadFloat4x4(&m_pRootBoneTransform->Get_WorldMatrix_Ref());
	_vector vCurPos = XMLoadFloat4(_pJointSegment->Get_TargetPosition());
	vCurPos = XMVectorSetY(vCurPos, 0.f);
	_vector vRootPos = pRootSegWorldMat.r[(_uint)CTransform::STATE::POSITION];
	vRootPos = XMVectorSetY(vRootPos, 0.f);

	_float fDistance = XMVectorGetX(XMVector3Length(vCurPos - vRootPos));

	if (fDistance > m_fConstraintMaxDistance || fDistance < m_fConstraintMinDistance) {
		return true;
	}

	return false;
}

_bool CIK_Manager::Compare_Angle(vector<CIK_Segment*> _pSegmentGroup)
{
	_matrix pSegTransform = XMLoadFloat4x4(_pSegmentGroup[0]->Get_WorldSpace());
	_matrix pSubSegTransform = XMLoadFloat4x4(_pSegmentGroup[1]->Get_WorldSpace());
	_matrix pRootSegTransform = XMLoadFloat4x4(&m_pRootBoneTransform->Get_WorldMatrix_Ref());

	_vector vCurLook = XMLoadFloat4(_pSegmentGroup[0]->Get_ToePosition()) - XMLoadFloat4(_pSegmentGroup[0]->Get_CurPosition());
	vCurLook = XMVectorSetY(vCurLook, 0.f);
	vCurLook = XMVector3Normalize(vCurLook);

	_vector vRootLook = pRootSegTransform.r[(_uint)CTransform::STATE::LOOK];
	vRootLook = XMVectorSetY(vRootLook, 0.f);
	vRootLook = XMVector3Normalize(vRootLook);

	_float fDotValue = XMVectorGetX(XMVector3Dot(vRootLook, vCurLook));
	fDotValue = max(-1.0f, min(fDotValue, 1.0f));

	_float fAngle = acosf(fDotValue);
	_float fCurAngle = XMConvertToDegrees(fAngle);

	_vector vCross = XMVector3Cross(vRootLook, vCurLook);
	_float fSign = XMVectorGetY(vCross);
	if (fSign < 0.f)
		fCurAngle *= -1.f;

	_vector vRootForward = XMVector3Normalize(m_pRootBoneTransform->Get_State(CTransform::STATE::LOOK));
	_vector vRootPosition = m_pRootBoneTransform->Get_State(CTransform::STATE::POSITION);
	_vector vUp = XMVector3Normalize(m_pRootBoneTransform->Get_State(CTransform::STATE::UP));

	_float fAngleDeg = _pSegmentGroup[0]->Get_BaseAngle();
	_float fRad = XMConvertToRadians(fAngleDeg);
	_matrix matRotY = XMMatrixRotationAxis(vUp, fRad);

	_vector vTargetDir = XMVector3TransformNormal(vRootForward, matRotY);
	vTargetDir = XMVectorSetY(vTargetDir, 0.f);
	vTargetDir = XMVector3Normalize(vTargetDir);

	fDotValue = XMVectorGetX(XMVector3Dot(vRootLook, vTargetDir));
	fDotValue = max(-1.0f, min(fDotValue, 1.0f));

	fAngle = acosf(fDotValue);
	_float fBaseAngle = XMConvertToDegrees(fAngle);

	vCross = XMVector3Cross(vRootLook, vTargetDir);
	fSign = XMVectorGetY(vCross);
	if (fSign < 0.f)
		fBaseAngle *= -1.f;

	_float fMax = fBaseAngle + m_fConstraintAngle;
	_float fMin = fBaseAngle - m_fConstraintAngle;
	if (fCurAngle > fMax || fCurAngle < fMin) {
		return true;
	}

	return false;
}

CCloneable* CIK_Manager::Clone(void* pArg)
{
	CIK_Manager* pInstance = new CIK_Manager(*this);

	//if (FAILED(pInstance->Initialize_Prototype()))
	//{
	//	MSG_BOX("Failed To Create : CIK_Manager");
	//	Safe_Release(pInstance);
	//}

	return pInstance;
}

CIK_Manager* CIK_Manager::Create(void* pArg)
{
	CIK_Manager* pInstance = new CIK_Manager();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CIK_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIK_Manager::Free()
{
	for (auto& pSegmentGroup : m_SegmentGroups)
	{
		for (auto& Segment : pSegmentGroup.Segments)
			Safe_Release(Segment);

		pSegmentGroup.Segments.clear();
	}
	m_SegmentGroups.clear();
}