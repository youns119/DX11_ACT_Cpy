#include "pch.h"
#include "Pagurus_Event_GrabOff.h"

#include "GameInstance.h"

#include "Pagurus_Fork.h"
#include "Player.h"

#include "UI_Manager.h"

CPagurus_Event_GrabOff::CPagurus_Event_GrabOff()
{
}

HRESULT CPagurus_Event_GrabOff::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPagurus_Fork = static_cast<CPagurus_Fork*>(_pGameObject->Find_PartObject({ L"Part_Fork" }));

	return S_OK;
}

HRESULT CPagurus_Event_GrabOff::Execute()
{
	CPlayer* pPlayer = nullptr;
	if(nullptr != m_pPagurus_Fork->Get_GrabObject())
		pPlayer = static_cast<CPlayer*>(m_pPagurus_Fork->Get_GrabObject());

	if (nullptr != pPlayer)
	{
		pPlayer->UnGrab(50.f, IDamaged::DAMAGE::DAMAGE_SUPER, m_pGameObject);

		m_pPagurus_Fork->Clear_GrabObject();

		CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
		pUIManager->Set_FadeSpeed(15.f);
		pUIManager->FadeOut();
		
		CGameInstance::GetInstance()->Play_Sound(L"Pagurus_HUNGRY.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, .5f);
	}

	return S_OK;
}

CPagurus_Event_GrabOff* CPagurus_Event_GrabOff::Create(CGameObject* _pGameObject)
{
	CPagurus_Event_GrabOff* pInstance = new CPagurus_Event_GrabOff;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CPagurus_Event_GrabOff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPagurus_Event_GrabOff::Free()
{
	super::Free();
}