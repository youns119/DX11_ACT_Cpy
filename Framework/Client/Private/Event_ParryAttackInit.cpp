#include "pch.h"
#include "Event_ParryAttackInit.h"

#include "GameInstance.h"

CEvent_ParryAttackInit::CEvent_ParryAttackInit()
{
}

HRESULT CEvent_ParryAttackInit::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEvent_ParryAttackInit::Execute()
{
	CGameInstance::GetInstance()->Call_Lag(0.1f, 0.8f);

	return S_OK;
}

CEvent_ParryAttackInit* CEvent_ParryAttackInit::Create(CGameObject* _pGameObject)
{
	CEvent_ParryAttackInit* pInstance = new CEvent_ParryAttackInit;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_ParryAttackInit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_ParryAttackInit::Free()
{
	super::Free();
}