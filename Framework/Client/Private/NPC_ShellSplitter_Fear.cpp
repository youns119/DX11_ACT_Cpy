#include "pch.h"
#include "NPC_ShellSplitter_Fear.h"

#include "GameInstance.h"
CNPC_ShellSplitter_Fear::CNPC_ShellSplitter_Fear()
{
}

HRESULT CNPC_ShellSplitter_Fear::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_ShellSplitter_Fear::Execute()
{
	CGameInstance::GetInstance()->Play_Sound(L"Executioner_FearV3.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK1, 0.5f);
	return S_OK;
}

CNPC_ShellSplitter_Fear* CNPC_ShellSplitter_Fear::Create(CGameObject* _pGameObject)
{
	CNPC_ShellSplitter_Fear* pInstance = new CNPC_ShellSplitter_Fear;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CNPC_ShellSplitter_Fear");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_ShellSplitter_Fear::Free()
{
	super::Free();
}