#include "pch.h"
#include "Cuttlefish_Event_MoveSide.h"
#include "GameInstance.h"
#include "Enemy_Cuttlefish.h"

CCuttlefish_Event_MoveSide::CCuttlefish_Event_MoveSide(_float _fSpeed, _bool _bIsSideMove)
	: m_fSpeed(_fSpeed),
	m_bIsSideMove(_bIsSideMove)
{
}

HRESULT CCuttlefish_Event_MoveSide::Execute()
{	
	static_cast<CEnemy_Cuttlefish*>(m_pGameObject)->Set_IsSideMove(m_bIsSideMove, m_fSpeed);
	return S_OK;
}

CCuttlefish_Event_MoveSide* CCuttlefish_Event_MoveSide::Create(CGameObject* _pGameObject, _bool _bIsSideMove, _float _fSpeed)
{
	CCuttlefish_Event_MoveSide* pInstance = new CCuttlefish_Event_MoveSide(_fSpeed, _bIsSideMove);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_Event_MoveSide");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCuttlefish_Event_MoveSide::Free()
{
	__super::Free();
}