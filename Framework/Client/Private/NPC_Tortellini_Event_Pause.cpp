#include "pch.h"
#include "NPC_Tortellini_Event_Pause.h"

#include "NPC_Tortellini.h"

CNPC_Tortellini_Event_Pause::CNPC_Tortellini_Event_Pause()
{
}

HRESULT CNPC_Tortellini_Event_Pause::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Tortellini_Event_Pause::Execute()
{
	static_cast<CNPC_Tortellini*>(m_pGameObject)->Pause_CineCamera();

	return S_OK;
}

CNPC_Tortellini_Event_Pause* CNPC_Tortellini_Event_Pause::Create(CGameObject* _pGameObject)
{
	CNPC_Tortellini_Event_Pause* pInstance = new CNPC_Tortellini_Event_Pause;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CNPC_Tortellini_Event_Pause");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Tortellini_Event_Pause::Free()
{
	super::Free();
}