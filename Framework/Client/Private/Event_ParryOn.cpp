#include "pch.h"
#include "Event_ParryOn.h"

#include "Player.h"

CEvent_ParryOn::CEvent_ParryOn()
{
}

HRESULT CEvent_ParryOn::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayerStat = static_cast<CPlayer*>(m_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_ParryOn::Execute()
{
	m_pPlayerStat->bParry = true;
	m_pPlayerStat->bInvincible = true;

	return S_OK;
}

CEvent_ParryOn* CEvent_ParryOn::Create(CGameObject* _pGameObject)
{
	CEvent_ParryOn* pInstance = new CEvent_ParryOn;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_ParryOn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_ParryOn::Free()
{
	super::Free();
}