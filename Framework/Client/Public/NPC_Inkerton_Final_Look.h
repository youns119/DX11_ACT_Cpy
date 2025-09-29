#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CNPC_Inkerton_Final_Look final
	: public CEvent
{
	using super = CEvent;

private:
	CNPC_Inkerton_Final_Look();
	virtual ~CNPC_Inkerton_Final_Look() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

public:
	static CNPC_Inkerton_Final_Look* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END