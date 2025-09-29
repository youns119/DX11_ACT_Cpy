#include "pch.h"
#include "Cuttlefish_Event_MissileL.h"
#include "GameInstance.h"
#include "Enemy_Cuttlefish.h"

CCuttlefish_Event_MissileL::CCuttlefish_Event_MissileL()
{
}

HRESULT CCuttlefish_Event_MissileL::Execute()
{	
	static_cast<CEnemy_Cuttlefish*>(m_pGameObject)->Throw_Missile_L();
	return S_OK;
}

CCuttlefish_Event_MissileL* CCuttlefish_Event_MissileL::Create(CGameObject* _pGameObject)
{
	CCuttlefish_Event_MissileL* pInstance = new CCuttlefish_Event_MissileL();

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_Event_MissileL");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCuttlefish_Event_MissileL::Free()
{
	__super::Free();
}