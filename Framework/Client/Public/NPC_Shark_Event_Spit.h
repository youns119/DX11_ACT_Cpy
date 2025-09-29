#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CNPC_Shark_Event_Spit final
	: public CEvent
{
	using super = CEvent;

private:
	CNPC_Shark_Event_Spit();
	virtual ~CNPC_Shark_Event_Spit() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

public:
	static CNPC_Shark_Event_Spit* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END