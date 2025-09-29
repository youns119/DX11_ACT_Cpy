#include "pch.h"
#include "Bowman_Event_Reload.h"
#include "GameInstance.h"
#include "Enemy_Bowman.h"
#include "Bowman_Arrow.h"


CBowman_Event_Reload::CBowman_Event_Reload()
{
}


HRESULT CBowman_Event_Reload::Execute()
{	
	static_cast<CBowman_Arrow*>(static_cast<CEnemy_Bowman*>(m_pGameObject)->Find_PartObject({ TEXT("Part_Arrow") }))->Reload_Arrow();
	return S_OK;
}

CBowman_Event_Reload* CBowman_Event_Reload::Create(CGameObject* _pGameObject)
{
	CBowman_Event_Reload* pInstance = new CBowman_Event_Reload();

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CBowman_Event_Reload");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBowman_Event_Reload::Free()
{
	__super::Free();
}
