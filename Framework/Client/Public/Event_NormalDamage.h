#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

struct FPlayerStat;

class CEvent_NormalDamage final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_NormalDamage();
	virtual ~CEvent_NormalDamage() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_NormalDamage* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END