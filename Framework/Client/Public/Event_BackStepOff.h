#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

struct FPlayerStat;

class CEvent_BackStepOff final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_BackStepOff();
	virtual ~CEvent_BackStepOff() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_BackStepOff* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END