#include "pch.h"
#include "Event_HeavyDamage.h"

#include "Player.h"

CEvent_HeavyDamage::CEvent_HeavyDamage()
{
}

HRESULT CEvent_HeavyDamage::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayerStat = static_cast<CPlayer*>(_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_HeavyDamage::Execute()
{
	m_pPlayerStat->eDamageType = IDamaged::DAMAGE::DAMAGE_HEAVY;

	return S_OK;
}

CEvent_HeavyDamage* CEvent_HeavyDamage::Create(CGameObject* _pGameObject)
{
	CEvent_HeavyDamage* pInstance = new CEvent_HeavyDamage;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_HeavyDamage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_HeavyDamage::Free()
{
	super::Free();
}