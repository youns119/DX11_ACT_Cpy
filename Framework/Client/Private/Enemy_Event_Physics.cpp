#include "pch.h"
#include "Enemy_Event_Physics.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"

//Engine
#include "Physics_Controller.h"


CEnemy_Event_Physics::CEnemy_Event_Physics(CPhysics_Controller* _pPhysicsController, _bool _bIsActive)
	: m_bIsActive(_bIsActive),
	m_pPhysicsController(_pPhysicsController)
{
}


HRESULT CEnemy_Event_Physics::Execute()
{
	m_pPhysicsController->Set_UserData({ m_bIsActive });
	return S_OK;
}

CEnemy_Event_Physics* CEnemy_Event_Physics::Create(CGameObject* _pGameObject, CPhysics_Controller* _pPhysicsController, _bool _bIsActive)
{
	CEnemy_Event_Physics* pInstance = new CEnemy_Event_Physics(_pPhysicsController, _bIsActive);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEnemy_Event_Physics");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Event_Physics::Free()
{
	__super::Free();
}
