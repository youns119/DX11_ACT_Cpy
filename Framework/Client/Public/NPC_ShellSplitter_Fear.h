#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CNPC_ShellSplitter_Fear final
	: public CEvent
{
	using super = CEvent;

private:
	CNPC_ShellSplitter_Fear();
	virtual ~CNPC_ShellSplitter_Fear() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

public:
	static CNPC_ShellSplitter_Fear* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END