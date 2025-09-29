#include "pch.h"
#include "Event_HammerChargeAttackStart.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"
#include "Transform.h"
#include "Movement.h"

CEvent_HammerChargeAttackStart::CEvent_HammerChargeAttackStart()
{
}

HRESULT CEvent_HammerChargeAttackStart::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_HammerChargeAttackStart::Execute()
{
	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

	m_pMovement->Add_Force(vLook, 12.f);

	return S_OK;
}

CEvent_HammerChargeAttackStart* CEvent_HammerChargeAttackStart::Create(CGameObject* _pGameObject)
{
	CEvent_HammerChargeAttackStart* pInstance = new CEvent_HammerChargeAttackStart;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_HammerChargeAttackStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_HammerChargeAttackStart::Free()
{
	super::Free();
}