#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CInkerton_Nutcracker;
class CInkerton_StretchedNutcracker;

class CInkerton_Event_GrabOff final 
	: public CEvent
{
	using super = CEvent;

private:
	CInkerton_Event_GrabOff();
	virtual ~CInkerton_Event_GrabOff() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	HRESULT Execute() override;
	
private :
	CInkerton_Nutcracker*			m_pNutcracker{};
	CInkerton_StretchedNutcracker*	m_pStretchedNutcracker{};

public:
	static CInkerton_Event_GrabOff* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	