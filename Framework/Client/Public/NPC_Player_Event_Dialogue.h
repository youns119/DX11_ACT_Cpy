#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CNPC_Player_Event_Dialogue final
	: public CEvent
{
	using super = CEvent;

private:
	CNPC_Player_Event_Dialogue();
	virtual ~CNPC_Player_Event_Dialogue() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

public:
	static CNPC_Player_Event_Dialogue* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END