#include "pch.h"
#include "Event_NormalDamage.h"

#include "Player.h"

CEvent_NormalDamage::CEvent_NormalDamage()
{
}

HRESULT CEvent_NormalDamage::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayerStat = static_cast<CPlayer*>(_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_NormalDamage::Execute()
{
	m_pPlayerStat->eDamageType = IDamaged::DAMAGE::DAMAGE_NORMAL;

	return S_OK;
}

CEvent_NormalDamage* CEvent_NormalDamage::Create(CGameObject* _pGameObject)
{
	CEvent_NormalDamage* pInstance = new CEvent_NormalDamage;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_NormalDamage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_NormalDamage::Free()
{
	super::Free();
}