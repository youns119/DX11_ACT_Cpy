#include "pch.h"
#include "Event_BackStepOn.h"

#include "Player.h"

CEvent_BackStepOn::CEvent_BackStepOn()
{
}

HRESULT CEvent_BackStepOn::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayerStat = static_cast<CPlayer*>(m_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_BackStepOn::Execute()
{
	m_pPlayerStat->bBackStep = true;
	m_pPlayerStat->bInvincible = true;

	return S_OK;
}

CEvent_BackStepOn* CEvent_BackStepOn::Create(CGameObject* _pGameObject)
{
	CEvent_BackStepOn* pInstance = new CEvent_BackStepOn;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_BackStepOn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_BackStepOn::Free()
{
	super::Free();
}