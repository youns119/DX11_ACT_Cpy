#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CInkerton_Event_InkSpurt final : public CEvent
{
private:
	CInkerton_Event_InkSpurt();
	virtual ~CInkerton_Event_InkSpurt() = default;

public:
	HRESULT Execute() override;

public:
	static CInkerton_Event_InkSpurt* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	