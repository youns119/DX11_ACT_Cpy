#include "pch.h"
#include "Event_MantisPunch_Impact.h"

#include "GameInstance.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "CineCamera_Player.h"

CEvent_MantisPunch_Impact::CEvent_MantisPunch_Impact()
{
}

HRESULT CEvent_MantisPunch_Impact::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_MantisPunch_Impact::Execute()
{
	m_pPlayer->Get_PlayerCineCamera()->RotationShake(0.06f, 25.f, 0.6f);
	m_pPlayer->Get_PlayerCineCamera()->PositionShake(0.06f, 25.f, 0.6f);

	CGameInstance::GetInstance()->Call_Lag(0.1f, 0.8f);

	m_pPlayer->Play_PlayerSound(L"Parry_Attack");
	m_pPlayer->Play_PlayerSound(L"NormalAttack_Bubble");
	m_pPlayer->Play_PlayerSound(L"MantisPunch_Voice2");

	return S_OK;
}

CEvent_MantisPunch_Impact* CEvent_MantisPunch_Impact::Create(CGameObject* _pGameObject)
{
	CEvent_MantisPunch_Impact* pInstance = new CEvent_MantisPunch_Impact;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_MantisPunch_Impact");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_MantisPunch_Impact::Free()
{
	super::Free();
}