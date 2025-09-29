#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CMovement;

END

BEGIN(Client)

class CPlayer;

class CEvent_ParryAttackEnd final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_ParryAttackEnd();
	virtual ~CEvent_ParryAttackEnd() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CMovement* m_pMovement{};

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_ParryAttackEnd* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END