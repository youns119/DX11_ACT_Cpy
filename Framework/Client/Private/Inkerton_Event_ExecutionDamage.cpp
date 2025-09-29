#include "pch.h"
#include "Inkerton_Event_ExecutionDamage.h"

#include "GameInstance.h"

#include "Inkerton_Nutcracker.h"
#include "Inkerton_StretchedNutcracker.h"
#include "Player.h"
#include "Shell_Part.h"

CInkerton_Event_ExecutionDamage::CInkerton_Event_ExecutionDamage(_float _fDamage, _float _fShellDamage)
	:m_fDamage(_fDamage)
	,m_fShellDamage(_fShellDamage)
{
}

HRESULT CInkerton_Event_ExecutionDamage::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		E_FAIL;

	m_pInkerton_Nutcracker = static_cast<CInkerton_Nutcracker*>(_pGameObject->Find_PartObject({ L"Part_Nutcracker" }));
	m_pInkerton_StretchedNutcracker = static_cast<CInkerton_StretchedNutcracker*>(_pGameObject->Find_PartObject({ L"Part_StretchedNutcracker" }));
	
	return S_OK;
}

HRESULT CInkerton_Event_ExecutionDamage::Execute()
{	
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pInkerton_Nutcracker->Get_GrabObject());
	CShell_Part* pShell = nullptr;
	if (pPlayer) {		
		pPlayer->Instant_Damage(m_fDamage);
		pShell = static_cast<CShell_Part*>(pPlayer->Get_PlayerShell());		
	}		
	if (pShell)		
		pPlayer->Shell_Damage(20.f);
	 
	pPlayer = static_cast<CPlayer*>(m_pInkerton_StretchedNutcracker->Get_GrabObject());
	pShell = nullptr;
	if (pPlayer) {		
		pPlayer->Instant_Damage(m_fDamage);
		pShell = static_cast<CShell_Part*>(pPlayer->Get_PlayerShell());
	}
	if (pShell)		
		pPlayer->Shell_Damage(20.f);

	CGameInstance::GetInstance()->Call_Lag(0.2f, 0.2f);

	return S_OK;
}

CInkerton_Event_ExecutionDamage* CInkerton_Event_ExecutionDamage::Create(CGameObject* _pGameObject, _float _fDamage, _float _fShellDamage)
{
	CInkerton_Event_ExecutionDamage* pInstance = new CInkerton_Event_ExecutionDamage(_fDamage, _fShellDamage);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CInkerton_Event_ExecutionDamage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Event_ExecutionDamage::Free()
{
	super::Free();
}