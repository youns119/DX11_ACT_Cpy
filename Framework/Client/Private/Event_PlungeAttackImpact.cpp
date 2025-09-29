#include "pch.h"
#include "Event_PlungeAttackImpact.h"

#include "GameInstance.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"

CEvent_PlungeAttackImpact::CEvent_PlungeAttackImpact()
{
}

HRESULT CEvent_PlungeAttackImpact::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_PlungeAttackImpact::Execute()
{
	m_pPlayer->Player_CameraShake_Position(0.05f, 20.f, 0.5f);
	m_pPlayer->Player_CameraShake_Rotation(0.05f, 20.f, 0.5f);

	m_pPlayer->Play_PlayerSound(L"PlungeAttack_Impact");
	m_pPlayer->Play_PlayerSound(L"PlungeAttack_Bubble2");
	m_pPlayer->Play_PlayerSound(L"PlungeAttack_Voice");

	return S_OK;
}

CEvent_PlungeAttackImpact* CEvent_PlungeAttackImpact::Create(CGameObject* _pGameObject)
{
	CEvent_PlungeAttackImpact* pInstance = new CEvent_PlungeAttackImpact;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_PlungeAttackImpact");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_PlungeAttackImpact::Free()
{
	super::Free();
}
