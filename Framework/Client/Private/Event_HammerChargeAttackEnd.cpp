#include "pch.h"
#include "Event_HammerChargeAttackEnd.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"

CEvent_HammerChargeAttackEnd::CEvent_HammerChargeAttackEnd()
{
}

HRESULT CEvent_HammerChargeAttackEnd::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_HammerChargeAttackEnd::Execute()
{
	m_pPlayer->Player_CameraShake_Rotation(0.05f, 5.f, 0.2f);

	return S_OK;
}

CEvent_HammerChargeAttackEnd* CEvent_HammerChargeAttackEnd::Create(CGameObject* _pGameObject)
{
	CEvent_HammerChargeAttackEnd* pInstance = new CEvent_HammerChargeAttackEnd;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_HammerChargeAttackEnd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_HammerChargeAttackEnd::Free()
{
	super::Free();
}