#include "pch.h"
#include "Event_Skill_RollOut.h"

#include "GameInstance.h"
#include "Movement.h"

#include "Player.h"

CEvent_Skill_RollOut::CEvent_Skill_RollOut()
{
}

HRESULT CEvent_Skill_RollOut::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);

	return S_OK;
}

HRESULT CEvent_Skill_RollOut::Execute()
{
	return S_OK;
}

CEvent_Skill_RollOut* CEvent_Skill_RollOut::Create(CGameObject* _pGameObject)
{
	CEvent_Skill_RollOut* pInstance = new CEvent_Skill_RollOut;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_Skill_RollOut");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Skill_RollOut::Free()
{
	super::Free();
}