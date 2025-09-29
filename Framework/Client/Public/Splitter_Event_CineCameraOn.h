#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CCineCamera;

END

BEGIN(Client)

class CSplitter_Event_CineCameraOn final 
	: public CEvent
{
	using super = CEvent;

private:
	CSplitter_Event_CineCameraOn();
	virtual ~CSplitter_Event_CineCameraOn() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	HRESULT Execute() override;
	
private:
	CCineCamera* m_pCineCamera{};	

public:
	static CSplitter_Event_CineCameraOn* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	