#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CSplitter_LeftClaw;

class CSplitter_Event_GrabOff final 
	: public CEvent
{
	using super = CEvent;

private:
	CSplitter_Event_GrabOff();
	virtual ~CSplitter_Event_GrabOff() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	HRESULT Execute() override;
	
private :
	CSplitter_LeftClaw* m_pClaw{};

public:
	static CSplitter_Event_GrabOff* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	