#include "pch.h"
#include "Event_ParryOff.h"

#include "Player.h"

CEvent_ParryOff::CEvent_ParryOff()
{
}

HRESULT CEvent_ParryOff::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayerStat = static_cast<CPlayer*>(m_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_ParryOff::Execute()
{
	m_pPlayerStat->bParry = false;
	m_pPlayerStat->bInvincible = false;

	return S_OK;
}

CEvent_ParryOff* CEvent_ParryOff::Create(CGameObject* _pGameObject)
{
	CEvent_ParryOff* pInstance = new CEvent_ParryOff;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_ParryOff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_ParryOff::Free()
{
	super::Free();
}