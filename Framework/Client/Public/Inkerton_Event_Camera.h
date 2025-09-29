#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CInkerton_Event_Camera final : public CEvent
{
private:
	CInkerton_Event_Camera();
	virtual ~CInkerton_Event_Camera() = default;

public:
	HRESULT Execute() override;
	
private:


public:
	static CInkerton_Event_Camera* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	