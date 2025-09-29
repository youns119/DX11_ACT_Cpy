#include "pch.h"
#include "Cuttlefish_Event_ExecutionDamage.h"

#include "GameInstance.h"

#include "Cuttlefish_GrabTentacle.h"
#include "Player.h"
#include "Shell_Part.h"

CCuttlefish_Event_ExecutionDamage::CCuttlefish_Event_ExecutionDamage(_float _fDamage, _float _fShellDamage)
	:m_fDamage(_fDamage)
	,m_fShellDamage(_fShellDamage)
{
}

HRESULT CCuttlefish_Event_ExecutionDamage::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		E_FAIL;

	m_pGrabTentacle = static_cast<CCuttlefish_GrabTentacle*>(_pGameObject->Find_PartObject({ L"Part_Grab" }));	
	return S_OK;
}

HRESULT CCuttlefish_Event_ExecutionDamage::Execute()
{	
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGrabTentacle->Get_GrabObject());
	CShell_Part* pShell = nullptr;
	if (pPlayer) {
		pPlayer->Instant_Damage(m_fDamage);		
		pShell = static_cast<CShell_Part*>(pPlayer->Get_PlayerShell());		
	}		
	if (pShell)
		pPlayer->Shell_Damage(m_fDamage);

	return S_OK;
}

CCuttlefish_Event_ExecutionDamage* CCuttlefish_Event_ExecutionDamage::Create(CGameObject* _pGameObject, _float _fDamage, _float _fShellDamage)
{
	CCuttlefish_Event_ExecutionDamage* pInstance = new CCuttlefish_Event_ExecutionDamage(_fDamage, _fShellDamage);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_Event_ExecutionDamage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCuttlefish_Event_ExecutionDamage::Free()
{
	super::Free();
}