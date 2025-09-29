#include "pch.h"
#include "Inkerton_Event_Camera.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"


CInkerton_Event_Camera::CInkerton_Event_Camera()
{
}


HRESULT CInkerton_Event_Camera::Execute()
{
	return S_OK;
}

CInkerton_Event_Camera* CInkerton_Event_Camera::Create(CGameObject* _pGameObject)
{
	CInkerton_Event_Camera* pInstance = new CInkerton_Event_Camera();

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CInkerton_Event_Camera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Event_Camera::Free()
{
	__super::Free();
}
