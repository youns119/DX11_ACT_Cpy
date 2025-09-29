#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CPetroch_Event_GroundSlam final : public CEvent
{
private:
	CPetroch_Event_GroundSlam();
	virtual ~CPetroch_Event_GroundSlam() = default;

public:
	HRESULT Execute() override;

public:
	static CPetroch_Event_GroundSlam* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	