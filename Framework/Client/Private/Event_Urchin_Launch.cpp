#include "pch.h"
#include "Event_Urchin_Launch.h"

#include "Player.h"

CEvent_Urchin_Launch::CEvent_Urchin_Launch()
{
}

HRESULT CEvent_Urchin_Launch::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayerStat = static_cast<CPlayer*>(_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_Urchin_Launch::Execute()
{
	m_pPlayerStat->bUrchinLaunch = true;

	return S_OK;
}

CEvent_Urchin_Launch* CEvent_Urchin_Launch::Create(CGameObject* _pGameObject)
{
	CEvent_Urchin_Launch* pInstance = new CEvent_Urchin_Launch;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_Urchin_Launch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Urchin_Launch::Free()
{
	super::Free();
}