#include "pch.h"
#include "Splitter_Event_CineCameraShake.h"

#include "GameInstance.h"
#include "CineCamera.h"

CSplitter_Event_CineCameraShake::CSplitter_Event_CineCameraShake()
{
}

HRESULT CSplitter_Event_CineCameraShake::Init(CGameObject* _pGameObject)
{
	m_pCineCamera = static_cast<CCineCamera*>(_pGameObject->Find_PartObject({ L"Part_CineCamera" }));

	return S_OK;
}

HRESULT CSplitter_Event_CineCameraShake::Execute()
{
	m_pCineCamera->RotationShake(0.04f, 5.f, 0.3f);

	return S_OK;
}

CSplitter_Event_CineCameraShake* CSplitter_Event_CineCameraShake::Create(CGameObject* _pGameObject)
{
	CSplitter_Event_CineCameraShake* pInstance = new CSplitter_Event_CineCameraShake();

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CSplitter_Event_CineCameraShake");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_Event_CineCameraShake::Free()
{
	super::Free();
}