#include "pch.h"
#include "Event_SwipeAttackInit.h"

#include "Player.h"

CEvent_SwipeAttackInit::CEvent_SwipeAttackInit()
{

}

HRESULT CEvent_SwipeAttackInit::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayerStat = static_cast<CPlayer*>(_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_SwipeAttackInit::Execute()
{
	m_pPlayerStat->fAnimSpeed = 0.4f;

	return S_OK;
}

CEvent_SwipeAttackInit* CEvent_SwipeAttackInit::Create(CGameObject* _pGameObject)
{
	CEvent_SwipeAttackInit* pInstance = new CEvent_SwipeAttackInit;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_SwipeAttackInit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_SwipeAttackInit::Free()
{
	super::Free();
}