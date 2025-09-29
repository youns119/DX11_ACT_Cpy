#include "pch.h"
#include "Event_Effect_ChargingSeq02.h"
#include "GameInstance.h"
#include "Player.h"

CEvent_Effect_ChargingSeq02::CEvent_Effect_ChargingSeq02()
{
}

HRESULT CEvent_Effect_ChargingSeq02::Init(CGameObject* _pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pSocketMatrix)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pParentWorldMatrix = pParentWorldMatrix;
	m_pSocketMatrix = pSocketMatrix;

	return S_OK;
}

HRESULT CEvent_Effect_ChargingSeq02::Execute()
{
	_matrix CombinedMatrix{}, SocketMatrix{};
	_vector vRight = XMVectorSet(m_pParentWorldMatrix->_11, m_pParentWorldMatrix->_12, m_pParentWorldMatrix->_13, 0.f);
	_vector vUp = XMVectorSet(m_pParentWorldMatrix->_21, m_pParentWorldMatrix->_22, m_pParentWorldMatrix->_23, 0.f);
	_vector vLook = XMVectorSet(m_pParentWorldMatrix->_31, m_pParentWorldMatrix->_32, m_pParentWorldMatrix->_33, 0.f);

	SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	for (size_t i = 0; i < 3; ++i)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	CombinedMatrix = SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_CHARGING_SEQ02, CombinedMatrix.r[(_uint)CTransform::STATE::POSITION] + vLook + vUp * 0.25f, -vLook, false, nullptr);
	// 스프링암에 타겟이 설정 되었을 시 해당 타겟을 바라보도록 할 것(위치 지정 후 룩앳)
	return S_OK;
}

CEvent_Effect_ChargingSeq02* CEvent_Effect_ChargingSeq02::Create(CGameObject* _pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pSocketMatrix)
{
	CEvent_Effect_ChargingSeq02* pInstance = new CEvent_Effect_ChargingSeq02;

	if (FAILED(pInstance->Init(_pGameObject, pParentWorldMatrix, pSocketMatrix)))
	{
		MSG_BOX("Failed To Create : CEvent_Effect_ChargingSeq02");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Effect_ChargingSeq02::Free()
{
	super::Free();
}