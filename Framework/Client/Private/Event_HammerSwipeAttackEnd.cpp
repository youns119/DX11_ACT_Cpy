#include "pch.h"
#include "Event_HammerSwipeAttackEnd.h"

#include "Player.h"
#include "Movement.h"

CEvent_HammerSwipeAttackEnd::CEvent_HammerSwipeAttackEnd()
{
}

HRESULT CEvent_HammerSwipeAttackEnd::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_HammerSwipeAttackEnd::Execute()
{
	m_pMovement->Add_Force(XMVectorSet(0.f, 1.f, 0.f, 0.f), 30.f);

	m_pPlayer->Set_AutoLock(false);

	return S_OK;
}

CEvent_HammerSwipeAttackEnd* CEvent_HammerSwipeAttackEnd::Create(CGameObject* _pGameObject)
{
	CEvent_HammerSwipeAttackEnd* pInstance = new CEvent_HammerSwipeAttackEnd;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_HammerSwipeAttackEnd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_HammerSwipeAttackEnd::Free()
{
	super::Free();
}