#include "pch.h"
#include "Event_GrappleAttack_Launch.h"

#include "GameInstance.h"
#include "Player.h"

CEvent_GrappleAttack_Launch::CEvent_GrappleAttack_Launch()
{
}

HRESULT CEvent_GrappleAttack_Launch::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_GrappleAttack_Launch::Execute()
{
	m_pPlayer->Grapple_Launch();

	return S_OK;
}

CEvent_GrappleAttack_Launch* CEvent_GrappleAttack_Launch::Create(CGameObject* _pGameObject)
{
	CEvent_GrappleAttack_Launch* pInstance = new CEvent_GrappleAttack_Launch;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_GrappleAttack_Launch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_GrappleAttack_Launch::Free()
{
	super::Free();
}