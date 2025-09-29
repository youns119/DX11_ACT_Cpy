#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CCuttlefish_Event_MissileL final : public CEvent
{
private:
	CCuttlefish_Event_MissileL();
	virtual ~CCuttlefish_Event_MissileL() = default;

public:
	HRESULT Execute() override;

public:
	static CCuttlefish_Event_MissileL* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	