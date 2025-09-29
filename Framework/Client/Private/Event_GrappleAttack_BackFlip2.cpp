#include "pch.h"
#include "Event_GrappleAttack_BackFlip2.h"

#include "GameInstance.h"

CEvent_GrappleAttack_BackFlip2::CEvent_GrappleAttack_BackFlip2()
{
}

HRESULT CEvent_GrappleAttack_BackFlip2::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEvent_GrappleAttack_BackFlip2::Execute()
{
	CGameInstance::GetInstance()->Call_Lag(0.1f, 1.3f);

	return S_OK;
}

CEvent_GrappleAttack_BackFlip2* CEvent_GrappleAttack_BackFlip2::Create(CGameObject* _pGameObject)
{
	CEvent_GrappleAttack_BackFlip2* pInstance = new CEvent_GrappleAttack_BackFlip2;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_GrappleAttack_BackFlip2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_GrappleAttack_BackFlip2::Free()
{
	super::Free();
}