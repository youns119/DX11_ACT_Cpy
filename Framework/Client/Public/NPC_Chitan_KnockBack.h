#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CNPC_Chitan_KnockBack final
	: public CEvent
{
	using super = CEvent;

private:
	CNPC_Chitan_KnockBack();
	virtual ~CNPC_Chitan_KnockBack() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

public:
	static CNPC_Chitan_KnockBack* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END