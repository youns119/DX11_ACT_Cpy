#include "pch.h"
#include "NPC_Roland_Death_Move.h"

#include "NPC_Roland_Death.h"

CNPC_Roland_Death_Move::CNPC_Roland_Death_Move()
{
}

HRESULT CNPC_Roland_Death_Move::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Roland_Death_Move::Execute()
{
	static_cast<CNPC_Roland_Death*>(m_pGameObject)->Move();
	return S_OK;
}

CNPC_Roland_Death_Move* CNPC_Roland_Death_Move::Create(CGameObject* _pGameObject)
{
	CNPC_Roland_Death_Move* pInstance = new CNPC_Roland_Death_Move;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CNPC_Roland_Death_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Roland_Death_Move::Free()
{
	super::Free();
}