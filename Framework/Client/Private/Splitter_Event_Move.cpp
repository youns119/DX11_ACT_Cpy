#include "pch.h"
#include "Splitter_Event_Move.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"


CSplitter_Event_Move::CSplitter_Event_Move(_float _fSpeed, _bool _bIsChase)
	: m_fSpeed(_fSpeed),
	m_bIsChase(_bIsChase)
{
}


HRESULT CSplitter_Event_Move::Execute()
{
	static_cast<CEnemy_Splitter*>(m_pGameObject)->Set_IsChase(m_bIsChase, m_fSpeed);
	return S_OK;
}

CSplitter_Event_Move* CSplitter_Event_Move::Create(CGameObject* _pGameObject, _bool _bIsChase, _float _fSpeed)
{
	CSplitter_Event_Move* pInstance = new CSplitter_Event_Move(_fSpeed, _bIsChase);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CSplitter_Event_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_Event_Move::Free()
{
	__super::Free();
}