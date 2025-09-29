#include "pch.h"
#include "Event_EvadeNakedStart.h"

#include "Player.h"
#include "Movement.h"

CEvent_EvadeNakedStart::CEvent_EvadeNakedStart()
{
}

HRESULT CEvent_EvadeNakedStart::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayerStat = static_cast<CPlayer*>(_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_EvadeNakedStart::Execute()
{
	m_pMovement->Reset_Velocity();

	_vec3 vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

	if (m_pPlayerStat->bInAir)
	{
		m_pMovement->Set_GravityScale(m_pPlayerStat->fGravityScale * 0.25f);
		m_pMovement->Set_InAirControlRatio(0.7f);
	}

	m_pMovement->Add_Force(vLook._vector(), 15.f);

	return S_OK;
}

CEvent_EvadeNakedStart* CEvent_EvadeNakedStart::Create(CGameObject* _pGameObject)
{
	CEvent_EvadeNakedStart* pInstance = new CEvent_EvadeNakedStart;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_EvadeNakedStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_EvadeNakedStart::Free()
{
	super::Free();
}