#include "pch.h"
#include "Event_HammerSwipeAttackStart.h"

#include "GameInstance.h"

#include "Player.h"
#include "Transform.h"
#include "Movement.h"

CEvent_HammerSwipeAttackStart::CEvent_HammerSwipeAttackStart()
{
}

HRESULT CEvent_HammerSwipeAttackStart::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_HammerSwipeAttackStart::Execute()
{
	m_pPlayer->Get_PlayerStat()->fAnimSpeed = 1.f;

	_float3 vSwipeDir = m_pMovement->Get_InputDir();
	_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);

	if (_vec3(vSwipeDir).Length() == 0.f)
		return S_OK;

	m_pTransform->LookAt(vPosition + XMLoadFloat3(&vSwipeDir));
	m_pMovement->Add_Force(XMLoadFloat3(&vSwipeDir), 15.f);

	m_pPlayer->Play_PlayerSound(L"Hammer_SwipeAttack");
	m_pPlayer->Play_PlayerSound(L"SwipeAttack_Bubble");
	m_pPlayer->Play_PlayerSound(L"Hammer_SwipeAttack_Voice");

	return S_OK;
}

CEvent_HammerSwipeAttackStart* CEvent_HammerSwipeAttackStart::Create(CGameObject* _pGameObject)
{
	CEvent_HammerSwipeAttackStart* pInstance = new CEvent_HammerSwipeAttackStart;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_HammerSwipeAttackStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_HammerSwipeAttackStart::Free()
{
	super::Free();
}