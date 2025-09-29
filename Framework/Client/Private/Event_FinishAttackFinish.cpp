#include "pch.h"
#include "Event_FinishAttackFinish.h"

#include "GameInstance.h"

#include "Player.h"
#include "CineCamera_Player.h"

CEvent_FinishAttackFinish::CEvent_FinishAttackFinish()
{
}

HRESULT CEvent_FinishAttackFinish::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_FinishAttackFinish::Execute()
{
	if (!m_pPlayer->Get_PlayerCineCamera()->IsCurrCam())
		m_pPlayer->Get_PlayerCineCamera()->Reset();
	else m_pPlayer->Get_PlayerCineCamera()->Stop_Cinematic();

	return S_OK;
}

CEvent_FinishAttackFinish* CEvent_FinishAttackFinish::Create(CGameObject* _pGameObject)
{
	CEvent_FinishAttackFinish* pInstance = new CEvent_FinishAttackFinish;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_FinishAttackFinish");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_FinishAttackFinish::Free()
{
	super::Free();
}