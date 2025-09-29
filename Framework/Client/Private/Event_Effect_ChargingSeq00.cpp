#include "pch.h"
#include "Event_Effect_ChargingSeq00.h"
#include "GameInstance.h"
#include "Player.h"

CEvent_Effect_ChargingSeq00::CEvent_Effect_ChargingSeq00()
{
}

HRESULT CEvent_Effect_ChargingSeq00::Init(CGameObject* _pGameObject, const _float4x4* pCombinedWorldMatrix)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pCombinedWorldMatrix = pCombinedWorldMatrix;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_Effect_ChargingSeq00::Execute()
{
	_matrix CombinedMatrix = XMLoadFloat4x4(m_pCombinedWorldMatrix);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_CHARGING_SEQ00, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), true, m_pCombinedWorldMatrix);
	// 스프링암에 타겟이 설정 되었을 시 해당 타겟을 바라보도록 할 것(위치 지정 후 룩앳)

	m_pPlayer->Play_PlayerSound(L"ChargeAttack_Effect");

	return S_OK;
}

CEvent_Effect_ChargingSeq00* CEvent_Effect_ChargingSeq00::Create(CGameObject* _pGameObject, const _float4x4* pCombinedWorldMatrix)
{
	CEvent_Effect_ChargingSeq00* pInstance = new CEvent_Effect_ChargingSeq00;

	if (FAILED(pInstance->Init(_pGameObject, pCombinedWorldMatrix)))
	{
		MSG_BOX("Failed To Create : CEvent_Effect_ChargingSeq00");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Effect_ChargingSeq00::Free()
{
	super::Free();
}