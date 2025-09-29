#include "pch.h"
#include "Event_GrappleAttack_Impact.h"

#include "GameInstance.h"
#include "Player.h"
#include "CineCamera_Player.h"

CEvent_GrappleAttack_Impact::CEvent_GrappleAttack_Impact()
{
}

HRESULT CEvent_GrappleAttack_Impact::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_GrappleAttack_Impact::Execute()
{
	CGameInstance::GetInstance()->Call_Lag(0.1f, 0.8f);

	m_pPlayer->Get_PlayerCineCamera()->RotationShake(0.02f, 25.f, 0.7f);

	m_pPlayer->Play_PlayerSound(L"Grapple_Attack");
	m_pPlayer->Play_PlayerSound(L"NormalAttack_Bubble");
	m_pPlayer->Play_PlayerSound(L"Hammer_ChargeAttack_Voice2");

	return S_OK;
}

CEvent_GrappleAttack_Impact* CEvent_GrappleAttack_Impact::Create(CGameObject* _pGameObject)
{
	CEvent_GrappleAttack_Impact* pInstance = new CEvent_GrappleAttack_Impact;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_GrappleAttack_Impact");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_GrappleAttack_Impact::Free()
{
	super::Free();
}