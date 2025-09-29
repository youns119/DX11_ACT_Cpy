#include "pch.h"
#include "Splitter_Event_CineCameraOn.h"

#include "GameInstance.h"
#include "CineCamera.h"

CSplitter_Event_CineCameraOn::CSplitter_Event_CineCameraOn()
{
}


HRESULT CSplitter_Event_CineCameraOn::Init(CGameObject* _pGameObject)
{
	m_pCineCamera = static_cast<CCineCamera*>(_pGameObject->Find_PartObject({L"Part_CineCamera"}));

	return S_OK;
}

HRESULT CSplitter_Event_CineCameraOn::Execute()
{
	m_pCineCamera->Play(L"Execute", L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
	CGameInstance::GetInstance()->Camera_Transition(L"CineCamera_Splitter", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);

	return S_OK;
}

CSplitter_Event_CineCameraOn* CSplitter_Event_CineCameraOn::Create(CGameObject* _pGameObject)
{
	CSplitter_Event_CineCameraOn* pInstance = new CSplitter_Event_CineCameraOn();

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CSplitter_Event_CineCameraOn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_Event_CineCameraOn::Free()
{
	super::Free();
}