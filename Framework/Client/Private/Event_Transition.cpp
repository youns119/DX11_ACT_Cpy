#include "pch.h"
#include "Event_Transition.h"

#include "Player.h"

CEvent_Transition::CEvent_Transition()
{
}

HRESULT CEvent_Transition::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayerStat = static_cast<CPlayer*>(_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_Transition::Execute()
{
	m_pPlayerStat->bTransition = true;

	return S_OK;
}

CEvent_Transition* CEvent_Transition::Create(CGameObject* _pGameObject)
{
	CEvent_Transition* pInstance = new CEvent_Transition;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_Transition");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Transition::Free()
{
	super::Free();
}