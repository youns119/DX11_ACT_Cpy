#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CTransform;
class CMovement;

END

BEGIN(Client)

class CPlayer;

class CEvent_MantisPunch_TimeSlow final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_MantisPunch_TimeSlow();
	virtual ~CEvent_MantisPunch_TimeSlow() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_MantisPunch_TimeSlow* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END