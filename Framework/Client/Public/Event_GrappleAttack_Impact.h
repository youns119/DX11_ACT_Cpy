#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CPlayer;

class CEvent_GrappleAttack_Impact final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_GrappleAttack_Impact();
	virtual ~CEvent_GrappleAttack_Impact() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_GrappleAttack_Impact* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END