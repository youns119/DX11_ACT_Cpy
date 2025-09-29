#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CPlayer;

class CEvent_BackStepAttackFinish final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_BackStepAttackFinish();
	virtual ~CEvent_BackStepAttackFinish() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_BackStepAttackFinish* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END