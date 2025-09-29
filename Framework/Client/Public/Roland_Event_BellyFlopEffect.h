#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CRoland_Event_BellyFlopEffect final : public CEvent
{
private:
	CRoland_Event_BellyFlopEffect(CGameObject* _pGameObject);
	virtual ~CRoland_Event_BellyFlopEffect() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4* m_pCombinedWorldMatrix;

public:
	static CRoland_Event_BellyFlopEffect* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END