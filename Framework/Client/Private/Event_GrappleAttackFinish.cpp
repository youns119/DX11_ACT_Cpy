#include "pch.h"
#include "Event_GrappleAttackFinish.h"

#include "GameInstance.h"

#include "Player.h"
#include "CineCamera_Player.h"

CEvent_GrappleAttackFinish::CEvent_GrappleAttackFinish()
{
}

HRESULT CEvent_GrappleAttackFinish::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_GrappleAttackFinish::Execute()
{
	if (!m_pPlayer->Get_PlayerCineCamera()->IsCurrCam())
		m_pPlayer->Get_PlayerCineCamera()->Reset();
	else m_pPlayer->Get_PlayerCineCamera()->Stop_Cinematic();

	return S_OK;
}

CEvent_GrappleAttackFinish* CEvent_GrappleAttackFinish::Create(CGameObject* _pGameObject)
{
	CEvent_GrappleAttackFinish* pInstance = new CEvent_GrappleAttackFinish;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_GrappleAttackFinish");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_GrappleAttackFinish::Free()
{
	super::Free();
}