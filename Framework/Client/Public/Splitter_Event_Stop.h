#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CSplitter_Event_Stop final : public CEvent
{	
private:
	CSplitter_Event_Stop();
	virtual ~CSplitter_Event_Stop() = default;

public:
	HRESULT Execute() override;
	
public:
	static CSplitter_Event_Stop* Create(CGameObject* _pGameObject);
	virtual void Free() override;
	
};
END	