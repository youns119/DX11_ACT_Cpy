#include "pch.h"
#include "Roland_Event_Move.h"
#include "GameInstance.h"
#include "Boss_Roland.h"


CRoland_Event_Move::CRoland_Event_Move(_float _fSpeed, _bool _bIsChase)
	: m_fSpeed(_fSpeed),
	m_bIsChase(_bIsChase)
{
}


HRESULT CRoland_Event_Move::Execute()
{
	static_cast<CBoss_Roland*>(m_pGameObject)->Set_IsChase(m_bIsChase, m_fSpeed);	
	return S_OK;
}

CRoland_Event_Move* CRoland_Event_Move::Create(CGameObject* _pGameObject, _bool _bIsChase, _float _fSpeed)
{
	CRoland_Event_Move* pInstance = new CRoland_Event_Move(_fSpeed, _bIsChase);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CRoland_Event_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Event_Move::Free()
{
	__super::Free();
}
