#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CNPC_Roland_Death_Move final
	: public CEvent
{
	using super = CEvent;

private:
	CNPC_Roland_Death_Move();
	virtual ~CNPC_Roland_Death_Move() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

public:
	static CNPC_Roland_Death_Move* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END