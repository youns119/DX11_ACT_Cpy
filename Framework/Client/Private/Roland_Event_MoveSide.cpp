#include "pch.h"
#include "Roland_Event_MoveSide.h"
#include "GameInstance.h"
#include "Boss_Roland.h"


CRoland_Event_MoveSide::CRoland_Event_MoveSide(_float _fSpeed, _bool _bIsSideMove)
	: m_fSpeed(_fSpeed),
	m_bIsSideMove(_bIsSideMove)
{
}


HRESULT CRoland_Event_MoveSide::Execute()
{	
	static_cast<CBoss_Roland*>(m_pGameObject)->Set_IsSideMove(m_bIsSideMove, m_fSpeed);
	return S_OK;
}

CRoland_Event_MoveSide* CRoland_Event_MoveSide::Create(CGameObject* _pGameObject, _bool _bIsSideMove, _float _fSpeed)
{
	CRoland_Event_MoveSide* pInstance = new CRoland_Event_MoveSide(_fSpeed, _bIsSideMove);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CRoland_Event_MoveSide");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Event_MoveSide::Free()
{
	__super::Free();
}
