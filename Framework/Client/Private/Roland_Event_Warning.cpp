#include "pch.h"
#include "Roland_Event_Warning.h"
#include "GameInstance.h"
#include "Boss_Roland.h"

CRoland_Event_Warning::CRoland_Event_Warning(_bool _bIsShowing):
	m_bIsShowing(_bIsShowing)
{
}


HRESULT CRoland_Event_Warning::Execute()
{			
	static_cast<CBoss_Roland*>(m_pGameObject)->Set_IsUnblockable(m_bIsShowing);
	return S_OK;
}

CRoland_Event_Warning* CRoland_Event_Warning::Create(CGameObject* _pGameObject, _bool _bIsShowing)
{
	CRoland_Event_Warning* pInstance = new CRoland_Event_Warning(_bIsShowing);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CRoland_Event_Warning");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Event_Warning::Free()
{
	__super::Free();
}
