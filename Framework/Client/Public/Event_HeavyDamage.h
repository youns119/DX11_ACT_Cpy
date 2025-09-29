#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

struct FPlayerStat;

class CEvent_HeavyDamage final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_HeavyDamage();
	virtual ~CEvent_HeavyDamage() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_HeavyDamage* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END