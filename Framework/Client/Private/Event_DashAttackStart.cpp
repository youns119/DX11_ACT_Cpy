#include "pch.h"
#include "Event_DashAttackStart.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Movement.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"

CEvent_DashAttackStart::CEvent_DashAttackStart()
{
}

HRESULT CEvent_DashAttackStart::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_DashAttackStart::Execute()
{
	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

	m_pPlayer->Player_CameraShake_Rotation(0.02f, 5.f, 0.2f);
	m_pMovement->Add_Force(vLook, 15.f);

	return S_OK;
}

CEvent_DashAttackStart* CEvent_DashAttackStart::Create(CGameObject* _pGameObject)
{
	CEvent_DashAttackStart* pInstance = new CEvent_DashAttackStart;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_DashAttackStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_DashAttackStart::Free()
{
	super::Free();
}