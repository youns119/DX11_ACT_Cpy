#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CCuttlefish_GrabTentacle;

class CCuttlefish_Event_GrabOff final 
	: public CEvent
{
	using super = CEvent;

private:
	CCuttlefish_Event_GrabOff();
	virtual ~CCuttlefish_Event_GrabOff() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	HRESULT Execute() override;
	
private :
	CCuttlefish_GrabTentacle* m_pGrabTentacle{ nullptr };

public:
	static CCuttlefish_Event_GrabOff* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	