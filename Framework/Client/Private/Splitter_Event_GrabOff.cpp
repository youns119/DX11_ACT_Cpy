#include "pch.h"
#include "Splitter_Event_GrabOff.h"

#include "GameInstance.h"

#include "Splitter_LeftClaw.h"
#include "Player.h"
#include "UI_Manager.h"

CSplitter_Event_GrabOff::CSplitter_Event_GrabOff()
{
}

HRESULT CSplitter_Event_GrabOff::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		E_FAIL;

	m_pClaw = static_cast<CSplitter_LeftClaw*>(_pGameObject->Find_PartObject({ L"Part_LeftClaw" }));

	return S_OK;
}

HRESULT CSplitter_Event_GrabOff::Execute()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pClaw->Get_GrabObject());

	CTransform* pTransform = static_cast<CTransform*>(pPlayer->Find_Component(g_strTransformTag));
	_vector vLook = pTransform->Get_State(CTransform::STATE::LOOK);

	pPlayer->UnGrab(20.f, IDamaged::DAMAGE::DAMAGE_HEAVY, m_pGameObject);

	m_pClaw->Clear_GrabObject();	

	return S_OK;
}

CSplitter_Event_GrabOff* CSplitter_Event_GrabOff::Create(CGameObject* _pGameObject)
{
	CSplitter_Event_GrabOff* pInstance = new CSplitter_Event_GrabOff;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CSplitter_Event_GrabOff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_Event_GrabOff::Free()
{
	super::Free();
}