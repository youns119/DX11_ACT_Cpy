#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

struct FPlayerStat;

class CEvent_ParryOff final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_ParryOff();
	virtual ~CEvent_ParryOff() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject) override;

public:
	virtual HRESULT Execute() override;

private:
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_ParryOff* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END