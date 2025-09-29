#include "pch.h"
#include "Event_MantisPunch_PostImpact.h"

#include "GameInstance.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "CineCamera_Player.h"
#include "Movement.h"

CEvent_MantisPunch_PostImpact::CEvent_MantisPunch_PostImpact()
{
}

HRESULT CEvent_MantisPunch_PostImpact::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	return S_OK;
}

HRESULT CEvent_MantisPunch_PostImpact::Execute()
{
	_vec3 vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

	m_pMovement->Set_RotateToDirection(false);
	m_pMovement->Add_Force(vLook._vector(), -15.f);

	m_pPlayer->Get_PlayerCineCamera()->Resume();

	m_pPlayer->Get_PlayerCineCamera()->RotationShake(0.1f, 20.f, 2.f);
	m_pPlayer->Get_PlayerCineCamera()->PositionShake(0.1f, 20.f, 2.f);

	CGameInstance::GetInstance()->Call_Lag(0.01f, 1.f);

	m_pPlayer->Play_PlayerSound(L"MantisPunch_Impact");
	m_pPlayer->Play_PlayerSound(L"MantisPunch_Impact2");

	return S_OK;
}

CEvent_MantisPunch_PostImpact* CEvent_MantisPunch_PostImpact::Create(CGameObject* _pGameObject)
{
	CEvent_MantisPunch_PostImpact* pInstance = new CEvent_MantisPunch_PostImpact;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_MantisPunch_PostImpact");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_MantisPunch_PostImpact::Free()
{
	super::Free();
}