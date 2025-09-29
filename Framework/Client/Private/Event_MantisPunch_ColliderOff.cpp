#include "pch.h"
#include "Event_MantisPunch_ColliderOff.h"

#include "Skill_Topoda.h"

CEvent_MantisPunch_ColliderOff::CEvent_MantisPunch_ColliderOff()
{
}

HRESULT CEvent_MantisPunch_ColliderOff::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTopoda = static_cast<CSkill_Topoda*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_MantisPunch_ColliderOff::Execute()
{
	m_pTopoda->Toggle_Collider(false);

	return S_OK;
}

CEvent_MantisPunch_ColliderOff* CEvent_MantisPunch_ColliderOff::Create(CGameObject* _pGameObject)
{
	CEvent_MantisPunch_ColliderOff* pInstance = new CEvent_MantisPunch_ColliderOff;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_MantisPunch_ColliderOff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_MantisPunch_ColliderOff::Free()
{
	super::Free();
}