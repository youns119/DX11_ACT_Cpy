#include "pch.h"
#include "NPC_Chitan_KnockBack.h"

#include "NPC_Fighter.h"

CNPC_Chitan_KnockBack::CNPC_Chitan_KnockBack()
{
}

HRESULT CNPC_Chitan_KnockBack::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Chitan_KnockBack::Execute()
{
	static_cast<CNPC_Fighter*>(m_pGameObject)->KnockBack_Player();
	return S_OK;
}

CNPC_Chitan_KnockBack* CNPC_Chitan_KnockBack::Create(CGameObject* _pGameObject)
{
	CNPC_Chitan_KnockBack* pInstance = new CNPC_Chitan_KnockBack;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CNPC_Chitan_KnockBack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Chitan_KnockBack::Free()
{
	super::Free();
}