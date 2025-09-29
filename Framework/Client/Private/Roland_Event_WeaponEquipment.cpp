#include "pch.h"
#include "Roland_Event_WeaponEquipment.h"
#include "GameInstance.h"
#include "Boss_Roland.h"

CRoland_Event_WeaponEquipment::CRoland_Event_WeaponEquipment(_bool _bIsShowing):
	m_bIsShowing(_bIsShowing)
{
}


HRESULT CRoland_Event_WeaponEquipment::Execute()
{			
	static_cast<CGameObject*>(m_pGameObject->Find_PartObject({ L"Part_Wrench_1_R" }))->Set_IsActive(m_bIsShowing);
	static_cast<CGameObject*>(m_pGameObject->Find_PartObject({ L"Part_Wrench_2_R" }))->Set_IsActive(m_bIsShowing);
	static_cast<CGameObject*>(m_pGameObject->Find_PartObject({ L"Part_Wrench_0_L" }))->Set_IsActive(m_bIsShowing);
	static_cast<CGameObject*>(m_pGameObject->Find_PartObject({ L"Part_Wrench_1_L" }))->Set_IsActive(m_bIsShowing);
	static_cast<CGameObject*>(m_pGameObject->Find_PartObject({ L"Part_Wrench_2_L" }))->Set_IsActive(m_bIsShowing);	
	return S_OK;
}

CRoland_Event_WeaponEquipment* CRoland_Event_WeaponEquipment::Create(CGameObject* _pGameObject, _bool _bIsShowing)
{
	CRoland_Event_WeaponEquipment* pInstance = new CRoland_Event_WeaponEquipment(_bIsShowing);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CRoland_Event_WeaponEquipment");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Event_WeaponEquipment::Free()
{
	__super::Free();
}
