#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CTransform;
class CMovement;

END

BEGIN(Client)

class CPlayer;

class CEvent_MantisPunch_TimeSlow2 final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_MantisPunch_TimeSlow2();
	virtual ~CEvent_MantisPunch_TimeSlow2() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_MantisPunch_TimeSlow2* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END