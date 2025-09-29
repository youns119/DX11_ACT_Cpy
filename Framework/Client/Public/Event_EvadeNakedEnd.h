#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CMovement;

END

BEGIN(Client)

struct FPlayerStat;

class CEvent_EvadeNakedEnd final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_EvadeNakedEnd();
	virtual ~CEvent_EvadeNakedEnd() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CMovement* m_pMovement{};

private :
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_EvadeNakedEnd* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END