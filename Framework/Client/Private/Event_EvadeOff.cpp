#include "pch.h"
#include "Event_EvadeOff.h"

#include "Player.h"

CEvent_EvadeOff::CEvent_EvadeOff()
{
}

HRESULT CEvent_EvadeOff::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayerStat = static_cast<CPlayer*>(m_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_EvadeOff::Execute()
{
	m_pPlayerStat->bEvade = false;
	m_pPlayerStat->bInvincible = false;

	return S_OK;
}

CEvent_EvadeOff* CEvent_EvadeOff::Create(CGameObject* _pGameObject)
{
	CEvent_EvadeOff* pInstance = new CEvent_EvadeOff;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_EvadeOff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_EvadeOff::Free()
{
	super::Free();
}