#include "pch.h"
#include "Event_PlungeAttackStart.h"

#include "GameInstance.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"
#include "Transform.h"
#include "Movement.h"

CEvent_PlungeAttackStart::CEvent_PlungeAttackStart()
{
}

HRESULT CEvent_PlungeAttackStart::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_PlungeAttackStart::Execute()
{
	m_pMovement->Add_Force(XMVectorSet(0.f, 1.f, 0.f, 0.f), 15.f);

	m_pPlayer->Set_AutoLock(true);

	m_pPlayer->Play_PlayerSound(L"PlungeAttack_Effect");
	m_pPlayer->Play_PlayerSound(L"PlungeAttack_Bubble");

	return S_OK;
}

CEvent_PlungeAttackStart* CEvent_PlungeAttackStart::Create(CGameObject* _pGameObject)
{
	CEvent_PlungeAttackStart* pInstance = new CEvent_PlungeAttackStart;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_PlungeAttackStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_PlungeAttackStart::Free()
{
	super::Free();
}