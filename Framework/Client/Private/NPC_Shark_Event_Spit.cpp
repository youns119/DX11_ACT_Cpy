#include "pch.h"
#include "NPC_Shark_Event_Spit.h"

#include "NPC_Shark.h"

CNPC_Shark_Event_Spit::CNPC_Shark_Event_Spit()
{
}

HRESULT CNPC_Shark_Event_Spit::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Shark_Event_Spit::Execute()
{
	static_cast<CNPC_Shark*>(m_pGameObject)->Event_Spit();
	return S_OK;
}

CNPC_Shark_Event_Spit* CNPC_Shark_Event_Spit::Create(CGameObject* _pGameObject)
{
	CNPC_Shark_Event_Spit* pInstance = new CNPC_Shark_Event_Spit;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CNPC_Shark_Event_Spit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Shark_Event_Spit::Free()
{
	super::Free();
}