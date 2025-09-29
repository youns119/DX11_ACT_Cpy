#include "pch.h"
#include "Inkerton_Event_MoveSide.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"


CInkerton_Event_MoveSide::CInkerton_Event_MoveSide(_float _fSpeed, _bool _bIsSideMove)
	: m_fSpeed(_fSpeed),
	m_bIsSideMove(_bIsSideMove)
{
}


HRESULT CInkerton_Event_MoveSide::Execute()
{	
	static_cast<CBoss_Inkerton*>(m_pGameObject)->Set_IsSideMove(m_bIsSideMove, m_fSpeed);
	return S_OK;
}

CInkerton_Event_MoveSide* CInkerton_Event_MoveSide::Create(CGameObject* _pGameObject, _bool _bIsSideMove, _float _fSpeed)
{
	CInkerton_Event_MoveSide* pInstance = new CInkerton_Event_MoveSide(_fSpeed, _bIsSideMove);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CInkerton_Event_MoveSide");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Event_MoveSide::Free()
{
	__super::Free();
}
