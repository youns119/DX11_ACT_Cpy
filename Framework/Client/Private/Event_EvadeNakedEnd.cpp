#include "pch.h"
#include "Event_EvadeNakedEnd.h"

#include "Player.h"
#include "Movement.h"

CEvent_EvadeNakedEnd::CEvent_EvadeNakedEnd()
{
}

HRESULT CEvent_EvadeNakedEnd::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayerStat = static_cast<CPlayer*>(_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_EvadeNakedEnd::Execute()
{
	m_pMovement->Set_GravityScale(m_pPlayerStat->fGravityScale);
	m_pMovement->Set_InAirControlRatio(m_pPlayerStat->fInAirControlRatio);

	return S_OK;
}

CEvent_EvadeNakedEnd* CEvent_EvadeNakedEnd::Create(CGameObject* _pGameObject)
{
	CEvent_EvadeNakedEnd* pInstance = new CEvent_EvadeNakedEnd;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_EvadeNakedEnd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_EvadeNakedEnd::Free()
{
	super::Free();
}