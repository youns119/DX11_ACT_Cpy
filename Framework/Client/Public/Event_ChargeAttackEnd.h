#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CMovement;

END

BEGIN(Client)

class CPlayer;

class CEvent_ChargeAttackEnd final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_ChargeAttackEnd();
	virtual ~CEvent_ChargeAttackEnd() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CMovement* m_pMovement{};

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_ChargeAttackEnd* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END