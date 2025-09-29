#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CEvent_GrappleAttack_BackFlip2 final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_GrappleAttack_BackFlip2();
	virtual ~CEvent_GrappleAttack_BackFlip2() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

public:
	static CEvent_GrappleAttack_BackFlip2* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END