#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CPetroch_Event_DivePunch final : public CEvent
{
private:
	CPetroch_Event_DivePunch();
	virtual ~CPetroch_Event_DivePunch() = default;

public:
	HRESULT Execute() override;

public:
	static CPetroch_Event_DivePunch* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	