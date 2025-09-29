#include "pch.h"
#include "Petroch_Event_GroundSlam.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"

CPetroch_Event_GroundSlam::CPetroch_Event_GroundSlam()
{
}

HRESULT CPetroch_Event_GroundSlam::Execute()
{
	static_cast<CBoss_Petroch*>(m_pGameObject)->Ground_Slam();
	return S_OK;
}

CPetroch_Event_GroundSlam* CPetroch_Event_GroundSlam::Create(CGameObject* _pGameObject)
{
	CPetroch_Event_GroundSlam* pInstance = new CPetroch_Event_GroundSlam();

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CPetroch_Event_GroundSlam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPetroch_Event_GroundSlam::Free()
{
	__super::Free();
}
