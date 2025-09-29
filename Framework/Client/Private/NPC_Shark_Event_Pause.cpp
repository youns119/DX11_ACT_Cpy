#include "pch.h"
#include "NPC_Shark_Event_Pause.h"

#include "NPC_Shark.h"

CNPC_Shark_Event_Pause::CNPC_Shark_Event_Pause()
{
}

HRESULT CNPC_Shark_Event_Pause::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Shark_Event_Pause::Execute()
{
	static_cast<CNPC_Shark*>(m_pGameObject)->Pause_CineCamera();

	return S_OK;
}

CNPC_Shark_Event_Pause* CNPC_Shark_Event_Pause::Create(CGameObject* _pGameObject)
{
	CNPC_Shark_Event_Pause* pInstance = new CNPC_Shark_Event_Pause;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CNPC_Shark_Event_Pause");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Shark_Event_Pause::Free()
{
	super::Free();
}