#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

struct FPlayerStat;

class CEvent_ChargeOn final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_ChargeOn();
	virtual ~CEvent_ChargeOn() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_ChargeOn* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END