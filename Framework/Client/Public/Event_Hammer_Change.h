#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CPlayer;

class CEvent_Hammer_Change final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_Hammer_Change();
	virtual ~CEvent_Hammer_Change() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_Hammer_Change* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END