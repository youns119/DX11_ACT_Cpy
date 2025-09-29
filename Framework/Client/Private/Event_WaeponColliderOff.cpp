#include "pch.h"
#include "Event_WeaponColliderOff.h"

#include "Player.h"

CEvent_WeaponColliderOff::CEvent_WeaponColliderOff()
{
}

HRESULT CEvent_WeaponColliderOff::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_WeaponColliderOff::Execute()
{
	m_pPlayer->Player_WeaponCollider(false);

	return S_OK;
}

CEvent_WeaponColliderOff* CEvent_WeaponColliderOff::Create(CGameObject* _pGameObject)
{
	CEvent_WeaponColliderOff* pInstance = new CEvent_WeaponColliderOff;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_WeaponColliderOff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_WeaponColliderOff::Free()
{
	super::Free();
}