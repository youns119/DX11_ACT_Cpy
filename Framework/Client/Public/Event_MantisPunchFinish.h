#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CPlayer;

class CEvent_MantisPunchFinish final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_MantisPunchFinish();
	virtual ~CEvent_MantisPunchFinish() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_MantisPunchFinish* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END