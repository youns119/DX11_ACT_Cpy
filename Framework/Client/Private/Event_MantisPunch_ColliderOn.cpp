#include "pch.h"
#include "Event_MantisPunch_ColliderOn.h"

#include "Skill_Topoda.h"

CEvent_MantisPunch_ColliderOn::CEvent_MantisPunch_ColliderOn()
{
}

HRESULT CEvent_MantisPunch_ColliderOn::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTopoda = static_cast<CSkill_Topoda*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_MantisPunch_ColliderOn::Execute()
{
	m_pTopoda->Toggle_Collider(true);

	return S_OK;
}

CEvent_MantisPunch_ColliderOn* CEvent_MantisPunch_ColliderOn::Create(CGameObject* _pGameObject)
{
	CEvent_MantisPunch_ColliderOn* pInstance = new CEvent_MantisPunch_ColliderOn;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_MantisPunch_ColliderOn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_MantisPunch_ColliderOn::Free()
{
	super::Free();
}