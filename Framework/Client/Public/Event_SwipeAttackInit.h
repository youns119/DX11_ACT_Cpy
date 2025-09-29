#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

struct FPlayerStat;

class CEvent_SwipeAttackInit final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_SwipeAttackInit();
	virtual ~CEvent_SwipeAttackInit() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_SwipeAttackInit* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END