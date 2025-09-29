#include "pch.h"
#include "Event_BackStepOff.h"

#include "Player.h"

CEvent_BackStepOff::CEvent_BackStepOff()
{
}

HRESULT CEvent_BackStepOff::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayerStat = static_cast<CPlayer*>(m_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_BackStepOff::Execute()
{
	m_pPlayerStat->bBackStep = false;
	m_pPlayerStat->bInvincible = false;

	return S_OK;
}

CEvent_BackStepOff* CEvent_BackStepOff::Create(CGameObject* _pGameObject)
{
	CEvent_BackStepOff* pInstance = new CEvent_BackStepOff;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_BackStepOff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_BackStepOff::Free()
{
	super::Free();
}