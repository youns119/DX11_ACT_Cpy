#include "pch.h"
#include "Petroch_Event_DivePunch.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"

CPetroch_Event_DivePunch::CPetroch_Event_DivePunch()
{
}

HRESULT CPetroch_Event_DivePunch::Execute()
{
	static_cast<CBoss_Petroch*>(m_pGameObject)->Dive_Punch();
	return S_OK;
}

CPetroch_Event_DivePunch* CPetroch_Event_DivePunch::Create(CGameObject* _pGameObject)
{
	CPetroch_Event_DivePunch* pInstance = new CPetroch_Event_DivePunch();

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CPetroch_Event_DivePunch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPetroch_Event_DivePunch::Free()
{
	__super::Free();
}
