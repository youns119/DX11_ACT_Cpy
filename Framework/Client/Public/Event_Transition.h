#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

struct FPlayerStat;

class CEvent_Transition final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_Transition();
	virtual ~CEvent_Transition() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_Transition* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END