#include "pch.h"
#include "Event_Hammer_Change.h"

#include "GameInstance.h"
#include "Player.h"

CEvent_Hammer_Change::CEvent_Hammer_Change()
{
}

HRESULT CEvent_Hammer_Change::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_Hammer_Change::Execute()
{
	m_pPlayer->Change_Hammer();

	CGameInstance::GetInstance()->Call_Lag(0.1f, 0.08f);

	return S_OK;
}

CEvent_Hammer_Change* CEvent_Hammer_Change::Create(CGameObject* _pGameObject)
{
	CEvent_Hammer_Change* pInstance = new CEvent_Hammer_Change;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_Hammer_Change");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Hammer_Change::Free()
{
	super::Free();
}