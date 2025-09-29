#include "pch.h"
#include "Event_ChargeOff.h"

#include "Player.h"

CEvent_ChargeOff::CEvent_ChargeOff()
{
}

HRESULT CEvent_ChargeOff::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayerStat = static_cast<CPlayer*>(_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_ChargeOff::Execute()
{
	m_pPlayerStat->bCharging = false;

	return S_OK;
}

CEvent_ChargeOff* CEvent_ChargeOff::Create(CGameObject* _pGameObject)
{
	CEvent_ChargeOff* pInstance = new CEvent_ChargeOff;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_ChargeOff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_ChargeOff::Free()
{
	super::Free();
}