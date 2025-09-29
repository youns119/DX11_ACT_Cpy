#include "pch.h"
#include "Cuttlefish_Event_Move.h"
#include "GameInstance.h"
#include "Enemy_Cuttlefish.h"


CCuttlefish_Event_Move::CCuttlefish_Event_Move(_float _fSpeed, _bool _bIsChase)
	: m_fSpeed(_fSpeed),
	m_bIsChase(_bIsChase)
{
}

HRESULT CCuttlefish_Event_Move::Execute()
{
	static_cast<CEnemy_Cuttlefish*>(m_pGameObject)->Set_IsChase(m_bIsChase, m_fSpeed);
	return S_OK;
}

CCuttlefish_Event_Move* CCuttlefish_Event_Move::Create(CGameObject* _pGameObject, _bool _bIsChase, _float _fSpeed)
{
	CCuttlefish_Event_Move* pInstance = new CCuttlefish_Event_Move(_fSpeed, _bIsChase);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_Event_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCuttlefish_Event_Move::Free()
{
	__super::Free();
}