#include "pch.h"
#include "Event_WeaponColliderOn.h"

#include "Player.h"

CEvent_WeaponColliderOn::CEvent_WeaponColliderOn()
{
}

HRESULT CEvent_WeaponColliderOn::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_WeaponColliderOn::Execute()
{
	m_pPlayer->Player_WeaponCollider(true);

	return S_OK;
}

CEvent_WeaponColliderOn* CEvent_WeaponColliderOn::Create(CGameObject* _pGameObject)
{
	CEvent_WeaponColliderOn* pInstance = new CEvent_WeaponColliderOn;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_WeaponColliderOn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_WeaponColliderOn::Free()
{
	super::Free();
}