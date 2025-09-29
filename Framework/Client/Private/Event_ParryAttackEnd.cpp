#include "pch.h"
#include "Event_ParryAttackEnd.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "CineCamera_Player.h"
#include "Movement.h"

CEvent_ParryAttackEnd::CEvent_ParryAttackEnd()
{
}

HRESULT CEvent_ParryAttackEnd::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_ParryAttackEnd::Execute()
{
	m_pMovement->Add_Force(XMVectorSet(0.f, 1.f, 0.f, 0.f), 20.f);

	m_pPlayer->Get_PlayerCineCamera()->RotationShake(0.02f, 25.f, 0.7f);

	return S_OK;
}

CEvent_ParryAttackEnd* CEvent_ParryAttackEnd::Create(CGameObject* _pGameObject)
{
	CEvent_ParryAttackEnd* pInstance = new CEvent_ParryAttackEnd;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_ParryAttackEnd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_ParryAttackEnd::Free()
{
	super::Free();
}