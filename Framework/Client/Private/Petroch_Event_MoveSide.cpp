#include "pch.h"
#include "Petroch_Event_MoveSide.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"


CPetroch_Event_MoveSide::CPetroch_Event_MoveSide(_float _fSpeed, _bool _bIsSideMove)
	: m_fSpeed(_fSpeed),
	m_bIsSideMove(_bIsSideMove)
{
}


HRESULT CPetroch_Event_MoveSide::Execute()
{	
	static_cast<CBoss_Petroch*>(m_pGameObject)->Set_IsSideMove(m_bIsSideMove, m_fSpeed);
	return S_OK;
}

CPetroch_Event_MoveSide* CPetroch_Event_MoveSide::Create(CGameObject* _pGameObject, _bool _bIsSideMove, _float _fSpeed)
{
	CPetroch_Event_MoveSide* pInstance = new CPetroch_Event_MoveSide(_fSpeed, _bIsSideMove);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CPetroch_Event_MoveSide");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPetroch_Event_MoveSide::Free()
{
	__super::Free();
}