#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CInkerton_Event_Fire final : public CEvent
{
private:
	CInkerton_Event_Fire();
	virtual ~CInkerton_Event_Fire() = default;

public:
	HRESULT Execute() override;

public:
	static CInkerton_Event_Fire* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	