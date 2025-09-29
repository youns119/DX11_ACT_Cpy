#include "pch.h"
#include "Splitter_Event_Collider.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"


CSplitter_Event_Collider::CSplitter_Event_Collider(const _wstring& _strPartObject, const _wstring& _strComName, _bool _bIsActive)
	: m_strPartObject(_strPartObject),
	m_strComName(_strComName),
	m_bIsActive(_bIsActive)
{
}


HRESULT CSplitter_Event_Collider::Execute()
{
	static_cast<CCollider*>(static_cast<CEnemy_Splitter*>(m_pGameObject)->Find_PartObject({ m_strPartObject })->Find_Component(m_strComName))->Set_Active(m_bIsActive);
	return S_OK;
}

CSplitter_Event_Collider* CSplitter_Event_Collider::Create(CGameObject* _pGameObject, const _wstring& _strPartObject, const _wstring& _strComName, _bool _bIsActive)
{
	CSplitter_Event_Collider* pInstance = new CSplitter_Event_Collider(_strPartObject, _strComName, _bIsActive);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CSplitter_Event_Collider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_Event_Collider::Free()
{
	__super::Free();
}
