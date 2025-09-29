#include "pch.h"
#include "Event_BackStepAttackStart.h"

#include "GameInstance.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"
#include "CineCamera_Player.h"
#include "Transform.h"
#include "Movement.h"

CEvent_BackStepAttackStart::CEvent_BackStepAttackStart()
{
}

HRESULT CEvent_BackStepAttackStart::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_BackStepAttackStart::Execute()
{
	_vec3 vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

	m_pMovement->Set_ForceFriction(m_pPlayer->Get_PlayerStat()->fForceFriction * 4.f);
	m_pMovement->Add_Force(vLook._vector(), 30.f);

	m_pPlayer->Get_PlayerStat()->bInvincible = true;

	m_pPlayer->Get_PlayerCineCamera()->Resume();

	return S_OK;
}

CEvent_BackStepAttackStart* CEvent_BackStepAttackStart::Create(CGameObject* _pGameObject)
{
	CEvent_BackStepAttackStart* pInstance = new CEvent_BackStepAttackStart;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_BackStepAttackStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_BackStepAttackStart::Free()
{
	super::Free();
}