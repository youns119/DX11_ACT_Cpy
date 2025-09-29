#include "pch.h"
#include "Event_SwipeAttackStart.h"

#include "GameInstance.h"
#include "Player.h"
#include "Transform.h"
#include "Movement.h"

CEvent_SwipeAttackStart::CEvent_SwipeAttackStart()
{
}

HRESULT CEvent_SwipeAttackStart::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);
	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_SwipeAttackStart::Execute()
{
	m_pPlayerStat->fAnimSpeed = 1.f;

	_float3 vSwipeDir = m_pMovement->Get_InputDir();
	_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vUp = m_pTransform->Get_State(CTransform::STATE::UP);
	_vector vRight = m_pTransform->Get_State(CTransform::STATE::RIGHT);

	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_SWIPE_ATT, XMVectorSet(0.f, 0.f, 0.f, 1.f) + vUp * 0.5f, vRight, true, &m_pTransform->Get_WorldMatrix_Ref(), 0.f, true);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_SWIPE_ATT, XMVectorSet(0.f, 0.f, 0.f, 1.f) + vUp * 0.5f, -vRight, true, &m_pTransform->Get_WorldMatrix_Ref(), 0.f, true);

	m_pPlayer->Play_PlayerSound(L"SwipeAttack");
	m_pPlayer->Play_PlayerSound(L"SwipeAttack_Bubble");
	m_pPlayer->Play_PlayerSound(L"SwipeAttack_Voice");

	if (_vec3(vSwipeDir).Length() == 0.f)
		return S_OK;

	m_pTransform->LookAt(vPosition + XMLoadFloat3(&vSwipeDir));
	m_pMovement->Add_Force(XMLoadFloat3(&vSwipeDir), 15.f);
	m_pMovement->Set_MaxFallingSpeed(m_pPlayerStat->fMaxFallingSpeed);

	return S_OK;
}

CEvent_SwipeAttackStart* CEvent_SwipeAttackStart::Create(CGameObject* _pGameObject)
{
	CEvent_SwipeAttackStart* pInstance = new CEvent_SwipeAttackStart;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_SwipeAttackStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_SwipeAttackStart::Free()
{
	super::Free();
}