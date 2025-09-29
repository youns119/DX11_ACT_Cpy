#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

struct FPlayerStat;

class CEvent_ParryOn final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_ParryOn();
	virtual ~CEvent_ParryOn() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject) override;

public:
	virtual HRESULT Execute() override;

private:
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_ParryOn* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END