#include "pch.h"
#include "Cuttlefish_Event_MissileR.h"
#include "GameInstance.h"
#include "Enemy_Cuttlefish.h"

CCuttlefish_Event_MissileR::CCuttlefish_Event_MissileR()
{
}

HRESULT CCuttlefish_Event_MissileR::Execute()
{	
	static_cast<CEnemy_Cuttlefish*>(m_pGameObject)->Throw_Missile_R();
	return S_OK;
}

CCuttlefish_Event_MissileR* CCuttlefish_Event_MissileR::Create(CGameObject* _pGameObject)
{
	CCuttlefish_Event_MissileR* pInstance = new CCuttlefish_Event_MissileR();

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_Event_MissileR");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCuttlefish_Event_MissileR::Free()
{
	__super::Free();
}