#include "pch.h"
#include "Event_NormalAttackEnd.h"

#include "Player.h"
#include "Movement.h"

CEvent_NormalAttackEnd::CEvent_NormalAttackEnd()
{
}

HRESULT CEvent_NormalAttackEnd::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_NormalAttackEnd::Execute()
{
	m_pMovement->Reset_Velocity();

	m_pPlayer->Set_AutoLock(false);

	return S_OK;
}

CEvent_NormalAttackEnd* CEvent_NormalAttackEnd::Create(CGameObject* _pGameObject)
{
	CEvent_NormalAttackEnd* pInstance = new CEvent_NormalAttackEnd;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_NormalAttackEnd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_NormalAttackEnd::Free()
{
	super::Free();
}