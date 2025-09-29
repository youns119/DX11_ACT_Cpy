#include "pch.h"
#include "Inkerton_Event_CineCameraOn.h"

#include "GameInstance.h"
#include "CineCamera.h"

CInkerton_Event_CineCameraOn::CInkerton_Event_CineCameraOn()
{
}


HRESULT CInkerton_Event_CineCameraOn::Init(CGameObject* _pGameObject)
{
	m_pCineCamera = static_cast<CCineCamera*>(_pGameObject->Find_PartObject({L"Part_CineCamera"}));

	return S_OK;
}

HRESULT CInkerton_Event_CineCameraOn::Execute()
{
	m_pCineCamera->Play(L"Execute", L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.f);
	CGameInstance::GetInstance()->Camera_Transition(L"CineCamera_Inkerton", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);

	return S_OK;
}

CInkerton_Event_CineCameraOn* CInkerton_Event_CineCameraOn::Create(CGameObject* _pGameObject)
{
	CInkerton_Event_CineCameraOn* pInstance = new CInkerton_Event_CineCameraOn();

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CInkerton_Event_CineCameraOn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Event_CineCameraOn::Free()
{
	super::Free();
}