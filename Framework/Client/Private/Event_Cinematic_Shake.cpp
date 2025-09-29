#include "pch.h"
#include "Event_Cinematic_Shake.h"

#include "CineCamera.h"

#include "CinematicCamera.h"

CEvent_Cinematic_Shake::CEvent_Cinematic_Shake()
{
}

HRESULT CEvent_Cinematic_Shake::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pCinematicCamera = static_cast<CCinematicCamera*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_Cinematic_Shake::Execute()
{
	m_pCinematicCamera->Get_CineCamera()->PositionShake(0.1f, 20.f, 0.5);
	m_pCinematicCamera->Get_CineCamera()->RotationShake(0.1f, 20.f, 0.5);

	return S_OK;
}

CEvent_Cinematic_Shake* CEvent_Cinematic_Shake::Create(CGameObject* _pGameObject)
{
	CEvent_Cinematic_Shake* pInstance = new CEvent_Cinematic_Shake;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_Cinematic_Shake");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Cinematic_Shake::Free()
{
	super::Free();
}