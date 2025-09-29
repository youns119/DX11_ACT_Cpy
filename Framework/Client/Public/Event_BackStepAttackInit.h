#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CEvent_BackStepAttackInit final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_BackStepAttackInit();
	virtual ~CEvent_BackStepAttackInit() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

public:
	static CEvent_BackStepAttackInit* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END