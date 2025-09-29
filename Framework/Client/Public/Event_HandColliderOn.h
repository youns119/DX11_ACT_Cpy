#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CPlayer;

class CEvent_HandColliderOn final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_HandColliderOn();
	virtual ~CEvent_HandColliderOn() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_HandColliderOn* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END