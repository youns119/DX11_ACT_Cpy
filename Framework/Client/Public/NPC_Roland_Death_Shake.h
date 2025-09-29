#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CNPC_Roland_Death_Shake final
	: public CEvent
{
	using super = CEvent;

private:
	CNPC_Roland_Death_Shake();
	virtual ~CNPC_Roland_Death_Shake() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

public:
	static CNPC_Roland_Death_Shake* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END