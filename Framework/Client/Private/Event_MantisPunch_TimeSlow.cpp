#include "pch.h"
#include "Event_MantisPunch_TimeSlow.h"

#include "GameInstance.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "CineCamera_Player.h"
#include "Movement.h"

CEvent_MantisPunch_TimeSlow::CEvent_MantisPunch_TimeSlow()
{
}

HRESULT CEvent_MantisPunch_TimeSlow::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_MantisPunch_TimeSlow::Execute()
{
	CGameInstance::GetInstance()->Call_Lag(0.08f, 0.6f);

	m_pPlayer->Get_PlayerCineCamera()->Resume();

	m_pPlayer->Play_PlayerSound(L"Parry_Success");
	m_pPlayer->Play_PlayerSound(L"NormalAttack_Bubble");

	return S_OK;
}

CEvent_MantisPunch_TimeSlow* CEvent_MantisPunch_TimeSlow::Create(CGameObject* _pGameObject)
{
	CEvent_MantisPunch_TimeSlow* pInstance = new CEvent_MantisPunch_TimeSlow;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_MantisPunch_TimeSlow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_MantisPunch_TimeSlow::Free()
{
	super::Free();
}