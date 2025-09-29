#include "pch.h"
#include "Sardine_Event_Collider.h"
#include "GameInstance.h"
#include "Enemy_Sardine.h"


CSardine_Event_Collider::CSardine_Event_Collider(const _wstring& _strPartObject, const _wstring& _strComName, _bool _bIsActive)
	: m_strPartObject(_strPartObject),
	m_strComName(_strComName),
	m_bIsActive(_bIsActive)
{
}


HRESULT CSardine_Event_Collider::Execute()
{
	
	static_cast<CCollider*>(static_cast<CEnemy_Sardine*>(m_pGameObject)->Find_PartObject({ m_strPartObject })->Find_Component(m_strComName))->Set_Active(m_bIsActive);
	//Set_BaseAnimSpeed
	//Get_CurrAnimation
	return S_OK;
}

CSardine_Event_Collider* CSardine_Event_Collider::Create(CGameObject* _pGameObject, const _wstring& _strPartObject, const _wstring& _strComName, _bool _bIsActive)
{
	CSardine_Event_Collider* pInstance = new CSardine_Event_Collider(_strPartObject, _strComName, _bIsActive);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CSardine_Event_Collider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSardine_Event_Collider::Free()
{
	__super::Free();
}
