#include "pch.h"
#include "Event_MantisPunch_TimeSlow2.h"

#include "GameInstance.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "CineCamera_Player.h"
#include "Movement.h"

CEvent_MantisPunch_TimeSlow2::CEvent_MantisPunch_TimeSlow2()
{
}

HRESULT CEvent_MantisPunch_TimeSlow2::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_MantisPunch_TimeSlow2::Execute()
{
	CGameInstance::GetInstance()->Call_Lag(0.08f, 0.6f);

	return S_OK;
}

CEvent_MantisPunch_TimeSlow2* CEvent_MantisPunch_TimeSlow2::Create(CGameObject* _pGameObject)
{
	CEvent_MantisPunch_TimeSlow2* pInstance = new CEvent_MantisPunch_TimeSlow2;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_MantisPunch_TimeSlow2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_MantisPunch_TimeSlow2::Free()
{
	super::Free();
}