#include "pch.h"
#include "Event_ActiveSkill.h"

#include "Player.h"

CEvent_ActiveSkill::CEvent_ActiveSkill()
{
}

HRESULT CEvent_ActiveSkill::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_ActiveSkill::Execute()
{
	m_pPlayer->Use_ActiveSkill();

	return S_OK;
}

CEvent_ActiveSkill* CEvent_ActiveSkill::Create(CGameObject* _pGameObject)
{
	CEvent_ActiveSkill* pInstance = new CEvent_ActiveSkill;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_ActiveSkill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_ActiveSkill::Free()
{
	super::Free();
}