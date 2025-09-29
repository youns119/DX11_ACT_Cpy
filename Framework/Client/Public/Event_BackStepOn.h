#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

struct FPlayerStat;

class CEvent_BackStepOn final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_BackStepOn();
	virtual ~CEvent_BackStepOn() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_BackStepOn* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END