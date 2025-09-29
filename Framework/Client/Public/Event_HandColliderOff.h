#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CPlayer;

class CEvent_HandColliderOff final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_HandColliderOff();
	virtual ~CEvent_HandColliderOff() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_HandColliderOff* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END