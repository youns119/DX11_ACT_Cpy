#include "pch.h"
#include "Event_HandColliderOn.h"

#include "Player.h"

CEvent_HandColliderOn::CEvent_HandColliderOn()
{
}

HRESULT CEvent_HandColliderOn::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_HandColliderOn::Execute()
{
	m_pPlayer->Player_HandCollider(true);

	return S_OK;
}

CEvent_HandColliderOn* CEvent_HandColliderOn::Create(CGameObject* _pGameObject)
{
	CEvent_HandColliderOn* pInstance = new CEvent_HandColliderOn;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_HandColliderOn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_HandColliderOn::Free()
{
	super::Free();
}