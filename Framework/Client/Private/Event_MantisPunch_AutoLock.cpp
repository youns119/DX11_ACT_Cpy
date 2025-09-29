#include "pch.h"
#include "Event_MantisPunch_AutoLock.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "CineCamera_Player.h"
#include "Movement.h"

CEvent_MantisPunch_AutoLock::CEvent_MantisPunch_AutoLock()
{
}

HRESULT CEvent_MantisPunch_AutoLock::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	return S_OK;
}

HRESULT CEvent_MantisPunch_AutoLock::Execute()
{
	_vec3 vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

	m_pMovement->Add_Force(vLook._vector(), 10.f);

	m_pPlayer->Set_AutoLock(true, 30.f);

	m_pPlayer->Get_PlayerCineCamera()->Stop_Cinematic();

	return S_OK;
}

CEvent_MantisPunch_AutoLock* CEvent_MantisPunch_AutoLock::Create(CGameObject* _pGameObject)
{
	CEvent_MantisPunch_AutoLock* pInstance = new CEvent_MantisPunch_AutoLock;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_MantisPunch_AutoLock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_MantisPunch_AutoLock::Free()
{
	super::Free();
}