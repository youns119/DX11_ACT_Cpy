#include "pch.h"
#include "NPC_Shark_Event_Knock.h"

#include "NPC_Shark.h"

CNPC_Shark_Event_Knock::CNPC_Shark_Event_Knock()
{
}

HRESULT CNPC_Shark_Event_Knock::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Shark_Event_Knock::Execute()
{
	static_cast<CNPC_Shark*>(m_pGameObject)->Event_Knock();
	return S_OK;
}

CNPC_Shark_Event_Knock* CNPC_Shark_Event_Knock::Create(CGameObject* _pGameObject)
{
	CNPC_Shark_Event_Knock* pInstance = new CNPC_Shark_Event_Knock;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CNPC_Shark_Event_Knock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Shark_Event_Knock::Free()
{
	super::Free();
}