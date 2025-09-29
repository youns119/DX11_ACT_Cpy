#include "pch.h"
#include "Event_MantisPunchFinish.h"

#include "GameInstance.h"

#include "Player.h"
#include "CineCamera_Player.h"

CEvent_MantisPunchFinish::CEvent_MantisPunchFinish()
{
}

HRESULT CEvent_MantisPunchFinish::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_MantisPunchFinish::Execute()
{
	m_pPlayer->Get_PlayerCineCamera()->Stop_Shake();

	if (!m_pPlayer->Get_PlayerCineCamera()->IsCurrCam())
		m_pPlayer->Get_PlayerCineCamera()->Reset();
	else m_pPlayer->Get_PlayerCineCamera()->Stop_Cinematic();

	return S_OK;
}

CEvent_MantisPunchFinish* CEvent_MantisPunchFinish::Create(CGameObject* _pGameObject)
{
	CEvent_MantisPunchFinish* pInstance = new CEvent_MantisPunchFinish;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_MantisPunchFinish");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_MantisPunchFinish::Free()
{
	super::Free();
}