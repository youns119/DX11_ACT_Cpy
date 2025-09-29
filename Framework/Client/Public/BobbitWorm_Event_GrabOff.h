#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CBobbitWorm_Mouth;

class CBobbitWorm_Event_GrabOff final 
	: public CEvent
{
	using super = CEvent;

private:
	CBobbitWorm_Event_GrabOff();
	virtual ~CBobbitWorm_Event_GrabOff() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	HRESULT Execute() override;
	
private :
	CBobbitWorm_Mouth* m_pMouth{};	

public:
	static CBobbitWorm_Event_GrabOff* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	