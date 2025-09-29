#include "pch.h"
#include "Bowman_Event_Move.h"
#include "GameInstance.h"
#include "Enemy_Bowman.h"


CBowman_Event_Move::CBowman_Event_Move(_float _fSpeed, _bool _bIsChase)
	: m_fSpeed(_fSpeed),
	m_bIsChase(_bIsChase)
{
}


HRESULT CBowman_Event_Move::Execute()
{
	static_cast<CEnemy_Bowman*>(m_pGameObject)->Set_IsChase(m_bIsChase, m_fSpeed);
	return S_OK;
}

CBowman_Event_Move* CBowman_Event_Move::Create(CGameObject* _pGameObject, _bool _bIsChase, _float _fSpeed)
{
	CBowman_Event_Move* pInstance = new CBowman_Event_Move(_fSpeed, _bIsChase);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CBowman_Event_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBowman_Event_Move::Free()
{
	__super::Free();
}
