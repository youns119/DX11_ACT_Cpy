#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CPlayer;

class CEvent_MantisPunch_Impact final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_MantisPunch_Impact();
	virtual ~CEvent_MantisPunch_Impact() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_MantisPunch_Impact* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END