#include "pch.h"
#include "Event_HandColliderOff.h"

#include "Player.h"

CEvent_HandColliderOff::CEvent_HandColliderOff()
{
}

HRESULT CEvent_HandColliderOff::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_HandColliderOff::Execute()
{
	m_pPlayer->Player_HandCollider(false);

	return S_OK;
}

CEvent_HandColliderOff* CEvent_HandColliderOff::Create(CGameObject* _pGameObject)
{
	CEvent_HandColliderOff* pInstance = new CEvent_HandColliderOff;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_HandColliderOff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_HandColliderOff::Free()
{
	super::Free();
}