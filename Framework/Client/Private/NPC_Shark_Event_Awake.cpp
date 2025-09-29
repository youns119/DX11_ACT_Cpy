#include "pch.h"
#include "NPC_Shark_Event_Awake.h"

#include "NPC_Shark.h"

CNPC_Shark_Event_Awake::CNPC_Shark_Event_Awake()
{
}

HRESULT CNPC_Shark_Event_Awake::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Shark_Event_Awake::Execute()
{
	static_cast<CNPC_Shark*>(m_pGameObject)->Event_Awake();
	return S_OK;
}

CNPC_Shark_Event_Awake* CNPC_Shark_Event_Awake::Create(CGameObject* _pGameObject)
{
	CNPC_Shark_Event_Awake* pInstance = new CNPC_Shark_Event_Awake;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CNPC_Shark_Event_Awake");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Shark_Event_Awake::Free()
{
	super::Free();
}