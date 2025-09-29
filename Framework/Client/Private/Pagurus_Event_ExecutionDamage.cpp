#include "pch.h"
#include "Pagurus_Event_ExecutionDamage.h"

#include "GameInstance.h"

#include "Pagurus_Fork.h"
#include "Player.h"
#include "Shell_Part.h"

#include "UI_Manager.h"

CPagurus_Event_ExecutionDamage::CPagurus_Event_ExecutionDamage(_float _fDamage, _float _fShellDamage)
	:m_fDamage(_fDamage)
	,m_fShellDamage(_fShellDamage)
{
}

HRESULT CPagurus_Event_ExecutionDamage::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		E_FAIL;
	
	m_pPagurus_Fork = static_cast<CPagurus_Fork*>(_pGameObject->Find_PartObject({ L"Part_Fork" }));
	
	return S_OK;
}

HRESULT CPagurus_Event_ExecutionDamage::Execute()
{	
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pPagurus_Fork->Get_GrabObject());
	CShell_Part* pShell = nullptr;
	if (pPlayer) {				
		pShell = static_cast<CShell_Part*>(pPlayer->Get_PlayerShell());	
		if (pShell)
			pPlayer->Shell_Damage(m_fShellDamage);

		pPlayer->Instant_Damage(m_fDamage);
	}		

	CGameInstance::GetInstance()->Call_Lag(0.2f, 0.2f);
	return S_OK;
}

CPagurus_Event_ExecutionDamage* CPagurus_Event_ExecutionDamage::Create(CGameObject* _pGameObject, _float _fDamage, _float _fShellDamage)
{
	CPagurus_Event_ExecutionDamage* pInstance = new CPagurus_Event_ExecutionDamage(_fDamage, _fShellDamage);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CPagurus_Event_ExecutionDamage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPagurus_Event_ExecutionDamage::Free()
{
	super::Free();
}