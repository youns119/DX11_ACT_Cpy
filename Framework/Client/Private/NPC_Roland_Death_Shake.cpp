#include "pch.h"
#include "NPC_Roland_Death_Shake.h"

#include "NPC_Roland_Death.h"

CNPC_Roland_Death_Shake::CNPC_Roland_Death_Shake()
{
}

HRESULT CNPC_Roland_Death_Shake::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Roland_Death_Shake::Execute()
{
	static_cast<CNPC_Roland_Death*>(m_pGameObject)->Shake();
	return S_OK;
}

CNPC_Roland_Death_Shake* CNPC_Roland_Death_Shake::Create(CGameObject* _pGameObject)
{
	CNPC_Roland_Death_Shake* pInstance = new CNPC_Roland_Death_Shake;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CNPC_Roland_Death_Shake");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Roland_Death_Shake::Free()
{
	super::Free();
}