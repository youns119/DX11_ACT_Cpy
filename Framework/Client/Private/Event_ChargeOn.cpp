#include "pch.h"
#include "Event_ChargeOn.h"

#include "Player.h"

CEvent_ChargeOn::CEvent_ChargeOn()
{
}

HRESULT CEvent_ChargeOn::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayerStat = static_cast<CPlayer*>(_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_ChargeOn::Execute()
{
	m_pPlayerStat->bCharging = true;

	return S_OK;
}

CEvent_ChargeOn* CEvent_ChargeOn::Create(CGameObject* _pGameObject)
{
	CEvent_ChargeOn* pInstance = new CEvent_ChargeOn;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_ChargeOn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_ChargeOn::Free()
{
	super::Free();
}