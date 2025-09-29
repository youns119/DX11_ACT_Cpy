#include "pch.h"
#include "Enemy_Event_Collider.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"

//Engine
#include "Collider.h"


CEnemy_Event_Collider::CEnemy_Event_Collider(CCollider* _pCollider, _bool _bIsActive)
	: m_bIsActive(_bIsActive),
	m_pCollider(_pCollider)
{
}


HRESULT CEnemy_Event_Collider::Execute()
{
	m_pCollider->Set_Active(m_bIsActive);
	return S_OK;
}

CEnemy_Event_Collider* CEnemy_Event_Collider::Create(CGameObject* _pGameObject, CCollider* _pCollider, _bool _bIsActive)
{
	CEnemy_Event_Collider* pInstance = new CEnemy_Event_Collider(_pCollider, _bIsActive);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEnemy_Event_Collider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Event_Collider::Free()
{
	__super::Free();
}
