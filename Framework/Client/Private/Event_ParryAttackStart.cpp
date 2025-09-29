#include "pch.h"
#include "Event_ParryAttackStart.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "CineCamera_Player.h"
#include "Transform.h"
#include "Movement.h"

CEvent_ParryAttackStart::CEvent_ParryAttackStart()
{
}

HRESULT CEvent_ParryAttackStart::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_ParryAttackStart::Execute()
{
	_vec3 vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	m_pMovement->Add_Force(vLook._vector(), 15.f);

	m_pPlayer->Get_PlayerCineCamera()->Resume();

	return S_OK;
}

CEvent_ParryAttackStart* CEvent_ParryAttackStart::Create(CGameObject* _pGameObject)
{
	CEvent_ParryAttackStart* pInstance = new CEvent_ParryAttackStart;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_ParryAttackStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_ParryAttackStart::Free()
{
	super::Free();
}