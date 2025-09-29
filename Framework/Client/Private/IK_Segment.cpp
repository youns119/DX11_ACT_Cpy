#include "pch.h"
#include "IK_Segment.h"

#include "Bone.h"

CIK_Segment::CIK_Segment()
{
}

HRESULT CIK_Segment::Initialize(void* _pDesc)
{
	IK_Segment_DESC* pDesc = static_cast<IK_Segment_DESC*>(_pDesc);
	if (nullptr != pDesc) {
		m_pCombinedWorldMatrix = pDesc->pCombinedMatrix;
		m_fBaseAngle = pDesc->fBaseAngle;
		m_fPitchOffset = pDesc->fPitchOffset;
		m_fDefaultDistance = pDesc->fDefaultDistance;
		m_pOwner = pDesc->pOwner;			
	}
	else
		return E_FAIL;

	_vector vCurPos = XMLoadFloat4x4(Get_WorldSpace()).r[(_uint)CTransform::STATE::POSITION];
	XMStoreFloat4(&m_vCurPoistion, vCurPos);
	XMStoreFloat4(&m_vTargetPoistion, vCurPos);

	return S_OK;
}
 
void CIK_Segment::Update(_float fTimeDelta)
{
}

void CIK_Segment::Reposition()
{			
	_matrix WorldMatrix = XMLoadFloat4x4(Get_WorldSpace());	
	
	_matrix LookMat = WorldMatrix;
	LookMat.r[(_uint)CTransform::STATE::POSITION] = XMLoadFloat4(&m_vToePoistion);

	_matrix PosMat = WorldMatrix;
	PosMat.r[(_uint)CTransform::STATE::POSITION] = XMLoadFloat4(&m_vCurPoistion);

	CTransform* pParent = static_cast<CTransform*>(m_pOwner->Find_Component(g_strTransformTag));	
	_matrix LocalLookMatrix = LookMat * pParent->Get_WorldMatrix_Inversed();
	_matrix LocalPosMatrix = PosMat * pParent->Get_WorldMatrix_Inversed();

	_float3 LocalScale(
		XMVectorGetX(XMVector3Length(XMLoadFloat4x4(m_pCombinedWorldMatrix).r[(_uint)CTransform::STATE::RIGHT])),
		XMVectorGetX(XMVector3Length(XMLoadFloat4x4(m_pCombinedWorldMatrix).r[(_uint)CTransform::STATE::UP])),
		XMVectorGetX(XMVector3Length(XMLoadFloat4x4(m_pCombinedWorldMatrix).r[(_uint)CTransform::STATE::LOOK]))
	);

	_vector vLook = XMVector3Normalize(LocalLookMatrix.r[(_uint)CTransform::STATE::POSITION] - LocalPosMatrix.r[(_uint)CTransform::STATE::POSITION]);
	vLook = XMVectorSetW(vLook, 0.f);		
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);	

	_matrix PitchRotation = XMMatrixRotationAxis(vRight, XMConvertToRadians(m_fPitchOffset));
	vLook = XMVector3TransformNormal(vLook, PitchRotation);		
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
		
	_matrix LocalMat = XMMatrixIdentity();
	LocalMat.r[(_uint)CTransform::STATE::POSITION] = LocalPosMatrix.r[(_uint)CTransform::STATE::POSITION];
	LocalMat.r[(_uint)CTransform::STATE::LOOK] = XMVectorScale(vLook, LocalScale.z) * -1.2f;
	LocalMat.r[(_uint)CTransform::STATE::RIGHT] = XMLoadFloat4x4(m_pCombinedWorldMatrix).r[(_uint)CTransform::STATE::RIGHT];
	LocalMat.r[(_uint)CTransform::STATE::UP] = XMVectorScale(vUp, LocalScale.y);
	XMStoreFloat4x4(m_pCombinedWorldMatrix, LocalMat);
}

_float4x4* CIK_Segment::Get_WorldSpace()
{
	CTransform* pParent = static_cast<CTransform*>(m_pOwner->Find_Component(g_strTransformTag));
	XMStoreFloat4x4(&m_pWorldMatrix, XMLoadFloat4x4(m_pCombinedWorldMatrix) * pParent->Get_WorldMatrix());
	return &m_pWorldMatrix;
}

CCloneable* CIK_Segment::Clone(void* pArg)
{
	return nullptr;
}

CIK_Segment* CIK_Segment::Create(void* pDesc)
{
	CIK_Segment* pInstance = new CIK_Segment();

	if (pInstance->Initialize(pDesc)) {
		MSG_BOX("Failed To Create : CIK_Segment");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIK_Segment::Free()
{
}