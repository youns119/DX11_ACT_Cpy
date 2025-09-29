#pragma once
#include "Client_Defines.h"
#include "Event.h"
BEGIN(Client)

class CEvent_Effect_FallDown : public CEvent
{
	using super = CEvent;

private:
	CEvent_Effect_FallDown();
	virtual ~CEvent_Effect_FallDown() = default;

public:
	HRESULT Init(CGameObject* pGameObject);

public:
	virtual HRESULT Execute() override;

public:
	static CEvent_Effect_FallDown* Create(CGameObject* pGameObject);
	virtual void Free() override;
};

END