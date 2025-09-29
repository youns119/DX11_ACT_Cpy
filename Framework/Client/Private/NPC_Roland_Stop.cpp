#include "pch.h"
#include "NPC_Roland_Stop.h"

#include "NPC_Roland.h"

CNPC_Roland_Stop::CNPC_Roland_Stop()
{
}

HRESULT CNPC_Roland_Stop::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Roland_Stop::Execute()
{
	static_cast<CNPC_Roland*>(m_pGameObject)->Stop();
	return S_OK;
}

CNPC_Roland_Stop* CNPC_Roland_Stop::Create(CGameObject* _pGameObject)
{
	CNPC_Roland_Stop* pInstance = new CNPC_Roland_Stop;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CNPC_Roland_Stop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Roland_Stop::Free()
{
	super::Free();
}