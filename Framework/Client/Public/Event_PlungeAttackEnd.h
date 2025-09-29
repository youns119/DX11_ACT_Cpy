#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CMovement;

END

BEGIN(Client)

class CPlayer;

class CEvent_PlungeAttackEnd final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_PlungeAttackEnd();
	virtual ~CEvent_PlungeAttackEnd() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CMovement* m_pMovement{};

private :
	CPlayer* m_pPlayer{};

public:
	static CEvent_PlungeAttackEnd* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END