#include "pch.h"
#include "Petroch_Event_Move.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"


CPetroch_Event_Move::CPetroch_Event_Move(_float _fSpeed, _bool _bIsChase)
	: m_fSpeed(_fSpeed),
	m_bIsChase(_bIsChase)
{
}


HRESULT CPetroch_Event_Move::Execute()
{
	static_cast<CBoss_Petroch*>(m_pGameObject)->Set_IsChase(m_bIsChase, m_fSpeed);	
	return S_OK;
}

CPetroch_Event_Move* CPetroch_Event_Move::Create(CGameObject* _pGameObject, _bool _bIsChase, _float _fSpeed)
{
	CPetroch_Event_Move* pInstance = new CPetroch_Event_Move(_fSpeed, _bIsChase);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CPetroch_Event_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPetroch_Event_Move::Free()
{
	__super::Free();
}
