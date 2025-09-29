#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CMovement;

END

BEGIN(Client)

class CPlayer;

class CEvent_PlungeAttackStart final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_PlungeAttackStart();
	virtual ~CEvent_PlungeAttackStart() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private :
	CMovement* m_pMovement{};

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_PlungeAttackStart* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END