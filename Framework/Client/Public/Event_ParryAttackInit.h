#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CEvent_ParryAttackInit final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_ParryAttackInit();
	virtual ~CEvent_ParryAttackInit() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

public:
	static CEvent_ParryAttackInit* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END