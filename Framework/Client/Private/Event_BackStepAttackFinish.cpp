#include "pch.h"
#include "Event_BackStepAttackFinish.h"

#include "GameInstance.h"

#include "Player.h"
#include "CineCamera_Player.h"

CEvent_BackStepAttackFinish::CEvent_BackStepAttackFinish()
{
}

HRESULT CEvent_BackStepAttackFinish::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_BackStepAttackFinish::Execute()
{
	if (!m_pPlayer->Get_PlayerCineCamera()->IsCurrCam())
		m_pPlayer->Get_PlayerCineCamera()->Reset();
	else m_pPlayer->Get_PlayerCineCamera()->Stop_Cinematic();

	return S_OK;
}

CEvent_BackStepAttackFinish* CEvent_BackStepAttackFinish::Create(CGameObject* _pGameObject)
{
	CEvent_BackStepAttackFinish* pInstance = new CEvent_BackStepAttackFinish;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_BackStepAttackFinish");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_BackStepAttackFinish::Free()
{
	super::Free();
}