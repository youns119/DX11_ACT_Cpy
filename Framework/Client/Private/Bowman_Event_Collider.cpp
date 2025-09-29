#include "pch.h"
#include "Bowman_Event_Collider.h"
#include "GameInstance.h"
#include "Enemy_Bowman.h"


CBowman_Event_Collider::CBowman_Event_Collider(const _wstring& _strPartObject, const _wstring& _strComName, _bool _bIsActive)
	: m_strPartObject(_strPartObject),
	m_strComName(_strComName),
	m_bIsActive(_bIsActive)
{
}


HRESULT CBowman_Event_Collider::Execute()
{
	static_cast<CCollider*>(static_cast<CEnemy_Bowman*>(m_pGameObject)->Find_PartObject({ m_strPartObject })->Find_Component(m_strComName))->Set_Active(m_bIsActive);
	return S_OK;
}

CBowman_Event_Collider* CBowman_Event_Collider::Create(CGameObject* _pGameObject, const _wstring& _strPartObject, const _wstring& _strComName, _bool _bIsActive)
{
	CBowman_Event_Collider* pInstance = new CBowman_Event_Collider(_strPartObject, _strComName, _bIsActive);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CBowman_Event_Collider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBowman_Event_Collider::Free()
{
	__super::Free();
}
