#include "pch.h"
#include "Inkerton_Event_Move.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"


CInkerton_Event_Move::CInkerton_Event_Move(_float _fSpeed, _bool _bIsChase)
	: m_fSpeed(_fSpeed),
	m_bIsChase(_bIsChase)
{
}


HRESULT CInkerton_Event_Move::Execute()
{
	static_cast<CBoss_Inkerton*>(m_pGameObject)->Set_IsChase(m_bIsChase, m_fSpeed);	
	return S_OK;
}

CInkerton_Event_Move* CInkerton_Event_Move::Create(CGameObject* _pGameObject, _bool _bIsChase, _float _fSpeed)
{
	CInkerton_Event_Move* pInstance = new CInkerton_Event_Move(_fSpeed, _bIsChase);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CInkerton_Event_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Event_Move::Free()
{
	__super::Free();
}
