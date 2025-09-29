#include "pch.h"
#include "NPC_Shark_Event_Bite.h"

#include "NPC_Shark.h"

CNPC_Shark_Event_Bite::CNPC_Shark_Event_Bite()
{
}

HRESULT CNPC_Shark_Event_Bite::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Shark_Event_Bite::Execute()
{
	static_cast<CNPC_Shark*>(m_pGameObject)->Event_Bite();
	return S_OK;
}

CNPC_Shark_Event_Bite* CNPC_Shark_Event_Bite::Create(CGameObject* _pGameObject)
{
	CNPC_Shark_Event_Bite* pInstance = new CNPC_Shark_Event_Bite;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CNPC_Shark_Event_Bite");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Shark_Event_Bite::Free()
{
	super::Free();
}