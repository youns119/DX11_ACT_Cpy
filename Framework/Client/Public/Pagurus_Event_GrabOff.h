#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CPagurus_Fork;

class CPagurus_Event_GrabOff final 
	: public CEvent
{
	using super = CEvent;

private:
	CPagurus_Event_GrabOff();
	virtual ~CPagurus_Event_GrabOff() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	HRESULT Execute() override;
	
private :
	CPagurus_Fork*	m_pPagurus_Fork{};

public:
	static CPagurus_Event_GrabOff* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	