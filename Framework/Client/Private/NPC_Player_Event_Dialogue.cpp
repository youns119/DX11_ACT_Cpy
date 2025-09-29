#include "pch.h"
#include "NPC_Player_Event_Dialogue.h"

#include "NPC_Player.h"

CNPC_Player_Event_Dialogue::CNPC_Player_Event_Dialogue()
{
}

HRESULT CNPC_Player_Event_Dialogue::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Player_Event_Dialogue::Execute()
{
	static_cast<CNPC_Player*>(m_pGameObject)->Show_Dialogue();

	return S_OK;
}

CNPC_Player_Event_Dialogue* CNPC_Player_Event_Dialogue::Create(CGameObject* _pGameObject)
{
	CNPC_Player_Event_Dialogue* pInstance = new CNPC_Player_Event_Dialogue;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CNPC_Player_Event_Dialogue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Player_Event_Dialogue::Free()
{
	super::Free();
}