#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CBowman_Event_Reload final : public CEvent
{
private:
	CBowman_Event_Reload();
	virtual ~CBowman_Event_Reload() = default;

public:
	HRESULT Execute() override;


public:
	static CBowman_Event_Reload* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	