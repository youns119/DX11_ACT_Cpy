#include "pch.h"
#include "Petroch_Event_Pounce.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"

CPetroch_Event_Pounce::CPetroch_Event_Pounce()
{
}

HRESULT CPetroch_Event_Pounce::Execute()
{
	static_cast<CBoss_Petroch*>(m_pGameObject)->Pounce();
	return S_OK;
}

CPetroch_Event_Pounce* CPetroch_Event_Pounce::Create(CGameObject* _pGameObject)
{
	CPetroch_Event_Pounce* pInstance = new CPetroch_Event_Pounce();

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CPetroch_Event_Pounce");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPetroch_Event_Pounce::Free()
{
	__super::Free();
}
