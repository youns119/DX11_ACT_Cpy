#include "pch.h"
#include "Event_ChargeAttackStart.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Movement.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"

CEvent_ChargeAttackStart::CEvent_ChargeAttackStart()
{
}

HRESULT CEvent_ChargeAttackStart::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_ChargeAttackStart::Execute()
{
	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

	m_pPlayer->Player_CameraShake_Rotation(0.02f, 5.f, 0.2f);
	m_pMovement->Add_Force(vLook, 20.f);

	m_pPlayer->Play_PlayerSound(L"ChargeAttack_Bubble");
	m_pPlayer->Play_PlayerSound(L"ChargeAttack");
	m_pPlayer->Play_PlayerSound(L"ChargeAttack_Voice");

	return S_OK;
}

CEvent_ChargeAttackStart* CEvent_ChargeAttackStart::Create(CGameObject* _pGameObject)
{
	CEvent_ChargeAttackStart* pInstance = new CEvent_ChargeAttackStart;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_ChargeAttackStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_ChargeAttackStart::Free()
{
	super::Free();
}