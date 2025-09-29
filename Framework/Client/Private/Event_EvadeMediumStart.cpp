#include "pch.h"
#include "Event_EvadeMediumStart.h"

#include "Player.h"
#include "Movement.h"

CEvent_EvadeMediumStart::CEvent_EvadeMediumStart()
{
}

HRESULT CEvent_EvadeMediumStart::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayerStat = static_cast<CPlayer*>(_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_EvadeMediumStart::Execute()
{
	m_pMovement->Reset_Velocity();

	_vec3 vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	m_pMovement->Add_Force(vLook._vector(), 13.f);

	return S_OK;
}

CEvent_EvadeMediumStart* CEvent_EvadeMediumStart::Create(CGameObject* _pGameObject)
{
	CEvent_EvadeMediumStart* pInstance = new CEvent_EvadeMediumStart;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_EvadeMediumStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_EvadeMediumStart::Free()
{
	super::Free();
}