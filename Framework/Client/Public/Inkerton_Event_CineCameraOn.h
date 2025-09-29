#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CCineCamera;

END

BEGIN(Client)

class CInkerton_Event_CineCameraOn final 
	: public CEvent
{
	using super = CEvent;

private:
	CInkerton_Event_CineCameraOn();
	virtual ~CInkerton_Event_CineCameraOn() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	HRESULT Execute() override;
	
private:
	CCineCamera* m_pCineCamera{};	

public:
	static CInkerton_Event_CineCameraOn* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	