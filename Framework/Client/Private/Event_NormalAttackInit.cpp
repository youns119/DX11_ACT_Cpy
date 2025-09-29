#include "pch.h"
#include "Event_NormalAttackInit.h"

#include "Player.h"
#include "Movement.h"

CEvent_NormalAttackInit::CEvent_NormalAttackInit()
{
}

HRESULT CEvent_NormalAttackInit::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayerStat = static_cast<CPlayer*>(_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_NormalAttackInit::Execute()
{
	// 애니메이션 속도 일시적으로 증가
	m_pPlayerStat->fAnimSpeed = 2.f;

	return S_OK;
}

CEvent_NormalAttackInit* CEvent_NormalAttackInit::Create(CGameObject* _pGameObject)
{
	CEvent_NormalAttackInit* pInstance = new CEvent_NormalAttackInit;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_NormalAttackInit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_NormalAttackInit::Free()
{
	super::Free();
}