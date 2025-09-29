#include "pch.h"
#include "Event_HammerAttackStart.h"

#include "GameInstance.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"
#include "Transform.h"
#include "Movement.h"

CEvent_HammerAttackStart::CEvent_HammerAttackStart()
{
}

HRESULT CEvent_HammerAttackStart::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_HammerAttackStart::Execute()
{
	m_pPlayer->Player_CameraShake_Rotation(0.02f, 5.f, 0.2f);

	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	m_pMovement->Add_Force(vLook, 11.f);

	switch (m_pPlayer->Get_PlayerStat()->iComboCount)
	{
	case 2:
		m_pPlayer->Play_PlayerSound(L"Hammer_NormalAttack1");
		break;
	case 3:
		m_pPlayer->Play_PlayerSound(L"Hammer_NormalAttack2");
		m_pPlayer->Play_PlayerSound(L"Hammer_NormalAttack_Voice");
		break;
	default:
		break;
	}

	m_pPlayer->Play_PlayerSound(L"NormalAttack_Bubble");

	return S_OK;
}

CEvent_HammerAttackStart* CEvent_HammerAttackStart::Create(CGameObject* _pGameObject)
{
	CEvent_HammerAttackStart* pInstance = new CEvent_HammerAttackStart;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_HammerAttackStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_HammerAttackStart::Free()
{
	super::Free();
}