#include "pch.h"
#include "Sardine_Event_Move.h"
#include "GameInstance.h"
#include "Enemy_Sardine.h"


CSardine_Event_Move::CSardine_Event_Move(_float _fSpeed, _bool _bIsChase)
	: m_fSpeed(_fSpeed),
	m_bIsChase(_bIsChase)
{
}


HRESULT CSardine_Event_Move::Execute()
{
	static_cast<CEnemy_Sardine*>(m_pGameObject)->Set_IsChase(m_bIsChase, m_fSpeed);
	return S_OK;
}

CSardine_Event_Move* CSardine_Event_Move::Create(CGameObject* _pGameObject, _bool _bIsChase, _float _fSpeed)
{
	CSardine_Event_Move* pInstance = new CSardine_Event_Move(_fSpeed, _bIsChase);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CSardine_Event_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSardine_Event_Move::Free()
{
	__super::Free();
}
