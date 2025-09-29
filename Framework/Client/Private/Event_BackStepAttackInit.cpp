#include "pch.h"
#include "Event_BackStepAttackInit.h"

#include "GameInstance.h"

CEvent_BackStepAttackInit::CEvent_BackStepAttackInit()
{
}

HRESULT CEvent_BackStepAttackInit::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEvent_BackStepAttackInit::Execute()
{
	CGameInstance::GetInstance()->Call_Lag(0.1f, 0.8f);

	return S_OK;
}

CEvent_BackStepAttackInit* CEvent_BackStepAttackInit::Create(CGameObject* _pGameObject)
{
	CEvent_BackStepAttackInit* pInstance = new CEvent_BackStepAttackInit;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_BackStepAttackInit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_BackStepAttackInit::Free()
{
	super::Free();
}