#include "pch.h"
#include "BobbitWorm_Event_Camera.h"
#include "GameInstance.h"
#include "Enemy_BobbitWorm.h"
#include "CineCamera.h"

CBobbitWorm_Event_Camera::CBobbitWorm_Event_Camera()
{
}


HRESULT CBobbitWorm_Event_Camera::Init(CGameObject* _pGameObject)
{
	m_pCineCamera = static_cast<CCineCamera*>(_pGameObject->Find_PartObject({ L"Part_CineCamera" }));

	return S_OK;
}

HRESULT CBobbitWorm_Event_Camera::Execute()
{
	return S_OK;
}

CBobbitWorm_Event_Camera* CBobbitWorm_Event_Camera::Create(CGameObject* _pGameObject)
{
	CBobbitWorm_Event_Camera* pInstance = new CBobbitWorm_Event_Camera();

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CBobbitWorm_Event_Camera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBobbitWorm_Event_Camera::Free()
{
	__super::Free();
}