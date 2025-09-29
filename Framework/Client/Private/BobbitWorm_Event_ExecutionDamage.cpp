#include "pch.h"
#include "BobbitWorm_Event_ExecutionDamage.h"

#include "GameInstance.h"

#include "BobbitWorm_Mouth.h"
#include "Player.h"
#include "Shell_Part.h"
#include "UI_Manager.h"

CBobbitWorm_Event_ExecutionDamage::CBobbitWorm_Event_ExecutionDamage()
{
}

HRESULT CBobbitWorm_Event_ExecutionDamage::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		E_FAIL;

	m_pMouth = static_cast<CBobbitWorm_Mouth*>(_pGameObject->Find_PartObject({ L"Part_Mouth" }));
	
	return S_OK;
}

HRESULT CBobbitWorm_Event_ExecutionDamage::Execute()
{	
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pMouth->Get_GrabObject());	
	if (pPlayer) {
		CShell_Part* pShell = static_cast<CShell_Part*>(pPlayer->Get_PlayerShell());
		if (pShell)
			pPlayer->Shell_Damage(9999.f);
		else
		{
			pPlayer->Instant_Damage(9999.f);

			CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
			pUIManager->Set_FadeSpeed(15.f);
			pUIManager->FadeOut();
		}
	}

	return S_OK;
}

CBobbitWorm_Event_ExecutionDamage* CBobbitWorm_Event_ExecutionDamage::Create(CGameObject* _pGameObject)
{
	CBobbitWorm_Event_ExecutionDamage* pInstance = new CBobbitWorm_Event_ExecutionDamage;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CBobbitWorm_Event_ExecutionDamage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBobbitWorm_Event_ExecutionDamage::Free()
{
	super::Free();
}