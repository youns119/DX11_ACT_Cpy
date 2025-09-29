#include "pch.h"
#include "Event_EvadeOn.h"

#include "Player.h"

CEvent_EvadeOn::CEvent_EvadeOn()
{
}

HRESULT CEvent_EvadeOn::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayerStat = static_cast<CPlayer*>(m_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_EvadeOn::Execute()
{
	m_pPlayerStat->bEvade = true;
	m_pPlayerStat->bInvincible = true;

	return S_OK;
}

CEvent_EvadeOn* CEvent_EvadeOn::Create(CGameObject* _pGameObject)
{
	CEvent_EvadeOn* pInstance = new CEvent_EvadeOn;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_EvadeOn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_EvadeOn::Free()
{
	super::Free();
}