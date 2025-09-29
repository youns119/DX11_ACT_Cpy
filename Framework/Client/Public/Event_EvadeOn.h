#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

struct FPlayerStat;

class CEvent_EvadeOn final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_EvadeOn();
	virtual ~CEvent_EvadeOn() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_EvadeOn* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END