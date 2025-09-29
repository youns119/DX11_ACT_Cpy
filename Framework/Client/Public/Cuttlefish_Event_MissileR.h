#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CCuttlefish_Event_MissileR final : public CEvent
{
private:
	CCuttlefish_Event_MissileR();
	virtual ~CCuttlefish_Event_MissileR() = default;

public:
	HRESULT Execute() override;

public:
	static CCuttlefish_Event_MissileR* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	