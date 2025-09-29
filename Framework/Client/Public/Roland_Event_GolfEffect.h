#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CRoland_Event_GolfEffect final : public CEvent
{
private:
	CRoland_Event_GolfEffect(CGameObject* _pGameObject);
	virtual ~CRoland_Event_GolfEffect() = default;

public:
	HRESULT Execute() override;

public:
	static CRoland_Event_GolfEffect* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END