#include "pch.h"
#include "Event_MantisPunch_Dettach.h"

#include "Skill_Topoda.h"

CEvent_MantisPunch_Dettach::CEvent_MantisPunch_Dettach()
{
}

HRESULT CEvent_MantisPunch_Dettach::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTopoda = static_cast<CSkill_Topoda*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_MantisPunch_Dettach::Execute()
{
	m_pTopoda->Dettach();

	return S_OK;
}

CEvent_MantisPunch_Dettach* CEvent_MantisPunch_Dettach::Create(CGameObject* _pGameObject)
{
	CEvent_MantisPunch_Dettach* pInstance = new CEvent_MantisPunch_Dettach;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_MantisPunch_Dettach");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_MantisPunch_Dettach::Free()
{
	super::Free();
}