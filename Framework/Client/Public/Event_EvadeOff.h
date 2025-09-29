#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

struct FPlayerStat;

class CEvent_EvadeOff final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_EvadeOff();
	virtual ~CEvent_EvadeOff() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_EvadeOff* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END