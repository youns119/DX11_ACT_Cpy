#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CPetroch_Event_Pounce final : public CEvent
{
private:
	CPetroch_Event_Pounce();
	virtual ~CPetroch_Event_Pounce() = default;

public:
	HRESULT Execute() override;

public:
	static CPetroch_Event_Pounce* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	