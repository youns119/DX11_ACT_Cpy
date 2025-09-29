#include "pch.h"
#include "Event_ParryAttackFinish.h"

#include "GameInstance.h"

#include "Player.h"
#include "CineCamera_Player.h"

CEvent_ParryAttackFinish::CEvent_ParryAttackFinish()
{
}

HRESULT CEvent_ParryAttackFinish::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_ParryAttackFinish::Execute()
{
	if (!m_pPlayer->Get_PlayerCineCamera()->IsCurrCam())
		m_pPlayer->Get_PlayerCineCamera()->Reset();
	else m_pPlayer->Get_PlayerCineCamera()->Stop_Cinematic();

	return S_OK;
}

CEvent_ParryAttackFinish* CEvent_ParryAttackFinish::Create(CGameObject* _pGameObject)
{
	CEvent_ParryAttackFinish* pInstance = new CEvent_ParryAttackFinish;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_ParryAttackFinish");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_ParryAttackFinish::Free()
{
	super::Free();
}