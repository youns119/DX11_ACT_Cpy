#include "pch.h"
#include "Splitter_Event_ExecutionDamage.h"

#include "GameInstance.h"
#include "UI_Manager.h"

#include "Splitter_LeftClaw.h"
#include "Player.h"
#include "Shell_Part.h"

CSplitter_Event_ExecutionDamage::CSplitter_Event_ExecutionDamage(_float _Damage)
	:m_fDamage(_Damage)
{
}

HRESULT CSplitter_Event_ExecutionDamage::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		E_FAIL;

	m_pLeftClaw = static_cast<CSplitter_LeftClaw*>(_pGameObject->Find_PartObject({ L"Part_LeftClaw" }));	
	return S_OK;
}

HRESULT CSplitter_Event_ExecutionDamage::Execute()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pLeftClaw->Get_GrabObject());
	CShell_Part* pShell = nullptr;
	if (pPlayer) {		
		pShell = static_cast<CShell_Part*>(pPlayer->Get_PlayerShell());
		if (pShell)			
			pPlayer->Shell_Damage(999.f);
		else
		{
			pPlayer->Damaged(999.f, IDamaged::DAMAGE::DAMAGE_HEAVY, m_pGameObject);
			CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
			pUIManager->Set_FadeSpeed(20.f);
			pUIManager->FadeOut();
		}

	}
	
	return S_OK;
}

CSplitter_Event_ExecutionDamage* CSplitter_Event_ExecutionDamage::Create(CGameObject* _pGameObject, _float _Damage)
{
	CSplitter_Event_ExecutionDamage* pInstance = new CSplitter_Event_ExecutionDamage(_Damage);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CSplitter_Event_ExecutionDamage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_Event_ExecutionDamage::Free()
{
	super::Free();
}