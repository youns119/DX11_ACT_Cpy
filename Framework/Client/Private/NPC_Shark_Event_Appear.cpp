#include "pch.h"
#include "NPC_Shark_Event_Appear.h"

#include "NPC_Shark.h"

CNPC_Shark_Event_Appear::CNPC_Shark_Event_Appear()
{
}

HRESULT CNPC_Shark_Event_Appear::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Shark_Event_Appear::Execute()
{
	static_cast<CNPC_Shark*>(m_pGameObject)->Event_Appear();
	return S_OK;
}

CNPC_Shark_Event_Appear* CNPC_Shark_Event_Appear::Create(CGameObject* _pGameObject)
{
	CNPC_Shark_Event_Appear* pInstance = new CNPC_Shark_Event_Appear;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CNPC_Shark_Event_Appear");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Shark_Event_Appear::Free()
{
	super::Free();
}