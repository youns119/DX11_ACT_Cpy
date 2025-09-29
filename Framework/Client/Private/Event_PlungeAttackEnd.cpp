#include "pch.h"
#include "Event_PlungeAttackEnd.h"

#include "GameInstance.h"

#include "Player.h"
#include "Movement.h"

CEvent_PlungeAttackEnd::CEvent_PlungeAttackEnd()
{
}

HRESULT CEvent_PlungeAttackEnd::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_PlungeAttackEnd::Execute()
{
	m_pMovement->Activate(true);
	m_pMovement->Add_Force(XMVectorSet(0.f, -1.f, 0.f, 0.f), 30.f);

	return S_OK;
}

CEvent_PlungeAttackEnd* CEvent_PlungeAttackEnd::Create(CGameObject* _pGameObject)
{
	CEvent_PlungeAttackEnd* pInstance = new CEvent_PlungeAttackEnd;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_PlungeAttackEnd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_PlungeAttackEnd::Free()
{
	super::Free();
}