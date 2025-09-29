#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CCinematicCamera;

class CEvent_Cinematic_Shake final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_Cinematic_Shake();
	virtual ~CEvent_Cinematic_Shake() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private :
	CCinematicCamera* m_pCinematicCamera{};

public:
	static CEvent_Cinematic_Shake* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END