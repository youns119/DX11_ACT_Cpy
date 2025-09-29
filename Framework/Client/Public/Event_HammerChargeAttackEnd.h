#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CPlayer;

class CEvent_HammerChargeAttackEnd final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_HammerChargeAttackEnd();
	virtual ~CEvent_HammerChargeAttackEnd() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_HammerChargeAttackEnd* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END