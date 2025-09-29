#include "pch.h"
#include "NPC_Inkerton_Final_Look.h"

#include "NPC_Inkerton_Final.h"

CNPC_Inkerton_Final_Look::CNPC_Inkerton_Final_Look()
{
}

HRESULT CNPC_Inkerton_Final_Look::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Inkerton_Final_Look::Execute()
{
	static_cast<CNPC_Inkerton_Final*>(m_pGameObject)->Look();
	return S_OK;
}

CNPC_Inkerton_Final_Look* CNPC_Inkerton_Final_Look::Create(CGameObject* _pGameObject)
{
	CNPC_Inkerton_Final_Look* pInstance = new CNPC_Inkerton_Final_Look;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CNPC_Inkerton_Final_Look");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Inkerton_Final_Look::Free()
{
	super::Free();
}